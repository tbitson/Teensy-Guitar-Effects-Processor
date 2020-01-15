/*******************************************************************************
   Global settings variables and functions to
   save and load to non-volatile memory (EEPROM)

   version 1.1   Jan 2020


*******************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H


#include <EEPROM.h>



// prototypes
void loadDefaults();
bool saveConfig();
bool loadConfig();
void printConfig();
void clearEEPROM();
void showEEPROM();



// if the first byte of stored data matches this, it
// is assumed valid data for this version
#define EEPROM_VERSION 161
#define EEPROM_ADDR    0


// equalizer bands
enum eqBands {BASS, MID_BASS, MIDRANGE, MID_TREBLE, TREBLE};


// *** default settings NOT avaiable in GUI  nor stored in eeprom ***

#define HEADPHONE_OUTPUT_LEVEL 0.7
#define LINEOUT_AUDIO_LEVEL    8

// wah-wah settings
const int WAH_WAH_CENTER_FREQ = 500;
const int WAH_WAH_GAIN        = 4;
const int WAH_WAH_OCTAVES     = 2;  // +/- 2 octaves

// flanger
// Number of samples in each delay line
const int FLANGE_DELAY_LENGTH = 8 * AUDIO_BLOCK_SAMPLES;  // * 2 to 12
const int MAX_FLANGER_DEPTH   = FLANGE_DELAY_LENGTH / 4;  // * 1 to 3
const int SIDX                = FLANGE_DELAY_LENGTH / 4;  // / 4 to 8

// more of a chorus-like effect for flanger
//const int FLANGE_DELAY_LENGTH = 12 * AUDIO_BLOCK_SAMPLES;
//const int MAX_FLANGER_DEPTH   = FLANGE_DELAY_LENGTH / 8;
//const int SIDX                = 3 * FLANGE_DELAY_LENGTH / 4;
// create the delay buffer in memory
short delayline[FLANGE_DELAY_LENGTH];


// minimum change to trigger pot reading (eliminates false trigger)
#define POT_DELTA_VALUE 20

// dev mode - alters some buttons to execute debugging functions
// hold down Tuner button at startup to enter. Stuff that is altered
// changes depending on what's being debugged.
bool devMode = false;



struct Config
{
  byte    vers;

  float   eqBandVals[5];
  int     updateFilter[5];

  bool    compEnabled;
  uint8_t compGain;
  uint8_t compResponse;
  uint8_t compHardLimit;
  float   compThreshold;
  float   compAttack;
  float   compDecay;

  float   tremoloSpeed;
  float   tremoloDepth;

  float   flangerSpeed;
  int     flangerDepth;

  float   reverbVolume;
  float   reverbRoomsize;
  float   reverbDamping;

  int     inputValueAdj;

  uint8_t lastMenu;
};

// create instance of configuration struct
Config cfg;



void loadDefaults()
{
  // set up defaults
  cfg.vers                    = EEPROM_VERSION;

  // equalizer - set for flat response
  cfg.eqBandVals[BASS]        = 0.0;
  cfg.eqBandVals[MID_BASS]    = 0.0;
  cfg.eqBandVals[MIDRANGE]    = 0.0;
  cfg.eqBandVals[MID_TREBLE]  = 0.0;
  cfg.eqBandVals[TREBLE]      = 0.0;

  // compressor
  cfg.compEnabled   = false;
  cfg.compGain      = 1;      // 0, 1, 2 = 0, 6, 12db
  cfg.compResponse  = 1;      // integration time 0,1,2,3 = 0,25,50,100ms
  cfg.compHardLimit = 0;      // 0 or 1, 0 = soft knee, not in gui
  cfg.compThreshold = -18;    // 0 to -96dbBFS
  cfg.compAttack    = 0.6;    // db per sec
  cfg.compDecay     = 0.6;    // db per sec

  // tremolo
  cfg.tremoloSpeed     = 3.0;        // Hz
  cfg.tremoloDepth     = 0.4;        // 0 to 1.0

  // reverb
  cfg.reverbVolume    = 0.7;        // 0 to 1.0
  cfg.reverbRoomsize  = 0.5;        // 0 to 1.0
  cfg.reverbDamping   = 0.3;        // 0 to 1.0

  // flanger
  cfg.flangerDepth    = 0.4;        //
  cfg.flangerSpeed    = 1.0;        //

  // input level
  cfg.inputValueAdj   = 5;        // 0 to 15, 5 = 1.33vpp

  // general
  cfg.lastMenu        = 0;
}



bool loadConfig()
{
  byte version;

  // init eeprom library
  EEPROM.begin();
  delay(100);
  
  // load default settings
  loadDefaults();

  // check version of stored config
  EEPROM.get(EEPROM_ADDR, version);
  Serial.print("EEPROM Stored Version   = "); Serial.println(version);
  Serial.print("EEPROM Required Version = "); Serial.println(EEPROM_VERSION);

  if (version == EEPROM_VERSION)
  {
    EEPROM.get(EEPROM_ADDR, cfg);
    delay(200);
    Serial.println("Stored Config loaded");
    Serial.print("loaded "); Serial.print(sizeof(cfg)); Serial.println(" bytes");
    return true;
  }
  else
  {
    Serial.println("Config NOT loaded, using defaults");
    // clearEEPROM();
  }

  return false;
}



bool saveConfig()
{
  // don't store if version is 0, used for dev
  if (EEPROM_VERSION == 0)
    return false;

  cfg.vers = EEPROM_VERSION;
  EEPROM.put(EEPROM_ADDR, cfg);
  Serial.print("Saved "); Serial.print(sizeof(cfg)); Serial.println(" bytes");
  return true;
}






void printConfig()
{
  Serial.println();
  Serial.println(F("Current Config:"));
  Serial.print(F("Version = "));          Serial.println(cfg.vers);
  Serial.print(F("EQ Bass        = "));   Serial.println(cfg.eqBandVals[BASS]);
  Serial.print(F("EQ Mid-Bass    = "));   Serial.println(cfg.eqBandVals[MID_BASS]);
  Serial.print(F("EQ Midrange    = "));   Serial.println(cfg.eqBandVals[MIDRANGE]);
  Serial.print(F("EQ Mid-Treble  = "));   Serial.println(cfg.eqBandVals[MID_TREBLE]);
  Serial.print(F("EQ Treble      = "));   Serial.println(cfg.eqBandVals[TREBLE]);
  delay(100);
  Serial.print(F("Comp Enabled   = "));   Serial.println(cfg.compEnabled);
  Serial.print(F("Comp Gain      = "));   Serial.println(cfg.compGain);
  Serial.print(F("Comp Response  = "));   Serial.println(cfg.compResponse);
  Serial.print(F("Comp Limit     = "));   Serial.println(cfg.compHardLimit);
  Serial.print(F("Comp Threshold = "));   Serial.println(cfg.compThreshold);
  Serial.print(F("Comp Attack    = "));   Serial.println(cfg.compAttack);
  Serial.print(F("Comp Decay     = "));   Serial.println(cfg.compDecay);
  delay(200);
  Serial.print(F("Tremolo Speed  = "));   Serial.println(cfg.tremoloSpeed);
  Serial.print(F("Tremolo Depth  = "));   Serial.println(cfg.tremoloDepth);
  Serial.print(F("Flanger Speed  = "));   Serial.println(cfg.flangerSpeed);
  Serial.print(F("Flanger Depth  = "));   Serial.println(cfg.flangerDepth);
  delay(200);
  Serial.print(F("Reverb Volume   = "));  Serial.println(cfg.reverbVolume);
  Serial.print(F("Reverb Roomsize = "));  Serial.println(cfg.reverbRoomsize);
  Serial.print(F("Reverb Damping  = "));  Serial.println(cfg.reverbDamping);
  Serial.print(F("Input Level Adj = "));  Serial.println(cfg.inputValueAdj);
  Serial.print(F("Last Menu       = "));  Serial.println(cfg.lastMenu);
  Serial.println();
  delay(200);
}




// clear eeprom contents (dev use only so far)
void clearEEPROM()
{
  Serial.println("Clearing EEPROM...");

  for (int i = 0 ; i < EEPROM.length() ; i++)
    EEPROM.write(i, 0xFF);

  Serial.println("done");
}



// show eeprom contents (dev use only so far)
void showEEPROM()
{
  byte b;

  Serial.println("EEPROM contents:");

  for (int i = 0 ; i < EEPROM.length() ; i++)
  {
    EEPROM.get(i, b);

    Serial.print("Addr 0x");
    Serial.print(i, HEX);
    Serial.print(" = ");
    Serial.println(b);
  }

  Serial.println();
}

#endif
