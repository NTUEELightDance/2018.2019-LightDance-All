
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "mraa/common.hpp"
#include "mraa/gpio.hpp"
#include "mraa.hpp"

#ifdef USE_INTERNAL_PWM
    #include "mraa/pwm.hpp"
#else
    #include "PCA9685.h"
#endif

#include "common.h"

using namespace rapidjson;
using namespace std;

#define SPI_PORT 0
mraa::Spi spi(SPI_PORT);

Config conf;
bool use_local_file = false;

char buf[1024];
char sendbuf[1024];

unsigned int num_ws = 5;
unsigned int num_led[] = {88,96,60,36,36};

string json_str;

bool data_ok = false;
bool time_ok = false;

double TIME_BASE = 0;

#ifdef USE_INTERNAL_PWM
    mraa::Pwm pwm_0(18);
    mraa::Pwm pwm_1(19);
#else
    PCA9685 pca(0, 0x40);
#endif

void init_spi(){
    spi.mode( mraa::SPI_MODE0 );
    spi.frequency(2000000);
}

vector < vector< vector<Color> > > anim;

double get_sys_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (((double) tv.tv_usec) / 1000000);
}

vector<Seg> LD[NUM_PARTS];

int get_light(int part, double tm) {
  int res = 0;
  vector<Seg>& vec = LD[part];
  int S = vec.size();
  if(S == 0) return res;

  int lb = 0, rb = S-1;
  while(lb < rb)
  {
    int mb = (lb + rb + 1) >> 1;
    if(vec[mb].start > tm)
      rb = mb - 1;
    else
      lb = mb;
  }

  Seg& seg = vec[lb];
  if(seg.start <= tm && tm <= seg.end)
  {
    if(seg.ltype == 1)
      res = 4095;
    else if(seg.ltype == 2)
      res = 4095 * (tm - seg.start) / (seg.end - seg.start);
    else if(seg.ltype == 3)
      res = 4095 * (seg.end - tm) / (seg.end - seg.start);
  } 
    
  return res;
}

int init_sock() {
    int sock = 0;
    struct sockaddr_in dest;
    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_ADDR, &dest.sin_addr);
    dest.sin_port = htons(SERVER_PORT);
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock < 0) {
        cerr << "Failed to create socket." << endl;
        return -1;
    }
    if(connect(sock, (struct sockaddr*) &dest, sizeof(dest)) < 0) {
        cerr << "Failed to connect to server." << endl;
        return -1;
    }
    recv(sock, buf, 1024, 0);
    if(buf[0] != 'N') {
        cerr << "Failed to establish a connection." << endl;
        close(sock);
        return -1;
    }
    sprintf(sendbuf, "%d", conf.board_id);
    send(sock, sendbuf, strlen(sendbuf), 0);
    recv(sock, buf, 1024, 0);
    if(buf[0] != 'O') {
        cerr << "Board rejected by server!" << endl;
        close(sock);
        return -1;
    }
    return sock;
}

