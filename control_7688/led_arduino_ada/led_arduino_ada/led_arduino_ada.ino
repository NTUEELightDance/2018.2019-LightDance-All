#include <Adafruit_NeoPixel.h>
#include<SPI.h>

#define NUM_WS 5

int NUM_LEDS[] = {88,96,60,36,36};
int LED_PIN[]  = {5, 6, 7, 8, 9};

volatile boolean received;
volatile byte Slavereceived,Slavesend;

//Adafruit_NeoPixel *ws[NUM_WS];

Adafruit_NeoPixel strip0 = Adafruit_NeoPixel(NUM_LEDS[0], LED_PIN[0], NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(NUM_LEDS[1], LED_PIN[1], NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(NUM_LEDS[2], LED_PIN[2], NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip3 = Adafruit_NeoPixel(NUM_LEDS[3], LED_PIN[3], NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip4 = Adafruit_NeoPixel(NUM_LEDS[4], LED_PIN[4], NEO_GRB + NEO_KHZ800);

int led_counter = 0;
int rgb_counter = 0;
int rgb[3];

int wsid = -1;
bool check_wsid = false;

void setup() {
//  ws[0] = &strip0;
//  ws[1] = &strip1;
//  ws[2] = &strip2;
//  ws[3] = &strip3;
//  ws[4] = &strip4;
//
//  for (int i = 0;i < NUM_WS;i++){
//    ws[i]->setBrightness(50);
//    ws[i]->begin();
//  }
  strip0.begin();
  strip1.begin();
  strip2.begin();
  strip3.begin();
  strip4.begin();
   
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
//      Serial.println(wsid);
      switch(wsid){
        case 0:
          strip0.setPixelColor( led_counter, strip0.Color(rgb[0],rgb[1],rgb[2]) );
          Serial.println("0");
          break;
        case 1:
          strip1.setPixelColor( led_counter, strip1.Color(rgb[0],rgb[1],rgb[2]) );
          Serial.println("1");
          break;
        case 2:
          strip2.setPixelColor( led_counter, strip2.Color(rgb[0],rgb[1],rgb[2]) );
          Serial.println("2");
          break;
        case 3:
          strip3.setPixelColor( led_counter, strip3.Color(rgb[0],rgb[1],rgb[2]) );
          Serial.println("3");
          break;
        case 4:
          strip4.setPixelColor( led_counter, strip4.Color(rgb[0],rgb[1],rgb[2]) );
          Serial.println("4");
          break;
      }
//      ws[wsid]->setPixelColor( led_counter, ws[wsid]->Color(rgb[0],rgb[1],rgb[2]) );
      
      led_counter++;
      if(led_counter >= NUM_LEDS[wsid]){
        led_counter = 0;
        if(wsid == NUM_WS-1){
//          for (int i = 0;i < NUM_WS; i++){
//            ws[i]->show();
//          }

          strip0.show();
          strip1.show();
          strip2.show();
          strip3.show();
          strip4.show();
        }
      }
    }
    received = false;
  }
}
