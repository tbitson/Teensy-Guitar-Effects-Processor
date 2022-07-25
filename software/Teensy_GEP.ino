/***************************************************************************
    Teensy Guitar Effects Processor (GEP)

    This version only works with Teensy 3.5 or 3.6
    Build settings: CPU '120 MHz', USB Type 'Serial',  Optimize 'Fast woth LTO',

    Version 1.8.5  09Sept2020

    Repository: https://github.com/tbitson/Teensy-Guitar-Effects-Processor

    Arduino version used: 1.8.12
    Teensyduino version used: 1.48

    Based on Teensy Digital Guitar Amplifier / Effects Processor
    by Brian Miller Circuit Cellar July 2017

    Version 1.3 updated original project to current versions of
    the audio library, Arduino, and Teensyduino

    Version 1.4 switched to the 'freeverb' reverb engine, which
    exceeded Teensy 3.2 memory capability, so switched to teensy 3.5/3.6

    Version 1.5 was a complete organization change, some gui code rewritten,
    added multi-delay effect and screen, converted config to struct. This
    is the beginning of a conversion to use classes for the effects to make
    it a bit easier to add new effects. Never Released.

    Version 1.6 - bug fixes and clean-up to version 1.5.
    Shortened name to Teensy_GEP. Issue with TFT LCD and Audio Sheild sharing
    SPI bus. Temp workaround is to add delays at startup. Added test tone
    at the input to check effect without an audio source. Required an additional
    mixer at input. Holding down tuner button sets a dev mode that allows changes to
    behavior without affecting normal operation. Use to enable test tone using wahwah
    button. Enabled headphones output in addion to line out, need provision
    to adjust level. Added line out level indicator in input screen. Added 4 channel
    delay and screen. Added detection of a long press on effects buttons to allow
    toggling of 8 effects, no led indicators though. Selecting an effect also
    switches screens to the effect chosen. Reverb is now downstream of the other
    effects, which required a new mixer and logic changes. Guitar tuner changes seems
    to be working pretty good when compared to a commercial product. Test led is now
    toggled in the main loop, indicating we're alive. Maybe repurpose power led?
    Added serial interface. Enter '?' in terminal to see menu. Allows control via
    USB and monitoring mem usage, effects status, etc. Very helpful when creating
    new effects; no gui needed. Only a few commands so far.

    Version 1.7 - completed conversion of effects to classes. Ain't as clean as
    I hoped, still some room to optimize. Some major re-arrangement of code. Moved
    hardware related functions from main to hardware.h. Created radio button group
    in guiItems.h to handle the buttons in Compressor.h. Test tone is now injected
    at the beginning of the audio chain, not just in the dry path. Added status
    screen to provide a quick visual of effects status. Disabled audio interrupts
    at startup to minimize the 'clunk' when initializing the audio board. Delay
    changed to 2 channels, but added a feedback or 'reciculate' options with
    a slider. Added a status screen to show which effects are enabled; touch the
    center of the LCD. Expanded the serial command to allow enabling or disabling
    of all effects, added screem selection, etc.

    Version  1.8 - Added an 8-channel mixer to the GUI tools and Teensy Audio Lib.
    Converted code to use the 8-channel mixer which eliminated 2 4-channel mixers,
    thereby simplifing things a little. Added a lowpass "cab filter" to output to
    mellow the tone. Added output wet/dry mixer. Patched AVC code in audio lib.


    known bugs & todo:
    1) Hang on boot: seems to be a conflict between LCD and audio board serial flash that
    causes an intermittant hang on boot. Startup sequence rearranged, seems better but not fixed.
    2) Compressor - bug in audio lib, see pull #210, manually applied
    3) High freq noise under some condtions - seems to be coupled through USB
    todo: decide if current values are kept in cfg.xxx or in the class (probably class)
    i.e. the config code is now somewhat dated.
    todo: test compressor now that bug in lib is patched.
    todo: re-do the guiItems now that they mostly work
    todo: add indicator when bypass switch is in bypass
    todo: consider ditching the mix pot; since each effect has it's own volume
    control (usally a mixer channel), its redundant. Set each effect on its own.
    todo: move slider code from each effect into guiItems.h
    todo: refactor guiItems.h to move lcd code into a new file 



 ********************************************************************************/


const String VERSION = "Vers. 1.8.5";


// set to enable helpful diagnostic print-outs
bool debugPrint = true;

