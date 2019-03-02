CC = mipsel-openwrt-linux-g++
CFLAGS = -fPIC -O3 -Wall
LDFLAGS = -lpthread -ldl

%.o: %.cpp
	@echo "  CC  $<"
	@$(CC) -c -o $@ $< $(CFLAGS)

all: test_light

test_light: test_light.o PCA9685.o
	@echo "  LD  $<"
	@$(CC) -o upload/test_light test_light.o PCA9685.o $(LDFLAGS)

.PHONY: clean
clean:
	rm -f *.o upload/test_light 
