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

#include "PCA9685.h"
#include "mraa/common.hpp"
#include "mraa/gpio.hpp"

#include "common.h"

using namespace rapidjson;
using namespace std;

Config conf;

volatile int led_status = 0;
mraa::Gpio red_led(3);
mraa::Gpio green_led(0);

char buf[1024];
char sendbuf[1024];

string json_str;

bool data_ok = false;
bool time_ok = false;

double TIME_BASE = 0;

PCA9685 pca(0, 0x40);

void *led_loop(void*) {
    bool blink = false;
    red_led.dir(mraa::DIR_OUT);
    green_led.dir(mraa::DIR_OUT);
    red_led.write(0);
    green_led.write(0);
    while(true) {
        blink = !blink;
        switch(led_status) {
        case 0:
            red_led.write(1);
            green_led.write(0);
            break;
        case 1:
            red_led.write(blink);
            green_led.write(0);
            break;
        case 2:
        default:
            red_led.write(0);
            green_led.write(1);
            break;
        }
        usleep(250000);
    }
    pthread_exit(NULL);
}

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

int main() {
    conf = read_config();

    pthread_t pid;
    pthread_create(&pid, NULL, led_loop, NULL);

    while(!data_ok) {
        int sock = init_sock();
        if(sock < 0) {
            cerr << "Connection failed!" << endl;
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

    Document doc;
    doc.Parse(json_str.c_str());

    for(int i = 0; i < NUM_PARTS; i++) {
        const Value& jpart = doc[i];
        for(SizeType j = 0; j < jpart.Size(); ++j) {
            const Value& jseg = jpart[j];
            LD[i].push_back(Seg(jseg[0].GetDouble(), jseg[1].GetDouble(), jseg[2].GetInt()));
        }
        printf("Part %d : %d segments\n", i, LD[i].size());
    }

    led_status = 1;
    while(!time_ok) {
        int sock = init_sock();
        if(sock < 0) {
            cerr << "Connection failed!" << endl;
            return 1;
        }
        strcpy(sendbuf, "T");
        send(sock, sendbuf, strlen(sendbuf), 0);
        ssize_t rv = recv(sock, buf, 1024, 0);
        if(buf[0] == 'X') {
            cerr << "Server not ready!" << endl;
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
            cerr << "Delay too large! (" << delay << ")" << endl;
            sleep(2);
            continue;
        }
        cout.precision(4);
        cout << "Time calibration success!" << endl;
        cout << "TIME_BASE: ";
        printf("%10.4lf\n", TIME_BASE);
        cout << "Delay    : " << delay << endl;
        time_ok = true;
    }

    led_status = 2;
    while(true) {
        double tm = get_sys_time() - TIME_BASE;
        for(int i = 0; i < NUM_PARTS; ++i) {
            int light = get_light(i, tm);
            pca.setPWM(conf.pins[i], 0, light);
        }
    }

    return 0;
}
