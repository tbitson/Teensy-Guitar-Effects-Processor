/*******************************************************************************
     Hardware definitions and connection definitions

     Version 1.3

   Pin definitions for
     TFT LCD Display
     2 Rotory Encoders
     2 Anlog Pots
     2 on-board switches (save & tuner)
     1 external switch (compressor)
     A PCF8574 i2c bus expander controlling
       4 indicator leds
       4 pushbutton switches
     1 Test LED


*******************************************************************************/


#ifndef HARDWARE_H
#define HARDWARE_H


// which teensy are we using?
//#define TEENSY_32
#define TEENSY_35
// #define TEENSY_36
// #define TEENSY_40 


// uncomment to use the SD Card
//#define USE_SD_CARD


// simple defines
#define OFF 0
#define ON  1
#define UNITY 1.0

// reference: pins used by audio shield
// audio data:     9, 11, 13, 22, 23   
// audio control: 18, 19          (I2C)
// volume pot:    15              (A1)
// sd card:        7, 10, 12, 14  (SPI)
// ext sram:       6,  7, 12, 14  (SPI)
//
// SPI used for external delay. With current design.
// pins 7, 12, and 14 are shared with TFT LCD, which
// ssems to cause problems getting the LCD started
//
// Pin  Signal
//   6  CS
//   7  MOSI
//  12  MISO
//  14  SCK


// TFT LCD SPI pins (255 = always on)
#define TFT_DC   20
#define TFT_CS   21
#define TFT_RST  255
#define TFT_MOSI 7
#define TFT_SCLK 14
#define TFT_MISO 12

// touchscreen chip select line
#define CS_PIN  8


#ifdef USE_SD_CARD

// Use these with the Teensy 3.5 & 3.6 built-in SD card
#ifdef TEENSY_35
#define SDCARD_CS_PIN    BUILTIN_SDCARD
#define SDCARD_MOSI_PIN  11
#define SDCARD_SCK_PIN   13
#else
// use the sd card on the Teensy Audio Shield
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14
#endif
#endif


// activity led pin, use additional pin on 3.5/3.6
#ifdef TEENSY_35
#define TEST_LED    24
#else
#define TEST_LED    3
#endif


// rotary encoder pins
#define ENCODER1_PIN1   1
#define ENCODER1_PIN2   2
#define ENCODER2_PIN1   4
#define ENCODER2_PIN2   5


// on-board potentiometers - could be pedal inputs
#define WAH_WAH_POT   A1
#define BALANCE_POT   A2

// on-board switches (pressing the encoder button)
#define TUNER_SWITCH_PIN        0
#define SAVE_SWITCH_PIN         17

// tuner switch has several optional modes
#define GUITAR_TUNER    0  // default
#define PLAY_NOTE       1
#define SHOW_CONFIG     2
#define REBOOT_MODE     3


// external switch
#define COMPRESSOR_SWITCH_PIN   10


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


#endif
