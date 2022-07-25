/***************************************************************************
    Teensy Guitar Effects Processor (GEP)

    A multi-effect DSP for guitar and other musical instuments
    that uses the Paul Stoffregen's excellent Teensy Audio Library (TAL).

    This version only works with Teensy 3.5 or 3.6
    Build settings: CPU '120 MHz', USB Type 'Serial',  Optimize 'Fast',

    Version 1.6.8  13Jan2020

    Repository: https://github.com/tbitson/Teensy-Guitar-Effects-Processor

    Arduino version last used: 1.8.10
    Teensyduino version last used: 1.48

    Based on Teensy Digital Guitar Amplifier / Effects Processor
    by Brian Miller Circuit Cellar July 2017

    Version 1.3 updated original project to current versions of
    the audio library, Arduino, and Teensyduino

    Version 1.4 switched to the 'freeverb' reverb engine, which may have
    exceeded Teensy 3.2 memory capability, so switched to teensy 3.5/3.6

    Version 1.5 was a complete organization change, some gui code rewritten,
    added multi-delay effect and screen, converted config to struct. This
    is the beginning of a conversion to use classes for the effects to make
    it a bit easier to add new effects. Never Released.

    Version 1.6 - Current version: bug fixes and clean-up to version 1.5.
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
    new effects; not all options implemented so far. Added a blink to the activity
    led to show when a long press is detected.

    Known Issues:
    Reverb Sliders intermittant
    High freq noise under some condtions. Clock or TFT LCD ?
    Conflict on SPI Bus (?) causes lcd to wig now sometimes
    LCD display goes blank or locks up sometimes. Could be same as SPI issue.
        requires power cycle to clear. One option is to use Teensy 3.5/3.6 SPI2 port,
        but would require routing wire around PC board or drilling (until respin)
    Test tone not working; something weird about mixer 5
    Remove debug code calc in readMixPot dry level
    Reverb output is dead. Check logic for bypass. Some weird TAL bug?



 ********************************************************************************/


const String VERSION = "Version 1.6.8";


// uncomment debug print to enable helpful diagnostic print-outs
#define DEBUG_PRINT

// allow serial port commands
#define USE_SERIAL_CMDS

// teensy bug/conflict requires to define here
#define GAIN_UNITY  1.0

// audio patchpanel from audio tool
//#include "patches.h"
#include "patches_tone.h"

// hardware libraries
#include <Encoder.h>
#include <Bounce.h>
#include <PCF8574.h>
#include <ILI9341_t3.h>
#include <XPT2046_Touchscreen.h>

// program includes
#include "config.h"     // default settings & eeprom storage
#include "hardware.h"   // hardware connections
#include "utils.h"      // tft routines and misc stuff


// prototypes
long readParamEncoder();
long readValueEncoder();
void readMixPot();
void readWahWahPot();
bool checkCompressorSwitch();
uint8_t readButtons();
void startGuitarTuner();
void playTone();
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
#include "Delayer.h"
#include "Flanger.h"
#include "Tremolo.h"
#include "WahWah.h"
#include "InputLevel.h"
#include "Tuner.h"



// number of menus
#define NUM_MENUS       7

// effect screens
#define EQ_SCREEN          0
#define COMPRESSOR_SCREEN  1
#define TREMOLO_SCREEN     2
#define FLANGER_SCREEN     3
#define REVERB_SCREEN      4
#define INPUT_SCREEN       5
#define DELAY_SCREEN       6

// menu vars
uint8_t menuIndex;
boolean initialScreenDrawn = false;
boolean msgFlag = false;
uint8_t tunerMode = 0;

// button timer
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



// *** start processing ***


