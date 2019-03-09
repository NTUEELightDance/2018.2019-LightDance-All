#pragma once

#include <string>
#include <fstream>
#include <cstdio>
#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"

#define SERVER_ADDR "192.168.119.2"
#define SERVER_PORT 33117
#define NUM_PARTS 16

using namespace std;
using namespace rapidjson;

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

struct Color
{
	int r;
	int g;
	int b;

	Color(int x, int y, int z)
	{
		r = x;
		g = y;
		b = z;
	}
};

struct Config
{
	vector<int> pins;
	int board_id;
};

Config read_config() {
	Config conf;
	conf.pins = vector<int>(NUM_PARTS, 0);
	FILE *fp = fopen("config.json", "r");
	char readBuffer[65536];
	FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	Document doc;
	doc.ParseStream(is);
	fclose(fp);
	const Value& pinsArray = doc["pins"];
	for(int i = 0; i < NUM_PARTS; ++i) {
		conf.pins[i] = pinsArray[i].GetInt();
	}
	conf.board_id = doc["board_id"].GetInt();
	return conf;
}
