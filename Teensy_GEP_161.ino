/***************************************************************************
    Teensy Guitar Effects Processor (GEP)

    This version only works with Teensy 3.5 or 3.6
    Build settings: CPU '120 MHz', USB Type 'Serial',  Optimize 'Fast',

    Version 1.6.1  08 Jan2020

    Arduino version last used: 1.8.10
    Teensyduino version last used: 1.48

    Based on Teensy Digital Guitar Amplifier / Effects Processor
    by Brian Miller Circuit Cellar July 2017

    Version 1.3 updated original project to current versions of
    the audio library, Arduino, and Teensyduino

    Version 1.4 switched to the 'freeverb' reverb engine, which may have
    exceeded Teensy 3.2 memory capability, so switched tro teensy 3.5/3.6

    Version 1.5 was a complete organization change, some gui code rewritten,
    added multi-delay effect and screen, converted config to struct. This
    is the beginning of a conversion to use classes for the effects to make
    it a bit easier to add new effects. Never Released.

    Version 1.6 - Current version: bug fixes and clean-up to version 1.5.
    Shortened name to Teensy_GEP.

    Repository: https://github.com/tbitson/Teensy-Guitar-Effects-Processor

 ********************************************************************************/


const String VERSION = "Version 1.6.1";


// uncomment debug print to enable helpful diagnostic print-outs
//#define DEBUG_PRINT

// audio patchpanel from audio tool
#include "patches.h"

// hardware libraries
#include <Encoder.h>
#include <Bounce.h>

#include <PCF8574.h>
#include <ILI9341_t3.h>
#include <XPT2046_Touchscreen.h>

// program includes
#include "config.h"     // default settings & eeprom storage
#include "hardware.h"   // hardware connections
#include "utils.h"      // tft routines and misc

#define UNITY_GAIN 1.0

// prototypes
long readParamEncoder();
long readValueEncoder();
void readMixPot();
void readWahWahPot();
bool checkCompressorSwitch();
uint8_t readButtons();
void toggleReverb();
void toggleFlanger();
void toggleTremolo();
void toggleWahWah();
bool updateCompressor();
void startGuitarTuner();
void playNote();
void handleSerialCommand(char);



// encoder & pot status values
long paramEncVal;
long lastParamEncVal;
long valEncVal;
long lastValEncVal;
uint16_t LastMixPotVal;
uint16_t LastWahWahPotVal;


// add all the effects
#include "Compressor.h"
#include "EQ.h"
#include "Reverb.h"
#include "Flanger.h"
#include "Tremolo.h"
#include "WahWah.h"
#include "InputLevel.h"
#include "Tuner.h"



// menu vars
#define NUM_MENUS 6

uint8_t menuIndex = cfg.lastMenu;
boolean initialScreenDrawn = false;
boolean msgFlag = false;

uint8_t tunerMode = 0;
uint32_t lastTime;




// *** initialize hardware ***

// create instance of TFT LCD Display
ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_MISO);

// create instance of touchscreen - non-interrupt config
XPT2046_Touchscreen ts(CS_PIN);

// create instances of rotory encoders
Encoder valueEncoder(ENCODER1_PIN1, ENCODER1_PIN2);
Encoder paramEncoder(ENCODER2_PIN1, ENCODER2_PIN2);

// create instances to switch debouncers
Bounce compressorSwitch  = Bounce(COMPRESSOR_SWITCH_PIN, DEBOUNCE_MS);
Bounce tunerSwitch = Bounce(TUNER_SWITCH_PIN, DEBOUNCE_MS);
Bounce saveSwitch  = Bounce(SAVE_SWITCH_PIN, DEBOUNCE_MS);

// create instance of I2C Expansion chip for LEDs & Switches
PCF8574 PCF(PCF8574_ADDRESS);



// *** start code ***