void setup()
{
  // wait for cpu & lcd to initialize
  delay(1000);

  // start derial port
  Serial.begin(57600);

  // allocate memory for audio (mostly for delays)
  AudioMemory(256);

  // configure hardware i/o pins
  pinMode(TUNER_SWITCH_PIN, INPUT_PULLUP);
  pinMode(SAVE_SWITCH_PIN, INPUT_PULLUP);
  pinMode(COMPRESSOR_SWITCH_PIN, INPUT_PULLUP);
  pinMode(TEST_LED, OUTPUT);

  // turn on led to show we're awake
  digitalWrite(TEST_LED, ON);

  Serial.println("Teensy GEP");
  Serial.println(VERSION);

  // check for tuner button down, if so, enter dev mode
  if (digitalRead(TUNER_SWITCH_PIN) == 0)
  {
    devMode = true;
    Serial.println("Dev Mode Enabled");
  }

  // load configuration
  loadConfig();

#ifdef DEBUG_PRINT
  printConfig();
#endif

  // fire up the lcd screen
  initLCD();

  // show the splash screen
  splashScreen();
  delay(1000);

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


  Serial.println("Initialzing Effects");
  initCompressor();
  initEqualizer();
  initReverb();
  initDelayer();
  initTremolo();
  initFlanger();
  initWahWah();
  initInputLevel();


  // configure a sine wave for the test tone, disable
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

  // flush serial port recieve buffer
  while (Serial.available() > 0)
  {
    char c = Serial.read();
    Serial.print(c);
  }

  // let everyting settle
  delay(1000);

  // recall last menu used
  menuIndex = cfg.lastMenu;

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

    // is tuner button hijacked for alternate use?
    if (tunerMode == SHOW_CONFIG)
      printConfig();
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
        PCF.write(REVERB_LED, !reverbActive);
        message = reverbActive ? "Reverb ON" : "Reverb OFF";
        msgFlag = true;
        menuIndex = REVERB_SCREEN;
        break;

      case 0x02:  // Flanger Switch
        Serial.println("Toggle Flanger");
        toggleFlanger();
        PCF.write(FLANGER_LED, !flangerActive);
        message = flangerActive ? "Flanger ON" : "Flanger OFF";
        msgFlag = true;
        menuIndex = FLANGER_SCREEN;
        break;

      case 0x04:  // Tremolo switch
        Serial.println("Toggle Tremolo");
        toggleTremolo();
        PCF.write(TREMOLO_LED, !tremoloActive);
        message = tremoloActive ? "Tremolo ON" : "Tremolo OFF";
        msgFlag = true;
        menuIndex = TREMOLO_SCREEN;
        break;

      case 0x08:  // Wah-Wah switch
        Serial.println("Toggle Wah-Wah");
        // check if wah-wah button is hijacked
        if (devMode)
          playTone();
        else
        {
          toggleWahWah();
          PCF.write(WAH_WAH_LED, !wahWahActive);
          message = wahWahActive ? "Wah-Wah ON" : "Wah-Wah OFF";
          msgFlag = true;
        }
        break;

      case 0x81:
        toggleDelayer();
        //PCF.write(REVERB_LED, !delayerActive);
        message = delayerActive ? "Delay ON" : "Delay OFF";
        msgFlag = true;
        menuIndex = DELAY_SCREEN;
        break;

      default:
        message = "Unknown Switch Detected";
        msgFlag = true;
        break;
    }
  }


  // screen update loop  - continously call the current screen

  switch (menuIndex)
  {
    case EQ_SCREEN:
      doEqScreen();
      break;

    case COMPRESSOR_SCREEN:
      if (!initialScreenDrawn)
      {
        drawInitialCompressorScreen();
        initialScreenDrawn = true;
      }
      else
        updateCompressorScreen();
      break;

    case TREMOLO_SCREEN:
      if (!initialScreenDrawn)
        drawInitialTremoloScreen();
      else
        updateTremoloScreen();
      break;

    case FLANGER_SCREEN:
      if (!initialScreenDrawn)
        drawInitialFlangerScreen();
      else
        updateFlangerScreen();
      break;

    case REVERB_SCREEN:
      doReverbScreen();
      break;

    case INPUT_SCREEN:
      if (!initialScreenDrawn)
        drawInputLevelScreen();
      else
        updateInputLevelScreen();
      break;


    case DELAY_SCREEN:
      doDelayScreen();
      break;

    default:
      printValue("Error: Invalid Screen Selection", menuIndex);
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
    cfg.lastMenu = menuIndex;
  }

  // is there a message to display?
  if (msgFlag)
  {
    tftMessage(message);
    msgFlag = false;
    // redraw last screen
    initialScreenDrawn = false;
  }

  checkSerial();

  //uint32_t end = micros();
  //Serial.println(end - start);

  // slow down loop
  delay(50);
}



