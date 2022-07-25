/*******************************************************************************
   hardware.h -  Hardware definitions and functions

   Version 1.3   Jan 2020

   Pin definitions and initialization routines for:
        TFT LCD Display
        Rotory Encoders
        Analog Pots
        on-board switches (save & tuner)
        external switch (TBD)
        Test LED
        PCF8574 i2c bus expander controlling
           4 indicator leds
           4 pushbutton switches



   reference: pins used by audio shield
   audio data:     9, 11, 13, 22, 23
   audio control: 18, 19          (I2C)
   volume pot:    15              (A1)
   sd card:        7, 10, 12, 14  (SPI)
   ext sram:       6,  7, 12, 14  (SPI)

   SPI used for external delay. With current design,
   pins 7, 12, and 14 are shared with TFT LCD, which
   seems to cause problems getting the LCD started

    Pin  Signal
     6  CS
     7  MOSI
    12  MISO
    14  SCK

*******************************************************************************/



#ifndef HARDWARE_H
#define HARDWARE_H




// hardware libraries
#include <Encoder.h>
#include <Bounce.h>
#include <PCF8574.h>
#include <ILI9341_t3.h>
#include <XPT2046_Touchscreen.h>

#include "guiItems.h"


// prototypes
void checkTeensyType();
void resetEncoders();
long readParamEncoder();
long readValueEncoder();
int readMixPot();
int readWahWahPot();
uint8_t readButtons();
void setLED(uint8_t led, bool state);
void blinkLed(uint8_t blinks);


// global vars for encoder & pot status values
long paramEncVal;
long lastParamEncVal;
long valEncVal;
long lastValEncVal;
String procType = "UNKNOWN";

// previous pot values
int lastMixPot = 0;
int lastWahWahPot = 0;

// uncomment to use the SD Card
//#define USE_SD_CARD


// simple defines
#define OFF 0
#define ON  1
#define UNITY_GAIN  1.0
#define GAIN_UNITY  1.0
#define GAIN_OFF    0



// TFT LCD SPI pins (255 = always on)
#define TFT_DC   20
#define TFT_CS   21
#define TFT_RST  255
#define TFT_MOSI 7
#define TFT_SCLK 14
#define TFT_MISO 12

// touchscreen chip select line
#define CS_PIN  8



// Use these with the Teensy 3.5/3.6 built-in SD card
#ifdef USE_SD_CARD
#ifdef TEENSY_35
#define SDCARD_CS_PIN    BUILTIN_SDCARD
#define SDCARD_MOSI_PIN  11
#define SDCARD_SCK_PIN   13
#endif
#ifdef TEENSY_36
#define SDCARD_CS_PIN    BUILTIN_SDCARD
#define SDCARD_MOSI_PIN  11
#define SDCARD_SCK_PIN   13
#endif
#ifdef TEENSY_32
// use the sd card on the Teensy Audio Shield
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14
#endif
#endif


// activity led pin, use additional pin on 3.5/3.6
#ifdef TEENSY_32
#define TEST_LED    3
#else
#define TEST_LED    24
#endif



// rotary encoder pins
#define ENCODER1_PIN1   1
#define ENCODER1_PIN2   2
#define ENCODER2_PIN1   4
#define ENCODER2_PIN2   5


// on-board potentiometers - could be pedal inputs
#define WAH_WAH_POT   A1
#define MIX_POT       A2

// on-board switches (pressing the encoder button)
#define TUNER_SWITCH_PIN        0
#define SAVE_SWITCH_PIN         17


// external switch - depricated
// #define EXTERNAL_SWITCH_PIN   10

// Switch Debounce Time
#define DEBOUNCE_MS    15

// PCF8574 Remote 8-Bit I/O Expander for I2C Bus
// Interface to the LED and Buttson
#define PCF8574_ADDRESS  0x20

// PCF8574 LEDs
// leds inverted
#define LED_ON          0
#define LED_OFF         1

// PCF8574 Switch defines
// buttons inverted
#define SWITCH_ON       0
#define SWITCH_OFF      1

// PCF8574 LED numbers
#define REVERB_LED      7
#define FLANGER_LED     6
#define TREMOLO_LED     5
#define WAH_WAH_LED     4

