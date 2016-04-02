#include "util.h"

//#define WIFI_SSID "dclab"
//#define WIFI_PASS "ntueedclab"
//#define SERVER_IP 192,168,163,182
#define WIFI_SSID "NTUEE"
#define WIFI_PASS "ntueeXDD"
#define SERVER_IP 192,168,1,123
#define SERVER_PORT 33117

char buf[MAX_LEN];

bool wifi_connected()
{
  return LWiFi.status() == LWIFI_STATUS_CONNECTED;
}

void wifi_connect(int retry)
{
  int cnt = 0;
  while(!wifi_connected() && cnt <= retry)
  {
    LWiFi.connectWPA(WIFI_SSID, WIFI_PASS);
    cnt++;

    if(wifi_connected())
      debug("WiFi connected!!\n");
    else
      debug("WiFi connect failed, retry... " + itos(cnt) + "\n");
  }
  if(cnt == retry)
    debug("WiFi connect failed Q__Q\n");
}

bool Conn::connect()
{
  IPAddress ip(SERVER_IP);
  return c.connect(ip, SERVER_PORT);
}

void Conn::send(const string s)
{
  c.print(s.c_str());
  debug("Sent : " + s + "\n");
}

string Conn::read()
{
  uint32_t st = millis();
  string s;
  while(millis() - st <= 2000 && !c.available());
  if(c.available())
  {
    int b = c.readBytesUntil('\0', buf, BUF_LEN);
    buf[b] = '\0';
    s = string(buf);
  }
  debug("Read : " + s + "\n");
  return s;
}

string Conn::l_read()
{
  int len = atoi(read().c_str()) + 1;
  uint32_t st = millis();
  string s;
  while(millis() - st <= 2000 && !c.available());
  for(int i=0; i<len; i+=BUF_LEN)
  {
    int n = min(len-i, BUF_LEN);
    int b = c.readBytesUntil('\0', buf, BUF_LEN);
    buf[b] = '\0';
    s = s + string(buf);
    debug(itos(i+n) + " bytes ...\n");
  }

  debug("Read : " + s.substr(0, min(200u, s.size())) + "\n");
  return s;
}

void debug(const string s)
{
  if(DEBUG)
  {
    Serial.print(s.c_str());
    Serial.flush();
  }
}