// *** functions ***


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
  pot -= 4;

  // ignore changes of less than 2 counts
  delta = pot - LastMixPotVal;
  if (abs(delta) > 2)
  {

    // convert 0 to 1023 to 0 to 1.0
    wet = (float)pot / 1020.0;

    // dry channel stays at 100% while wet goes 0% to 100%
    //dry = 1.0;

    // dry goes from 100% to 50% while wet goes 0% to 100%
    //dry = 1.0 - (wet / 2.0);

    // dry goes from 100% to 0% while wet goes 0% to 100%
    dry = 1.0 - wet;

    dry = constrain(dry, 0, 1.0);
    wet = constrain(wet, 0, 1.0);

    // Mixer 3 changes wet level based on mix pot
    mixer3.gain(0, dry);

    if (reverbActive)
    {
      mixer3.gain(1, wet);
      mixer3.gain(2, 0);
    }
    else
    {
      mixer3.gain(1, 0);
      mixer3.gain(2, wet);
    }

    printValue("dry", dry);
    printValue("wet", wet);

    LastMixPotVal = pot;
  }
}



void readWahWahPot()
{
  int pot = 0;
  int delta;

  // read pot
  pot = analogRead(WAH_WAH_POT);

  // ignore changes of less than 2 counts
  delta = pot - LastWahWahPotVal;
  if (abs(delta) > 2)
  {
    // scale from -1.0 to +1.0 and set dc source
    float val = -1.0 + ((float)pot / 500.0);
    val = constrain(val, -1.0, 1.0);
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

  // turn off activity led
  digitalWrite(TEST_LED, OFF);

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
  digitalWrite(TEST_LED, OFF);

  Serial.print("Button = 0x");
  Serial.println(button, HEX);
  return button;
}




void startGuitarTuner()
{
  // turn off effects mix
  mixer3.gain(DRY_CH,           1.0);
  mixer3.gain(WET_NO_REVERB_CH, 0.0);
  mixer3.gain(WET_REVERB,       0.0);

  // launch the guitar tuner
  guitarTuner();

  // must be done with tuner, reset stuff

  // reset mix
  readMixPot();

  // go to menu screen
  initialScreenDrawn = false;
  menuIndex = cfg.lastMenu;
}



void playTone()
{
  // enable mixer channel & leave on for now
  Serial.println("Playing Tone");

  // tone is gated using an envelope effect
  envelope1.noteOn();
  delay(36);
  envelope1.noteOff();
}



void playLongTone()
{
  // enable mixer channel & leave on for now
  Serial.println("Playing Tone");

  //mixer5.gain(0, 1.0);
  //mixer5.gain(1, 1.0);

  // tone is gated using an envelope effect
  envelope1.noteOn();

  // read pots while playing tone
  for (int j = 0; j < 1000; j++)
  {
    readMixPot();
    readWahWahPot();
    delay(10);
  }

  envelope1.noteOff();
}



void checkSerial()
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

        case 't':
          playTone();
          break;

        case 'l':
          playLongTone();
          break;

        case 'd':
          devMode = !devMode;
          break;

        case 'C':
          toggleCompressor();
          break;

        case 'D':
          toggleDelayer();
          break;

        case 'R':
          toggleReverb();
          break;

        case 'T':
          toggleTremolo();
          break;

        case 'F':
          toggleFlanger();
          break;

        case 'W':
          toggleWahWah();
          break;



        case '?':
          Serial.println(F("p: Print config"));
          Serial.println(F("m: print Memory usage"));
          Serial.println(F("s: print effects Status"));
          Serial.println(F("t: play test Tone"));
          Serial.println(F("l: play long test Tone"));
          Serial.println(F("d: toggle Dev mode"));

          Serial.println(F("C: toggle Compressor"));
          Serial.println(F("D: toggle Delayer"));
          Serial.println(F("R: toggle Reverb"));
          Serial.println(F("F: toggle Flanger"));
          Serial.println(F("T: toggle Tremolo"));
          Serial.println(F("W: toggle WahWah"));

          Serial.println(F("?: print help"));
          Serial.println();
          break;

        default:
          Serial.println(F("I'm sorry Dave, I afraid I can't do that"));
      }
    }
  }
}



void printStatus()
{
  printValue("reverb Active", reverbActive);
  printValue("flanger Active", flangerActive);
  printValue("delayer Active", delayerActive);
  printValue("tremolo Active", tremoloActive);
  printValue("wahWah Active", wahWahActive);
}
