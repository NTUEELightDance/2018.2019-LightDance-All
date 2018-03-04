#include "PCA9685.h"
#include <cstdio>

PCA9685 pca(0, 0x40);

bool light[16];

int PINS[] = { 1, 2, 6, 5, 4, 3, 8, 15, 16, 7, 9, 10, 14, 13, 12, 11 };

int main() {
    for(int i = 0; i < 16; ++i) {
        light[i] = true;
    }

    while(true) {
        int id;
        for(int i = 0; i < 16; ++i) {
            pca.setPWM(PINS[i], 0, (light[i] ? 4095 : 0));
        }
        printf("> ");
        scanf("%d", &id);
        id %= 16;
        light[id] = !light[id];
    }

    return 0;
}
