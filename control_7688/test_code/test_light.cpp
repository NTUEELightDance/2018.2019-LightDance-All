#include "PCA9685.h"

PCA9685 pca(0, 0x40);

int main(){
    for (int i=1;i<=16;i++){
        pca.setPWM(i,0,4095);
    }
}