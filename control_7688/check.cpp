#include "PCA9685.h"
#include <cstdio>

#include "common.h"

PCA9685 pca(0, 0x40);

bool light[16];

int main() {
    Config conf;
    conf = read_config();

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
        id %= 16;
        light[id] = !light[id];
    }

    return 0;
}
