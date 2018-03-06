#include "PCA9685.h"
#include <cstdio>

#include "common.h"

PCA9685 pca(0, 0x40);

bool light[16];

int main() {
    Config conf;
    conf = read_config();

    puts("Use -1 to all on, -2 to all off.");

    for(int i = 0; i < 16; ++i) {
        light[i] = true;
    }

    while(true) {
        int id;
        for(int i = 0; i < 16; ++i) {
            pca.setPWM(conf.pins[i], 0, (light[i] ? 4095 : 0));
        }
        printf("> ");
        scanf("%d", &id);
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
