#include "util.h"

vector<Seg> Data[N_PART]; 
extern char* buf;

bool retrieve_data()
{
  wifi_connect();
  if(!wifi_connected())
    return false;

  debug("Start retrieving data...\n");

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
  c.send("D");
  s = c.l_read();

  DynamicJsonBuffer jbuf;
  JsonArray& root = jbuf.parseArray(s.c_str());
  if(!root.success())
  {
    debug("JSON parsing failed!!\n");
    return false;
  }

  for(int i=0; i<N_PART; i++)
  {
    JsonArray& jpart = root[i];
    for(JsonArray::iterator it=jpart.begin(); it!=jpart.end(); ++it)
    {
      JsonArray& jseg = *it;
      Data[i].push_back(Seg(jseg[0], jseg[1], jseg[2]));
    }
    debug(string("Part ") + char('A'+i) + " : " + itos(Data[i].size()) + " Segments\n");
  }

  c.send("S");
  debug("Retrieved data successfully!!\n");

  return true;
}

int get_light(int part, double tm)
{
  int res = 0;
  vector<Seg>& vec = Data[part];
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
    res = 255;
  }
    
  return res;
}