// allow serial port commands
#define USE_SERIAL_CMDS

// go to status screen after 30 seconds
//#define STATUS_SCREEN_ON_TIMEOUT

// jump to setting when effects selected
//#define SHOW_EFFECTS_SETTINGS


// audio patchpanel from audio tool
#include "patches.h"


// program includes
#include "utils.h"      // print routines, misc stuff
#include "config.h"     // default settings & eeprom storage
#include "hardware.h"   // hardware connections


// prototypes
void adjustMixValue();
void adjustWahWahValue();
void startGuitarTuner();
void playTone();
void printConfig();
void printStatus();
void doSerialCommands();



// add all the effects
#include "Compressor.h"
#include "EQ.h"
#include "Reverb.h"
#include "Delayer.h"
#include "Flanger.h"
#include "Tremolo.h"
#include "WahWah.h"
#include "Chorus.h"
#include "Levels.h"


// create instances of effects
Compressor compressor;
EQ  eq;
Delayer delayer;
Reverb reverb;
Tremolo tremolo;
Flanger flanger;
WahWah wahwah;
Chorus chorus;
Levels levels;

// include after declaring classes
#include "status.h"     // status screen
#include "tuner.h"
#include "update.h"



// number of menus
#define NUM_MENUS       8

// order of effect screens
#define EQ_SCREEN          0
#define COMPRESSOR_SCREEN  1
#define TREMOLO_SCREEN     2
#define REVERB_SCREEN      3
#define FLANGER_SCREEN     4
#define DELAY_SCREEN       5
#define CHORUS_SCREEN      6
#define INPUT_SCREEN       7
#define STATUS_SCREEN      8



// menu vars
uint8_t menuIndex = 0;
uint8_t lastMenuIndex = 0;
uint32_t menuChangedTime = 0;
boolean initScreen = true;
boolean msgFlag = false;
uint8_t tunerMode = 0;

// button timer
uint32_t lastTime;

// pot values
int mixPot;
int wahWahPot;





// *** start processing ***


void setup()
{
  // start serial port
  Serial.begin(57600);

  // configure hardware i/o pins
  pinMode(TUNER_SWITCH_PIN, INPUT_PULLUP);
  pinMode(SAVE_SWITCH_PIN, INPUT_PULLUP);
  pinMode(TEST_LED, OUTPUT);

  // turn on led to show we're awake
  digitalWrite(TEST_LED, ON);
  
  // wait for cpu & lcd to initialize
  delay(1000);

  // what teensy are we using?
  checkTeensyType();

  Serial.print("Teensy GEP "); 
  Serial.println(VERSION);
  Serial.println(procType);

  // start eeprom library
  EEPROM.begin();

  // check for tuner button down, if so, we'll reset to default config
  if (digitalRead(TUNER_SWITCH_PIN) == 0)
  {
    Serial.println("resetting to defaults");
    // bypass loading eeprom config
    loadDefaults();

    // blink led to show reset
    blinkLED(3);
  }
  else
  {
    // load configuration
    loadConfig();
  }


  printValue("Configuring Audio...");

  // allocate memory for audio (mostly for delays)
  AudioMemory(256);

  // Enable the audio shield and set the output volumes
  printValue("Enabling audio module");
  audioShield.enable();                              // start audio board
  delay(100);

  audioShield.muteHeadphone();
  audioShield.muteLineout();
  audioShield.inputSelect(AUDIO_INPUT_LINEIN);    // use the line-in input
  audioShield.volume(HEADPHONE_OUTPUT_LEVEL);     // headphone output level
  audioShield.lineOutLevel(LINEOUT_AUDIO_LEVEL);  // 3.16 V p-p out
  audioShield.audioPostProcessorEnable();         // enable SGTL5000 effects processing
  //audioShield.adcHighPassFilterDisable();       // Turn off ADC HP filter (Forum claims this reduces audio noise)

  // set up cab filter 2-stages of LPF, cutoff 4500 Hz, Q-factor 0.7071
  biquad1.setLowpass(0, 4500, .7071);
  biquad1.setLowpass(1, 4500, .7071);


  printValue("Initialzing Effects");
  compressor.init();
  eq.init();
  reverb.init();
  delayer.init();
  tremolo.init();
  flanger.init();
  wahwah.init();
  chorus.init();
  levels.init();

  // configure a sine wave for the test tone and disable
  sine2.frequency(500);   // 500 Hz
  sine2.amplitude(0.5);   // 50% amplitude

  // initialize encoders
  paramEncoder.write(0);
  valueEncoder.write(0);

  // init leds & pushbutton interface IC
  PCF.write8(0xff);

  // flush serial port recieve buffer
  while (Serial.available() > 0)
  {
    char c = Serial.read();
    Serial.print(c);
  }


  // update all global audio settings
  updateMix(readMixPot());
  updateWahWah(readWahWahPot());
  updateAudio();

  // fire up the lcd screen
  initLCD();
  delay(1000);

  // show the splash screen
  splashScreen();
  delay(4000);

  // recall last menu used
  menuIndex = cfg.lastMenu;

  // let everyting settle
  delay(500);

  printValue("Enabling Touchscreen");
  ts.begin();
  
  // ok, here we go...
  delay(500);
  audioShield.unmuteHeadphone();
  audioShield.unmuteLineout();

  Serial.println("Setup complete");
}




