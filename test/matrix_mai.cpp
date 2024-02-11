/****************************************************************
 * Simple test of ht16k33 library turning on and off LEDs
 */

#include "ht16k33.h"
#define HT16K33_DSP_NOBLINK   B00000000 // Display setup - no blink
#define HT16K33_DSP_BLINK2HZ  B00000010 // Display setup - 2hz blink
#define HT16K33_DSP_BLINK1HZ  B00000100 // Display setup - 1hz blink
#define HT16K33_DSP_BLINK05HZ B00000110 // Display setup - 0.5hz blink

// Define the class
HT16K33 HT;

/****************************************************************/
void setup() {
  Serial.begin(115200);
  Serial.println(F("ht16k33 light test v0.01"));
  Serial.println();
  // initialize everything, 0x00 is the i2c address for the first one (0x70 is added in the class).
  HT.begin(0x00);
  HT.setBrightness(15);
  /*for (int led=0; led<128; led++) {
    HT.setLedNow(led);
    delay(10);
  } // for led
  */
  HT.displayOn();
  //HT.setBlinkRate(HT16K33_DSP_BLINK05HZ);
}

/****************************************************************/
void loop() {
 
  uint8_t led;

  int randNumber = random(10);
  //HT.setBrightness(randNumber+5);
  delay(100);

/*
  Serial.println(F("Turn on all LEDs"));
  // first light up all LEDs
  for (led=0; led<128; led++) {
    HT.setLedNow(led);
    delay(10);
  } // for led

  delay(5000);

  Serial.println(F("Clear all LEDs"));
  //Next clear them
  for (led=0; led<128; led++) {
    HT.clearLedNow(led);
    delay(10);
  } // for led
  delay(5000);

  //Now do one by one, slowly, and print out which one
  for (led=0; led<20; led++) {
    HT.setLedNow(led);
    Serial.print(F("Led no "));
    Serial.print(led,DEC);
    Serial.print(F(": On"));
    delay(2000);
    HT.clearLedNow(led);
    Serial.println(F(" Off"));
    if(led>2 && led < 10) led = 15;
  } // for led
  */
} // loop  