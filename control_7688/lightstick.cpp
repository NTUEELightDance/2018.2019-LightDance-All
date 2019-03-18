#include <iostream>

#include "mraa/common.hpp"
#include "mraa/gpio.hpp"
#include "mraa/pwm.hpp"
#include "mraa.hpp"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <unistd.h>

using namespace rapidjson;
using namespace std;

#define SPI_PORT 0

mraa::Spi spi(SPI_PORT);
mraa::Pwm pwm_0(19);

int main(){
	pwm_0.period_ms(2);
	pwm_0.enable(true);
	spi.mode( mraa::SPI_MODE0 );
	spi.frequency(2000000);

	Document doc;
	bool data_ok = false;

	while(!data_ok){
		FILE *fp = fopen("data_ls.json", "r");
		if(fp<0){
			cerr << "Cannot read local file!" << endl;
		}
		char readBuffer[65536];
		FileReadStream is(fp)
	}

	float light = 0;
	while(true){
		pwm_0.write( light/ 4095 );
		light++;
		if(light>=4096) light = 0;

		spi.writeByte()

		usleep(250);
	}
	return 0;
}