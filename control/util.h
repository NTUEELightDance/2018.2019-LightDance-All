#ifndef UTIL_H
#define UTIL_H
#include "Arduino.h"
#include "LWiFi.h"
#include "LWiFiClient.h"
#include <ArduinoJson.h>

#include <cstdlib>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <algorithm>

#define BOARD_ID 3
#define N_PART 10
#define DEBUG 1
#define MAX_LEN 131072
#define BUF_LEN 2048

using namespace std;

static const int PINS[N_PART] = {0, 1, 13, 3, 4, 5, 6, 7, 8, 9};

bool calibrate_time();
double get_time();

bool retrieve_data();
int get_light(int, double);
int curve(int);

struct Seg
{
  double start;
  double end;
  int ltype;

  Seg(double a, double b, int c)
  {
    start = a;
    end = b;
    ltype = c;
  }
};

bool wifi_connected();
void wifi_connect(int retry=5);

class Conn
{
 public:
  bool connect();
  void send(const string s);
  string read();
  string l_read();
  ~Conn()
  {
    c.stop();
  }

 private:
  LWiFiClient c;
};

string itos(int);
string dtos(double);

void debug(const string);

#endif