void setup()
{
  // wait for cpu to initialize
  delay(1000);

  Serial.begin(115200);
  AudioMemory(140);

  // configure hardware i/o pins
  pinMode(TUNER_SWITCH_PIN, INPUT_PULLUP);
  pinMode(SAVE_SWITCH_PIN, INPUT_PULLUP);
  pinMode(COMPRESSOR_SWITCH_PIN, INPUT_PULLUP);
  pinMode(TEST_LED, OUTPUT);

  delay(1000);

  Serial.println("Teensy GEP");
  Serial.println(VERSION);

  // show what teensy we're running
  //printCPUinfo();

  // check for tuner button down, if so, enter dev mode
  if (digitalRead(TUNER_SWITCH_PIN) == 0)
  {
      devMode = true;
      Serial.println("Dev Mode Enabled");
    }

  // load configuration
  loadConfig();
  printConfig();

  // fire up the lcd screen
  initLCD();

  // show the splash screen
  splashScreen();

  Serial.println("Configuring Audio...");

  // Enable the audio shield and set the output volumes
  Serial.println("Enabling audio module");
  audioShield.enable();                              // start audio board
  delay(100);

  audioShield.muteHeadphone();
  audioShield.muteLineout();
  audioShield.inputSelect(AUDIO_INPUT_LINEIN);       // use the line-in input
  audioShield.volume(HEADPHONE_OUTPUT_LEVEL);        // headphone output level
  audioShield.lineInLevel(cfg.inputValueAdj);            // input level
  audioShield.lineOutLevel(LINEOUT_AUDIO_LEVEL);     // 3.16 V p-p out

  audioShield.audioPostProcessorEnable();            // enable SGTL5000 effects processing
  //audioShield.adcHighPassFilterDisable();            // Turn off ADC HP filter (Forum claims this reduces audio noise)


  // init the effects
  Serial.println("Initialzing Effects");
  initCompressor();
  initEqualizer();
  initReverb();
  initTremolo();
  initFlanger();
  initWahWah();
  initInputLevel();


  // Mixer 3 controls dry, wet, and tone levels
  // test tone is on ch. 2 of this mixer
  mixer3.gain(DRY_CH,  1.0);  // dry
  mixer3.gain(WET_CH,  0.0);  // wet
  mixer3.gain(TONE_CH, 1.0);  // tone

  // configure a sine wave for the test tone
  sine2.frequency(1000);  // 1000 Hz
  sine2.amplitude(0.5);   // 50% amplitude


  // *** external hardware ***

  // initialize encoders
  paramEncoder.write(0);
  valueEncoder.write(0);

  // init leds & pushbutton interface IC
  PCF.write8(0xff);

  Serial.println("Enabling Touchscreen");
  ts.begin();

  // show audio memory usage for debugging
  //printAudioMemUsage();

  // flush serial port recieve buffer
  while (Serial.available() > 0)
  {
    char c = Serial.read();
    Serial.print(c);
  }

  // let everyting settle
  delay(1000);

  // ok, here we go...
  audioShield.unmuteHeadphone();
  audioShield.unmuteLineout();

  Serial.println("Setup complete");
}




// *** main loop ***