// *** main loop ***

void loop()
{
  uint8_t switchPressed;
  String message;

  // slow loop down just a bit
  delay(50);

  // blink test led to show we're alive
  if (millis() - lastTime > 1000)
  {
    digitalWrite(TEST_LED, !digitalRead(TEST_LED));
    lastTime = millis();
  }

  // save switch pressed ?
  saveSwitch.update();
  if (saveSwitch.fallingEdge())
  {
    printValue("Save Button Pressed");
    saveConfig();
    message = "Config Saved";
    msgFlag = true;
  }

  // tuner switch pressed ?
  tunerSwitch.update();
  if (tunerSwitch.fallingEdge())
  {
    if (debugPrint) Serial.println("Tuner Button Presssed");
    startGuitarTuner();
  }


  // read front panel pots
  mixPot = readMixPot();
  if (abs(mixPot - lastMixPot) > 3)
    updateMix(mixPot);


  // read wah-wah pot
  wahWahPot = readWahWahPot();
  if (abs(wahWahPot - lastWahWahPot) > 3)
    updateWahWah(wahWahPot);


  // Read the front panel buttons
  switchPressed = readButtons();
  if (switchPressed)
  {
    switch (switchPressed)
    {
      case 0x01:  // Reverb switch
        printValue("Toggle Reverb");
        reverb.toggle();
        message = reverb.enabled ? "Reverb ON" : "Reverb OFF";
        msgFlag = true;
        menuIndex = REVERB_SCREEN;
        break;

      case 0x02:  // Flanger Switch
        printValue("Toggle Flanger");
        flanger.toggle();
        message = flanger.enabled ? "Flanger ON" : "Flanger OFF";
        msgFlag = true;
        menuIndex = FLANGER_SCREEN;
        break;

      case 0x04:  // Tremolo switch
        printValue("Toggle Tremolo");
        tremolo.toggle();
        message = tremolo.enabled ? "Tremolo ON" : "Tremolo OFF";
        msgFlag = true;
        menuIndex = TREMOLO_SCREEN;
        break;

      case 0x08:  // Wah-Wah switch
        printValue("Toggle Wah-Wah");
        wahwah.toggle();
        message = wahwah.enabled ? "Wah-Wah ON" : "Wah-Wah OFF";
        msgFlag = true;
        break;

      case 0x81:
        delayer.toggle();
        message = delayer.enabled ? "Delay ON" : "Delay OFF";
        msgFlag = true;
        menuIndex = DELAY_SCREEN;
        break;

      case 0x82:
        chorus.toggle();
        message = chorus.enabled ? "Chorus ON" : "Chorus OFF";
        msgFlag = true;
        menuIndex = CHORUS_SCREEN;
        break;

      case 0x84:
        eq.toggle();
        message = eq.enabled ? "EQ ON" : "EQ OFF";
        msgFlag = true;
        menuIndex = EQ_SCREEN;
        break;

      case 0x88:
        compressor.toggle();
        message = compressor.enabled ? "Compressor ON" : "Compressor OFF";
        msgFlag = true;
        menuIndex = COMPRESSOR_SCREEN;
        break;

      default:
        message = "Hmm.... Inavlid Button!!";
        msgFlag = true;
        break;
    }
    updateLEDs();
  }


  // screen update loop  - continously call the current screen
  if (menuIndex != lastMenuIndex)
  {
    printValue("menuIndex", menuIndex);
    initScreen = true;
    resetEncoders();
    menuChangedTime = millis();
    lastMenuIndex = menuIndex;
  }

  switch (menuIndex)
  {
    case EQ_SCREEN:
      eq.process(initScreen);
      break;

    case COMPRESSOR_SCREEN:
      compressor.process(initScreen);
      break;

    case TREMOLO_SCREEN:
      tremolo.process(initScreen);
      break;

    case FLANGER_SCREEN:
      flanger.process(initScreen);
      break;

    case REVERB_SCREEN:
      reverb.process(initScreen);
      break;

    case INPUT_SCREEN:
      levels.process(initScreen);
      break;

    case DELAY_SCREEN:
      delayer.process(initScreen);
      break;

    case CHORUS_SCREEN:
      chorus.process(initScreen);
      break;

    case STATUS_SCREEN:
      statusScreen(initScreen);
      break;

    default:
      printValue("Error: Invalid Screen Selection", menuIndex);
  }

  initScreen = false;


  // check touchscreen
  if (ts.touched())
  {
    TS_Point p = ts.getPoint();
    printPoint(p);

    // touched left half - previous menu
    if (p.x < 1200)
    {
      if (menuIndex > 0)
        menuIndex--;
      else
        menuIndex = NUM_MENUS - 1;
    }

    // touched right half - next menu
    else if (p.x > 2800)
    {
      menuIndex++;
      if (menuIndex > NUM_MENUS - 1)
        menuIndex = 0;
    }

    // touched center - go to status screen
    else
      menuIndex = STATUS_SCREEN;

    cfg.lastMenu = menuIndex;

    // add a delay after touch detected
    delay(100);
  }


  // is there a message to display?
  if (msgFlag)
  {
    tftMessage(message);
    msgFlag = false;
    // redraw last screen
    initScreen = true;
  }


  // is there a serial command waiting?
  doSerialCommands();


#ifdef STATUS_SCREEN_ON_TIMEOUT
  // time to go back to status screen?
  if (menuIndex != STATUS_SCREEN)
  {
    if (millis() - menuChangedTime > 60000)
    {
      menuIndex = STATUS_SCREEN;
      menuChangedTime = millis();
    }
  }
#endif
}



