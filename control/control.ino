#include "util.h"

long int cnt = 0;

void setup() {
  for(int i=0; i<=13; i++)
    pinMode(i, OUTPUT);
  digitalWrite(0, HIGH);
  digitalWrite(1, HIGH);

  Serial.begin(115200);
  LWiFi.begin();

  debug("Board ID : " + itos(BOARD_ID) + "\n");
  wifi_connect();

  digitalWrite(0, LOW);

  while(!retrieve_data())
  {
    debug("Retrieve data FAILED!\n");
    delay(2000);
  }

  digitalWrite(0, HIGH);
  digitalWrite(1, LOW);

  while(!calibrate_time())
  {
    debug("Calibrate time FAILED!\n");
    delay(2000);
  }

  digitalWrite(0, LOW);

  debug("Current Time : " + dtos(get_time()) + " s\n");
  debug("From Startup : " + dtos(millis() * 0.001) + " s\n");
  debug("===== Ready =====\n");
} 

void loop() {
  double tm = get_time();
  for(int i=0; i<N_PART; i++)
  {
    int light = get_light(i, tm);
    bool state = light;
    digitalWrite(PINS[i], state ? HIGH : LOW);
  }
}

