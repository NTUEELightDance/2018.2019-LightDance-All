#include "FastLED.h"
#include<SPI.h>

#define COLOR_ORDER GRB
#define LED_TYPE WS2812

#define NUM_WS 5

int NUM_LEDS[] = {88,48,60,36,36};
int LED_DT[]   = {5, 6, 7, 8, 9};

volatile boolean received;
volatile byte Slavereceived,Slavesend;

uint8_t max_bright = 64;
struct CRGB* ws[NUM_WS];
//struct CRGB leds[NUM_LEDS];
int led_counter = 0;
int rgb_counter = 0;
int rgb[3];

int wsid = -1;
bool check_wsid = false;

void setup() {
  for (int i=0;i<NUM_WS;i++){
    ws[i] = (struct CRGB*)malloc(sizeof(struct CRGB)*NUM_LEDS[i]);
  }
  LEDS.addLeds<LED_TYPE, 2, COLOR_ORDER>(ws[0], NUM_LEDS[0]);
  LEDS.addLeds<LED_TYPE, 3, COLOR_ORDER>(ws[1], NUM_LEDS[1]);
  LEDS.addLeds<LED_TYPE, 4, COLOR_ORDER>(ws[2], NUM_LEDS[2]);
  LEDS.addLeds<LED_TYPE, 5, COLOR_ORDER>(ws[3], NUM_LEDS[3]);
  LEDS.addLeds<LED_TYPE, 6, COLOR_ORDER>(ws[4], NUM_LEDS[4]);
  
//  LEDS.addLeds<LED_TYPE, LED_DT, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(max_bright);
//pinMode(MISO,OUTPUT);                   //Sets MISO as OUTPUT (Have to Send data to Master IN 
  SPCR |= _BV(SPE);                       //Turn on SPI in Slave Mode
  received = false;
  SPI.attachInterrupt();                  //Interuupt ON is set for SPI commnucation
//  pinMode(13,OUTPUT);
  Serial.begin(2000000);
}

ISR (SPI_STC_vect)                        //Inerrrput routine function 
{
  Slavereceived = SPDR;         // Value received from master if store in variable slavereceived
  received = true;                        //Sets received as True 
}

void loop() {
  if(received == true){
    if(Slavereceived == 63){ // start byte
      rgb_counter = 0;
      led_counter = 0;
      check_wsid = true;
    }
    else if(check_wsid){
      wsid = Slavereceived;
      check_wsid = false;
    }
    else if(rgb_counter<2){
      if(Slavereceived==0){
        rgb[rgb_counter] = 0;
      }
      else{
        rgb[rgb_counter] = (Slavereceived+1) << 2; 
      }
      rgb_counter++;
    }
    else{
      if(Slavereceived==0){
        rgb[rgb_counter] = 0;
      }
      else{
        rgb[rgb_counter] = (Slavereceived+1) << 2;
      }
      rgb_counter = 0;
      ws[wsid][led_counter].setRGB(rgb[0], rgb[1], rgb[2]);
      led_counter++;
      if(led_counter >= NUM_LEDS[wsid]){
        led_counter = 0;
        if(wsid == NUM_WS-1){
          FastLED.show();
        }
      }
    }
    received = false;
  }
}