void updateLEDs()
{
  setLED(REVERB_LED,  reverb.getStatus());
  setLED(FLANGER_LED, flanger.getStatus());
  setLED(TREMOLO_LED, tremolo.getStatus());
  setLED(WAH_WAH_LED, wahwah.getStatus());
}



void printPoint(TS_Point p)
{
  Serial.print("Pressure = "); Serial.println(p.z);
  Serial.print("Screen X = "); Serial.println(p.x);
  Serial.print("Screen Y = "); Serial.println(p.y);
  Serial.println();
}




void startGuitarTuner()
{
  // Disable all effects to minimize memory and cpu usage
  compressor.disable();
  flange1.voices(0, 0, 0);

  // enable inputs, not tone or delay feedback
  mixer1.gain(LEFT_IN, 1.0);
  mixer1.gain(TEST_TONE, 0);

  // turn off effects and enable dry
  mixer8_1.gain(REVERB_IN, 0);
  mixer8_1.gain(CHORUS_IN, 0);
  mixer8_1.gain(WAH_WAH_IN, 0);
  mixer8_1.gain(FLANGER_IN, 0);
  mixer8_1.gain(TREMOLO_IN, 0);
  mixer8_1.gain(DELAY_IN, 0);


  // launch the guitar tuner
  guitarTuner();

  // restart flanger
  flanger.init();

  // reset compressor
  if (cfg.compEnabled)
    compressor.enable();

  // read current pot settings
  mixPot = readMixPot();
  wahWahPot = readWahWahPot();

  // set audio controls back to current values
  updateMix(mixPot);
  updateWahWah(wahWahPot);
  updateAudio();

  // go to menu screen
  initScreen = true;
  menuIndex = cfg.lastMenu;
}



void playTone()
{
  // enable mixer channel & leave on for now
  printValue("Playing Tone");
  mixer1.gain(TEST_TONE, 1.0);
  delay(120);
  mixer1.gain(TEST_TONE, 0);
  printValue("done");
}



