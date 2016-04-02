#include "util.h"

void setup() {
  for(int i=0; i<=13; i++)
  {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  digitalWrite(2, LOW);
  digitalWrite(7, LOW);

  if(DEBUG) Serial.begin(115200);
  LWiFi.begin();

  debug("Board ID : " + itos(BOARD_ID) + "\n");
  wifi_connect();

  digitalWrite(2, HIGH);

  while(!retrieve_data())
  {
    debug("Retrieve data FAILED!\n");
    delay(2000);
  }

  digitalWrite(2, LOW);
  digitalWrite(7, HIGH);

  while(!calibrate_time())
  {
    debug("Calibrate time FAILED!\n");
    delay(2000);
  }

  debug("Current Time : " + dtos(get_time()) + " s\n");
  debug("From Startup : " + dtos(millis() * 0.001) + " s\n");
  debug("===== Ready =====\n");
} 

void loop() {
  double tm = get_time();
  for(int i=0; i<N_PART; i++)
  {
    int light = curve(get_light(i, tm));
    digitalWrite(PINS[i], rand() % 255 < light ? HIGH : LOW);
  }
}

