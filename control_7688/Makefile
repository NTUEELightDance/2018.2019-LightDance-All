CC = mipsel-openwrt-linux-g++
CFLAGS = -I./rapidjson -fPIC -O3 -Wall
LDFLAGS = -lmraa -lpthread -ldl

%.o: %.cpp
	@echo "  CC  $<"
	@$(CC) -c -o $@ $< $(CFLAGS)

all: control check

control: control.o PCA9685.o
	@echo "  LD  $<"
	@$(CC) -o upload/control control.o PCA9685.o $(LDFLAGS)

check: check.o PCA9685.o
	@echo "  LD  $<"
	@$(CC) -o upload/check check.o PCA9685.o $(LDFLAGS)

.PHONY: clean
clean:
	rm -f *.o upload/control upload/check
