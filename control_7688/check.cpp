#ifdef USE_INTERNAL_PWM
    #include "mraa/common.hpp"
    #include "mraa/pwm.hpp"
#else
    #include "PCA9685.h"
#endif

#include <cstdio>

#include "common.h"

#ifdef USE_INTERNAL_PWM
    mraa::Pwm pwm_0(18);
    mraa::Pwm pwm_1(19);
#else
    PCA9685 pca(0, 0x40);
#endif

bool light[16];

int main() {
    #ifdef USE_INTERNAL_PWM
        pwm_0.period_ms(2);
        pwm_1.period_ms(2);
        pwm_0.enable(true);
        pwm_1.enable(true);
    #endif

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
    }

    return 0;
}
