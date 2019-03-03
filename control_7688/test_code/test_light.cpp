#include "PCA9685.h"
#include <unistd.h>

PCA9685 pca(0, 0x40);

int main(){
    while(1){
        for (int i=1;i<=16;i++){
            pca.setPWM(i,0,4095);
        }
        sleep(1);
        for (int i=1;i<=16;i++){
            pca.setPWM(i,0,0);
        }
        sleep(1);
    }

    return 0;
}