#include "util.h"

double TIME_BASE = 0;

bool calibrate_time()
{
  wifi_connect();
  if(!wifi_connected())
    return false;

  debug("Start calibrating time...\n");

  Conn c;
  bool res = c.connect();
  if(!res)
    return false;

  string s = c.read();
  if(s[0] != 'N')
    return false;
  c.send(itos(BOARD_ID));
  s = c.read();
  if(s[0] != 'O')
    return false;
  c.send("T");
  s = c.read();
  if(s[0] == 'X')
  {
    debug("Server Not Ready\n");
    return false;
  }

  double ft1 = atof(s.c_str());
  double curtime = millis() * 0.001;
  c.send("S");
  s = c.read();
  double ft2 = atof(s.c_str());
  c.send("S");
  
  double ft = (ft1 + ft2) / 2;
  TIME_BASE = curtime - ft;
  double delay = ft2 - ft1;
  if(delay >= 0.2)
    return false;

  debug("Calibrate time successful!!\n");
  debug("TIME_BASE : " + dtos(TIME_BASE) + " s\n");
  debug("Delay : " + dtos(delay) + " s\n");

  return true;
}

double get_time()
{
  return millis() * 0.001 - TIME_BASE;
}

string itos(int x)
{
  string s;
  s.push_back('0'+(x%10));
  x /= 10;
  while(x)
  {
    s.push_back('0'+(x%10));
    x /= 10;
  }
  reverse(s.begin(), s.end());
  return s;
}

string dtos(double x)
{
  stringstream ss;
  ss << fixed << setprecision(3);
  ss << x;
  string s;
  ss >> s;
  return s;
}
