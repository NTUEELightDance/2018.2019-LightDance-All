CC = mipsel-openwrt-linux-g++
CFLAGS = -I/home/dodo0822/Downloads/rapidjson-1.1.0/include -fPIC -O3 -Wall
LDFLAGS = -lmraa -lpthread -ldl

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

control: control.o PCA9685.o
	$(CC) -o control control.o PCA9685.o $(LDFLAGS)

check: check.o PCA9685.o
	$(CC) -o check check.o PCA9685.o $(LDFLAGS)

.PHONY: clean
clean:
	rm control *.o check