void playLongTone()
{
  // enable mixer channel & leave on for now
  printValue("Playing Tone");

  mixer1.gain(TEST_TONE, 1.0);

  // read pots and buttons while playing tone
  for (int j = 0; j < 50; j++)
  {
    loop();
    delay(100);
  }
  mixer1.gain(TEST_TONE, 0);
  printValue("done");
}




void doSerialCommands()
{
  // check for incoming data on serial port
  if (Serial.available())
  {
    // read 1st character, preserving the rest
    char c = Serial.read();

    // filter out non-printable characters
    if (c > 32 && c < 127)
    {
      Serial.print("Serial Cmd ->"); Serial.println(c);

      switch (c)
      {
        case 'p':
          printConfig();
          break;

        case 'm':
          printAudioMemUsage();
          break;

        case 's':
          printStatus();
          break;

        case 'M':
          menuIndex++;
          if (menuIndex > NUM_MENUS - 1 )
            menuIndex = 0;
          break;

        case 't':
          playTone();
          break;

        case 'l':
          playLongTone();
          break;

        case 'R':
          reverb.toggle();
          updateLEDs();
          break;

        case 'T':
          tremolo.toggle();
          updateLEDs();
          break;

        case 'F':
          flanger.toggle();
          updateLEDs();
          break;

        case 'W':
          wahwah.toggle();
          updateLEDs();
          break;

        case 'C':
          compressor.toggle();
          break;

        case 'E':
          eq.toggle();
          break;

        case 'D':
          delayer.toggle();
          break;

        case 'c':
          chorus.toggle();
          break;

        case 'd':
          // toggle delay recirculate
          if (delayer.getRecirculate() > 0)
            delayer.setRecirculate(0);
          else
            delayer.setRecirculate(0.4);
          delayer.process(true);
          break;

        case '!':
          reverb.disable();
          flanger.disable();
          tremolo.disable();
          wahwah.disable();
          delayer.disable();
          compressor.disable();
          chorus.disable();
          eq.disable();
          break;

        case '$':
          clearEEPROM();
          break;

        case '?':
          Serial.println(F("p: Print config"));
          Serial.println(F("m: print Memory usage"));
          Serial.println(F("s: print effects Status"));
          Serial.println(F("t: play test Tone"));
          Serial.println(F("l: play long test Tone"));
          Serial.println(F("M: Move to next screen"));

          Serial.println(F("C: toggle Compressor"));
          Serial.println(F("E: toggle Equalizer"));
          Serial.println(F("R: toggle Reverb"));
          Serial.println(F("F: toggle Flanger"));
          Serial.println(F("T: toggle Tremolo"));
          Serial.println(F("W: toggle WahWah"));
          Serial.println(F("D: toggle Delayer"));
          Serial.println(F("c: toggle Chorus"));

          Serial.println(F("d: toggle Delay Recirculate"));

          Serial.println(F("!: Reset All Effects"));
          Serial.println(F("$: Clear EEPROM"));

          Serial.println(F("?: print help"));
          Serial.println();
          break;

        default:
          Serial.println(F("I'm sorry Dave, I afraid I can't do that"));
      }
    }
  }
}




void printConfig()
{
  Serial.println();
  Serial.println(F("Current Config:"));
  Serial.print(F("Version = ")); Serial.println(cfg.vers);

  compressor.printConfig();
  eq.printConfig();
  reverb.printConfig();
  tremolo.printConfig();
  delayer.printConfig();
  flanger.printConfig();
  levels.printConfig();
  chorus.printConfig();
  wahwah.printConfig();

  Serial.print(F("Last Menu  = ")); Serial.println(cfg.lastMenu);
  Serial.println();
  delay(200);
}



void printStatus()
{
  Serial.print(F("Compressor enabled = ")); Serial.println(compressor.getStatus());
  Serial.print(F("EQ enabled         = ")); Serial.println(eq.getStatus());
  Serial.print(F("Reverb enabled     = ")); Serial.println(reverb.getStatus());
  Serial.print(F("Flanger enabled    = ")); Serial.println(flanger.getStatus());
  Serial.print(F("Delay enabled      = ")); Serial.println(delayer.getStatus());
  Serial.print(F("Tremolo enabled    = ")); Serial.println(tremolo.getStatus());
  Serial.print(F("Chorus enabled     = ")); Serial.println(chorus.getStatus());
  Serial.print(F("Wah-Wah enabled    = ")); Serial.println(wahwah.getStatus());
}