int main(int argc, char** argv) {
    conf = read_config();

    if(argc > 1) {
        if(strcmp(argv[1], "--local") == 0) use_local_file = true;
    }

    #ifdef USE_INTERNAL_PWM
        pwm_0.period_ms(2);
        pwm_1.period_ms(2);
        pwm_0.enable(true);
        pwm_1.enable(true);
    #endif

    Document doc;

    if(use_local_file) {
        while(!data_ok) {
            FILE *fp = fopen("data.json", "r");
            if(fp < 0) {
                cerr << "Cannot read local file!" << endl;
                sleep(2);
                continue;
            }
            char readBuffer[65536];
            FileReadStream is(fp, readBuffer, sizeof(readBuffer));
            doc.ParseStream(is);
            fclose(fp);
            cout << "Local file read OK" << endl;
            data_ok = true;
        }
    } else {
        while(!data_ok) {
            int sock = init_sock();
            if(sock < 0) {
                cerr << "DATA Connection failed!" << endl;
                sleep(2);
                continue;
            }
            strcpy(sendbuf, "D");
            send(sock, sendbuf, strlen(sendbuf), 0);
            recv(sock, buf, 1024, 0);
            int data_len = atoi(buf);
            cout << "Data length: " << data_len << endl;
            ssize_t bytesRead = 0;
            json_str = "";
            while (bytesRead < data_len) {
                ssize_t rv = recv(sock, buf, 512, 0);
                if(rv == 0) {
                    cerr << "Socket closed gracefully before enough data received" << endl;
                    break;
                } else if(rv < 0) {
                    cerr << "Read error occurred before enough data received" << endl;
                    break;
                }
                bytesRead += rv;
                buf[rv] = '\0';
                json_str += string(buf);
            }
            if(bytesRead < data_len) {
                cerr << "Transmission failed!" << endl;
                close(sock);
                sleep(2);
                continue;
            }
            cout << "Transmission success!" << endl;
            strcpy(sendbuf, "S");
            send(sock, sendbuf, strlen(sendbuf), 0);
            close(sock);
            data_ok = true;
        }
        doc.Parse(json_str.c_str());
    }

    for(int i = 0; i < NUM_PARTS; i++) {
        const Value& jpart = doc[i];
        for(SizeType j = 0; j < jpart.Size(); ++j) {
            const Value& jseg = jpart[j];
            LD[i].push_back(Seg(jseg[0].GetDouble(), jseg[1].GetDouble(), jseg[2].GetInt()));
        }
        printf("Part %d : %d segments\n", i, LD[i].size());
    }

    init_spi();
    Document doc2;
    data_ok = false;
    while(!data_ok){
        FILE *fp = fopen("data_ws.json", "r");
        if (fp<0){
            cerr << "Cannot read local file!" << endl;
        }
        char readBuffer[65536];
        FileReadStream is(fp, readBuffer, sizeof(readBuffer));
        doc2.ParseStream(is);
        fclose(fp);
        cout << "data_ws.json read OK" << endl;
        data_ok = true;
    }

    const Value& jfile = doc2;
    for (SizeType i = 0;i < jfile.Size();i++){
        const Value& jarray = jfile[i];
        vector< vector<Color> > new_frame_list;
        anim.push_back(new_frame_list);
        for (SizeType j = 0 ; j < jarray.Size() ; j++){
            vector<Color> new_frame;
            anim[i].push_back(new_frame);
            const Value& jframe = jarray[j];
            for (SizeType k=0 ; k < jframe.Size() ; k++){
                const Value& jseg = jframe[k];
                anim[i][j].push_back(Color(jseg[0].GetInt(), jseg[1].GetInt(), jseg[2].GetInt() ));
            }
        }
    }
    cout << anim.size() << " LED arrays" << endl;
    for (unsigned int i = 0;i < anim.size();i++){
        cout << "Length of array[" << i << "]: " << anim[i].size() << endl;
    }

    while(!time_ok) {
        int sock = init_sock();
        if(sock < 0) {
            cerr << "TIME Connection failed!" << endl;
            sleep(2);
            continue;
        }
        strcpy(sendbuf, "T");
        send(sock, sendbuf, strlen(sendbuf), 0);
        ssize_t rv = recv(sock, buf, 1024, 0);
        if(buf[0] == 'X') {
            cerr << "TIME Server not ready!" << endl;
            sleep(2);
            continue;
        }
        buf[rv] = '\0';
        double ft1 = atof(buf);
        double curtime = get_sys_time();
        strcpy(sendbuf, "S");
        send(sock, sendbuf, strlen(sendbuf), 0);
        rv = recv(sock, buf, 1024, 0);
        buf[rv] = '\0';
        double ft2 = atof(buf);
        send(sock, sendbuf, strlen(sendbuf), 0);
        double ft = (ft1 + ft2) / 2;
        TIME_BASE = curtime - ft;
        double delay = ft2 - ft1;
        if(delay > 0.2) {
            cerr << "TIME Delay too large! (" << delay << ")" << endl;
            sleep(1);
            continue;
        }
        cout.precision(4);
        cout << "Time calibration success!" << endl;
        cout << "TIME_BASE: ";
        printf("%10.4lf\n", TIME_BASE);
        cout << "Delay    : " << delay << endl;
        time_ok = true;
    }

    int max_length = anim[0].size() - 20;
    int limit_length = anim[0].size();

    int last_frame = -1;
    while(true) {
        double tm = get_sys_time() - TIME_BASE;
        for(int i = 0; i < NUM_PARTS; ++i) {
            int light = get_light(i, tm);
            #ifdef USE_INTERNAL_PWM
                if(i == 0) pwm_0.write(((float) light) / 4095);
                else if(i == 1) pwm_1.write(((float) light) / 4095);
            #else
                pca.setPWM(conf.pins[i], 0, light);
            #endif
        }

        int current_frame = (int)(tm*10);
        if(tm < 0 || current_frame > limit_length){
            for(unsigned int i=0;i<num_ws;i++){
                spi.writeByte( (uint8_t)(63) ); // start byte
                spi.writeByte( (uint8_t)(i) ); // i-th gif
                for (unsigned int j=0;j<num_led[i];j++){
                    spi.writeByte( 0 );
                    spi.writeByte( 0 );
                    spi.writeByte( 0 );
                }
            }
        }
        else if(current_frame > last_frame){
            if(current_frame > max_length){
                // change max light
                for(unsigned int i=0;i<anim.size();i++){
                    spi.writeByte( (uint8_t)(62) ); // start byte
                    spi.writeByte( (uint8_t)(i) ); // i-th gif
                    for (unsigned int j=0;j<anim[i][current_frame].size();j++){
                        spi.writeByte( (uint8_t)anim[i][current_frame][j].r );
                        spi.writeByte( (uint8_t)anim[i][current_frame][j].g );
                        spi.writeByte( (uint8_t)anim[i][current_frame][j].b );
                    }
                }
            }
            else{
                // send ws signal
                for(unsigned int i=0;i<anim.size();i++){
                    spi.writeByte( (uint8_t)(63) ); // start byte
                    spi.writeByte( (uint8_t)(i) ); // i-th gif
                    for (unsigned int j=0;j<anim[i][current_frame].size();j++){
                        spi.writeByte( (uint8_t)anim[i][current_frame][j].r );
                        spi.writeByte( (uint8_t)anim[i][current_frame][j].g );
                        spi.writeByte( (uint8_t)anim[i][current_frame][j].b );
                    }
                }
            }
            last_frame = current_frame;
        }
    }

    return 0;
}
