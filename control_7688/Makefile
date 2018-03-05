CC = mipsel-openwrt-linux-g++
CFLAGS = -I/home/dodo0822/Downloads/rapidjson-1.1.0/include -fPIC -O3 -Wall
LDFLAGS = -lmraa -lpthread -ldl

%.o: %.cpp
	@echo "  CC  $<"
	@$(CC) -c -o $@ $< $(CFLAGS)

all: control check

control: control.o PCA9685.o
	@echo "  LD  $<"
	@$(CC) -o control control.o PCA9685.o $(LDFLAGS)

check: check.o PCA9685.o
	@echo "  LD  $<"
	@$(CC) -o check check.o PCA9685.o $(LDFLAGS)

.PHONY: clean
clean:
	rm -f control *.o check
