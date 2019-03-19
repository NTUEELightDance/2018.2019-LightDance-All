#ifdef USE_INTERNAL_PWM
    #include "mraa/common.hpp"
    #include "mraa/pwm.hpp"
#else
    #include "PCA9685.h"
#endif

#include <cstdio>

#include "common.h"
#include "mraa.hpp"

#define SPI_PORT 0
mraa::Spi spi(SPI_PORT);

#ifdef USE_INTERNAL_PWM
    mraa::Pwm pwm_0(18);
    mraa::Pwm pwm_1(19);
#else
    PCA9685 pca(0, 0x40);
#endif

bool light[16];

unsigned int num_ws = 5;
unsigned int num_led[] = {88,96,60,36,36};


int main() {
    #ifdef USE_INTERNAL_PWM
        pwm_0.period_ms(2);
        pwm_1.period_ms(2);
        pwm_0.enable(true);
        pwm_1.enable(true);
    #endif

    spi.mode( mraa::SPI_MODE0 );
    spi.frequency(2000000);

    Config conf;
    conf = read_config();

    puts("Use -1 to all on, -2 to all off.");

    for(int i = 0; i < 16; ++i) {
        light[i] = true;
    }

    while(true) {
        int id;
        for(int i = 0; i < 16; ++i) {
            #ifdef USE_INTERNAL_PWM
                if(i == 0) pwm_0.write(light[i] ? 1.0 : 0.0);
                else if(i == 1) pwm_1.write(light[i] ? 1.0 : 0.0);
            #else
                pca.setPWM(conf.pins[i], 0, (light[i] ? 4095 : 0));
            #endif
        }
        printf("> ");
        if(!(~scanf("%d", &id))) break;
        if(id == -1) {
            for(int i = 0; i < 16; ++i) {
                light[i] = true;
            }
        } else if(id == -2) {
            for(int i = 0; i < 16; ++i) {
                light[i] = false;
            }
        } else {
            id %= 16;
            light[id] = !light[id];
        }
        if (id == -1){
            for(unsigned int i=0;i<num_ws;i++){
                spi.writeByte( (uint8_t)(63) ); // start byte
                spi.writeByte( (uint8_t)(i) ); // i-th gif
                for (unsigned int j=0;j<num_led[i];j++){
                    spi.writeByte( (uint8_t)(6) );
                    spi.writeByte( (uint8_t)(6) );
                    spi.writeByte( (uint8_t)(6) );
                }
            }
        }
        else{
            for(unsigned int i=0;i<num_ws;i++){
                spi.writeByte( (uint8_t)(63) ); // start byte
                spi.writeByte( (uint8_t)(i) ); // i-th gif
                for (unsigned int j=0;j<num_led[i];j++){
                    spi.writeByte( (uint8_t)(0) );
                    spi.writeByte( (uint8_t)(0) );
                    spi.writeByte( (uint8_t)(0) );
                }
            }
        }
    }
    return 0;
}