void loop()
{
  uint8_t switchPressed;
  String message;


  // debug - remove. loop timing
  //uint32_t start = micros();

  // blink test led to show we're alive
  if (millis() - lastTime > 300)
  {
    digitalWrite(TEST_LED, !digitalRead(TEST_LED));
    lastTime = millis();
  }

  // check compressor switch & update
  if (checkCompressorSwitch())
  {
    toggleCompressor();

    if (cfg.compEnabled)
      message = "Compressor ON";
    else
      message = "Compressor OFF";
    msgFlag = true;
  }


  // save switch pressed ?
  saveSwitch.update();   // SAVE switch
  if (saveSwitch.fallingEdge())
  {
    Serial.println("Save Button Pressed");
    saveConfig();
    message = "Config Saved";
    msgFlag = true;
  }


  // tuner switch pressed ?
  tunerSwitch.update();
  if (tunerSwitch.fallingEdge())
  {
    Serial.println("Tuner Button Presssed");

    // is tuner button hijacked for alternate use
    if (tunerMode == SHOW_CONFIG)
      printConfig();
    else if (tunerMode == PLAY_NOTE)
      playNote();
    else if (tunerMode == REBOOT_MODE)
      rebootTeensy();
    else
      startGuitarTuner();
  }

  // read front panel pots
  readMixPot();
  readWahWahPot();

  // Read the front panel buttons
  switchPressed = readButtons();

  if (switchPressed)
  {
    switch (switchPressed)
    {
      case 0x01:  // Reverb switch
        Serial.println("Toggle Reverb");
        toggleReverb();
        message = reverbActive ? "Reverb ON" : "Reverb OFF";
        msgFlag = true;
        break;

      case 0x02:  // Flanger Switch
        Serial.println("Toggle Flanger");
        toggleFlanger();
        message = flangerActive ? "Flanger ON" : "Flanger OFF";
        msgFlag = true;
        break;

      case 0x04:  // Tremolo switch
        Serial.println("Toggle Tremolo");
        toggleTremolo();
        message = tremoloActive ? "Tremolo ON" : "Tremolo OFF";
        msgFlag = true;
        break;

      case 0x08:  // Wah-Wah switch
        Serial.println("Toggle Wah-Wah");
        toggleWahWah();
        message = wahWahActive ? "Wah-Wah ON" : "Wah-Wah OFF";
        msgFlag = true;
        break;
    }
  }


  // screen update loop  - continously call the current screen
  switch (menuIndex)
  {
    case 0:
      doEqScreen();
      break;

    case 1:
      if (!initialScreenDrawn)
      {
        drawInitialCompressorScreen();
        initialScreenDrawn = true;
      }
      else
        updateCompressorScreen();
      break;

    case 2:
      if (!initialScreenDrawn)
        drawInitialTremoloScreen();
      else
        updateTremoloScreen();
      break;

    case 3:
      if (!initialScreenDrawn)
        drawInitialFlangerScreen();
      else
        updateFlangerScreen();
      break;

    case 4:
      doReverbScreen();
      break;

    case 5:
      if (!initialScreenDrawn)
        drawInputLevelScreen();
      else
        updateInputLevelScreen();
      break;
  }
  cfg.lastMenu = menuIndex;

  // check touchscreen
  if (ts.touched())
  {
    TS_Point p = ts.getPoint();
    //Serial.print("Pressure = "); Serial.println(p.z);
    //Serial.print("Screen X = "); Serial.println(p.x);
    //Serial.print("Screen Y = "); Serial.println(p.y);

    // touched left half - increment menu
    if (p.x > 1500)
    {
      menuIndex++;
      if (menuIndex > NUM_MENUS - 1)
        menuIndex = 0;
    }

    // touched right half - decrement menu
    if (p.x < 1501)
    {
      menuIndex--;
      if (menuIndex < 0)
        menuIndex = NUM_MENUS - 1;
    }

    // flag to draw the initial screen
    initialScreenDrawn = false;
  }

  // is there a message to display?
  if (msgFlag)
  {
    tftMessage(message);
    msgFlag = false;
    // redraw last screen
    initialScreenDrawn = false;
  }

  // check for incoming data on serial port
  if (Serial.available())
  {
    // read 1st character, preserving the rest
    char c = Serial.read();

    // filter out non-printable characters
    if (c > 32 && c < 127)
    {
      Serial.print("Serial Cmd ->"); Serial.println(c);
      handleSerialCommand(c);
    }
  }

  //uint32_t end = micros();
  //Serial.println(end - start);

  delay(50);
}



// *** functions ***


long readParamEncoder()
{
  // read encoder & divide results by 4 to slow down
  paramEncVal = paramEncoder.read() >> 2;

  //  if (abs(lastParamEncVal - paramEncVal) > 2)
  //  {
  //    Serial.print("Param Encoder = ");
  //    Serial.println(paramEncVal);
  //  }

  return paramEncVal;
}



long readValueEncoder()
{
  // read encoder & divide results by 4 to slow down
  valEncVal = valueEncoder.read() >> 2;

  //  if (abs(lastValEncVal - valEncVal) > 2)
  //  {
  //    Serial.print("Value Encoder = ");
  //    Serial.println(valEncVal);
  //  }

  return valEncVal;
}