// PCF8574 button numbers
#define REVERB_SWITCH   0
#define FLANGER_SWITCH  1
#define TREMOLO_SWITCH  2
#define WAH_WAH_SWITCH  3

// future and in-work stuff
//#define BYPASS_LED    39

// cable notes
// BLK - GND
// BRN - BYPASS_SWITCH (active low)
// RED - A21
// ORG - RIGHT_CH_OUT
// YEL - RIGHT_CH_IN
// GRN - A22
// BLU - LEFT_CH_IN
// VIO - LEFT_CH_OUT



/*************************************************/


// *** create hardware instances ***

// create TFT LCD Display
ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_MISO);

// create instance of touchscreen - non-interrupt config
XPT2046_Touchscreen ts(CS_PIN);

// create instances of rotory encoders
Encoder valueEncoder(ENCODER1_PIN1, ENCODER1_PIN2);
Encoder paramEncoder(ENCODER2_PIN1, ENCODER2_PIN2);

// create instances to switch debouncers
Bounce tunerSwitch = Bounce(TUNER_SWITCH_PIN, DEBOUNCE_MS);
Bounce saveSwitch  = Bounce(SAVE_SWITCH_PIN, DEBOUNCE_MS);

// create instance of I2C Expansion chip for LEDs & Switches
PCF8574 PCF(PCF8574_ADDRESS);





void checkTeensyType()
{
  // which teensy are we using?
#ifdef __MK20DX256__
#define TEENSY_32
  Serial.println( "Teensy 3.2");
  procType = "Teensy 3.2";

#elif defined(__MK64FX512__)
#define TEENSY_35
  Serial.println( "Teensy 3.5");
  procType = "Teensy 3.5";

#elif defined(__MK66FX1M0__)
#define TEENSY_36
  Serial.println( "Teensy 3.6");
  procType = "Teensy 3.6";

#elif defined TEENSY_40
  Serial.println( "Teensy 4.0");
  procType = "Teensy 4.0";

#else
  Serial.println("unknown chip");
#warning "Unknown Teensy Chip"
  procType = "UNKNOWN";
#endif

}




void resetEncoders()
{
  paramEncoder.write(0);
  valueEncoder.write(0);
}



long readParamEncoder()
{
  // read encoder & divide results by 4 to slow down
  paramEncVal = paramEncoder.read() >> 2;
  return paramEncVal;
}



long readValueEncoder()
{
  // read encoder & divide results by 4 to slow down
  valEncVal = valueEncoder.read() >> 2;
  return valEncVal;
}



// returns 0 to 1023
int readMixPot()
{
  int pot = 0;

  // 8 averages
  for (uint8_t i = 0; i < 8; i++)
    pot += analogRead(MIX_POT);
  pot /= 8;

  return pot;
}



// returns 0 to 1023
int readWahWahPot()
{
  int pot = 0;

  // read pot & take 8 averages
  for (uint8_t i = 0; i < 8; i++)
    pot += analogRead(WAH_WAH_POT);
  pot /= 8;

  return pot;
}


uint8_t readButtons()
{
  uint32_t start = 0;
  uint32_t now = 0;
  uint8_t temp = 0;
  uint8_t button = 0;

  // buttons are inverted and on lower 4 bits
  button = ~PCF.read8() & 0x0F;

  if (button == 0)
    return 0;

  // turn off led
  digitalWrite(TEST_LED, OFF);

  // check how long button is down
  start = millis();
  temp = button;

  // loop until button is released
  while (temp != 0)
  {
    temp = ~PCF.read8() & 0x0F;
    now = millis();

    // turn on led when long press detected
    if (now - start > 1500)
      digitalWrite(TEST_LED, ON);

    delay(20);
  }

  // if button down > 1.5 secs, its a 'long' press
  if (now - start > 1500)
  {
    printValue("long press detected");
    button += 0x80;
  }

  printHexValue("Button", button);
  return button;
}



void setLED(uint8_t led, bool state)
{
  // led is inverted (led off = 1), so fix it here
  PCF.write(led, !state);
}



void blinkLED(uint8_t blinks)
{
  for (uint8_t i = 0; i < blinks; i++)
  {
    digitalWrite(TEST_LED, OFF);
    delay(100);
    digitalWrite(TEST_LED, ON);
    delay(100);
  }
}


#endif