void readMixPot()
{
  // read wet/dry mix pot and set mixer accordingly
  int pot = 0;
  float wet, dry;
  int delta;

  // 8 averages
  for (uint8_t i = 0; i < 8; i++)
    pot += analogRead(BALANCE_POT);

  pot /= 8;

  // ignore changes of less than 2 counts
  delta = pot - LastMixPotVal;
  if (abs(delta) > 2)
  {
    // convert 0 to 1023 to 0 to 1.0
    wet = (float)pot / 1023.0;

    // wet drops from 100% to 50% as effect goes up
    // based on subjective listening
    dry = 1.0 - (wet / 2.0);
#ifdef DEBUG_MODE
    dry = 1.0 - wet;
#endif

    // Mixer 3 changes wet level based on mix pot
    mixer3.gain(DRY_CH, dry);
    mixer3.gain(WET_CH, wet);

    printValue("dry", dry);
    printValue("wet", wet);

    LastMixPotVal = pot;
  }
}



void readWahWahPot()
{
  int pot = 0;
  int delta;

  // average 8 readings
  for (uint8_t i = 0; i < 8; i++)
    pot += analogRead(WAH_WAH_POT);

  pot /= 8;

  // ignore changes of less than 2 counts
  delta = pot - LastWahWahPotVal;
  if (abs(delta) > 2)
  {
    // scale to +/- 1.0 and set dc source
    float val = -1.0 + ((float)pot / 512.0);
    dc2.amplitude(val);
    LastWahWahPotVal = pot;

    printValue("Wah-Wah Pot", pot);
  }
}



bool checkCompressorSwitch()
{
  // update switch debounce info
  compressorSwitch.update();

  // switch pressed?
  if (compressorSwitch.fallingEdge())
  {
    Serial.println("Compressor Switch Pressed");
    return true;
  }

  return false;
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

  // check how long button is down
  start = millis();
  temp = button;
  // loop until button is released
  while (temp != 0)
  {
    temp = ~PCF.read8() & 0x0F;
    now = millis();
    //Serial.print("Button = 0x"); Serial.println(temp, HEX);
    //Serial.print("delta t = "); Serial.println(now - start);
    delay(20);
  }

  // if button down > 1.5 secs, its a 'long' press
  if (now - start > 1500)
  {
    //Serial.println("long press");
    //set upper bit as a flag
    button += 0x80;
  }

  Serial.print("Button = 0x"); Serial.println(button, HEX);
  return button;
}




void startGuitarTuner()
{
  // turn off effects mix
  mixer3.gain(DRY_CH, 1.0);
  mixer3.gain(WET_CH, 0.0);
  mixer3.gain(TONE_CH, 0.0);

  // launch the guitar tuner
  guitarTuner();

  // done with tuner, reset stuff

  // reset mix
  readMixPot();

  // go to menu screen
  initialScreenDrawn = false;
  menuIndex = 0;
}



// *** Effects Control ***

void togglecomp()
{
  if (cfg.compEnabled)
    disableCompressor();
  else
    enableCompressor();
}



void toggleReverb()
{
  if (reverbActive)
  {
    disableReverb();
    PCF.write(REVERB_LED, LED_OFF);
  }
  else
  {
    enableReverb();
    PCF.write(REVERB_LED, LED_ON);
  }
}



void toggleFlanger()
{
  if (flangerActive)
  {
    disableFlanger();
    PCF.write(FLANGER_LED, LED_OFF);
  }
  else
  {
    enableFlanger();
    PCF.write(FLANGER_LED, LED_ON);
  }
}



void toggleTremolo()
{
  if (tremoloActive)
  {
    disableTremolo();
    PCF.write(TREMOLO_LED, LED_OFF);
  }
  else
  {
    enableTremolo();
    PCF.write(TREMOLO_LED, LED_ON);
  }
}



void toggleWahWah()
{
  if (wahWahActive)
  {
    // turn off
    disableWahWah();
    PCF.write(WAH_WAH_LED, LED_OFF);
  }
  else
  {
    // turn on
    enableWahWah();
    PCF.write(WAH_WAH_LED, LED_ON);
  }
}




void playNote()
{
  envelope1.noteOn();
  delay(36);
  envelope1.noteOff();
}


void handleSerialCommand(char c)
{
  switch (c)
  {
    case 'p':
      printConfig();
      break;

    case 'm':
      printAudioMemUsage();
      break;

    case 'n':
      playNote();
      break;

    case 'c':
      toggleCompressor();
      break;

    default:
      Serial.println("I'm sorry Dave, I didn't get that");
  }
}
