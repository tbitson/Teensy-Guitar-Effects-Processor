/******************************************************
   config.h

   Global settings, variables, and functions to
   save and load to non-volatile memory (EEPROM).
   Set the default values if EEPROM contents do
   not match the current version.

   version 1.2   Jan 2020


 ********************************************************/

#ifndef CONFIG_H
#define CONFIG_H


#include <EEPROM.h>



// prototypes
void loadDefaults();
bool saveConfig();
bool loadConfig();
void clearEEPROM();
void showEEPROM();



// if the first byte of stored data matches this, it
// is assumed valid data for this version
#define EEPROM_VERSION 183
#define EEPROM_ADDR    0

// equalizer bands
#define NUM_EQ_BANDS  5
enum eqBands {BASS, MID_BASS, MIDRANGE, MID_TREBLE, TREBLE};

#define HEADPHONE_OUTPUT_LEVEL 0.7
#define LINEOUT_AUDIO_LEVEL    8


// dev mode - alters some buttons to execute debugging functions
// hold down Tuner button at startup to enter. Stuff that is altered
// changes depending on what's being debugged.
bool devMode = false;


// the settings saved to non-volatile memory
struct Config
{
  byte    vers;

  float   eqBandVals[5];
  int     updateFilter[5];

  bool    compEnabled;
  uint8_t compGain;
  uint8_t compResponse;
  uint8_t compLimit;
  float   compThreshold;
  float   compAttack;
  float   compDecay;

  float   tremoloVolume;
  float   tremoloSpeed;
  float   tremoloDepth;

  float   flangerSpeed;
  int16_t flangerDepth;

  float   reverbVolume;
  float   reverbRoomsize;
  float   reverbDamping;

  float   delayTimes[4];
  float   delayVols[4];
  float   recirculate;

  float   chorusVoices;
  float   chorusVolume;

  uint8_t inputLevel;

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
  cfg.compGain      = 1;       // 0 = 0db, 1 = 6db, 2 = 12db
  cfg.compResponse  = 1;       // integration time 0 = 0ms, 1 = 25ms, 2 = 50ms, 3 = 100ms
  cfg.compLimit     = 0;       // 0 = soft knee, 1 = hard limit, not in gui
  cfg.compThreshold = -20;     // 0 to -96dbBFS
  cfg.compAttack    = 3.0;     // db per sec
  cfg.compDecay     = 4.0;     // db per sec

  // tremolo
  cfg.tremoloVolume = 0.8;     // 0 to 1.0
  cfg.tremoloSpeed  = 3.0;     // 0.5 to 8.0 Hz
  cfg.tremoloDepth  = 0.4;     // 0 to 1.0

  // reverb
  cfg.reverbVolume   = 0.7;    // 0 to 1.0
  cfg.reverbRoomsize = 0.5;    // 0 to 1.0
  cfg.reverbDamping  = 0.3;    // 0 to 1.0

  // delayer
  cfg.delayTimes[0] = 200;     // delay in ms, 0 to 1000
  cfg.delayTimes[1] = 0;       // delay in ms, 0 to 1000
  cfg.delayVols[0]  = 0.8;     // delay chan volume 0 to 1.0
  cfg.delayVols[1]  = 0.0;     // delay chan volume 0 to 1.0
  cfg.recirculate   = 0.0;     // delay Recirculate aka feedback level 0 to 1.0

  // flanger
  cfg.flangerSpeed    = 1.6;   // effects rate, Hz
  cfg.flangerDepth    = 128;   // effect 'strength'

  // chorus effect
  cfg.chorusVoices    = 2;
  cfg.chorusVolume    = 0.5;

  // input level
  cfg.inputLevel      = 5;     // 0 to 15, 5 = 1.33vpp

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
    //clearEEPROM();
  }

  return false;
}



bool saveConfig()
{
  // don't store if version is 0, used for dev to force defaults
  if (EEPROM_VERSION == 0)
    return false;

  cfg.vers = EEPROM_VERSION;
  EEPROM.put(EEPROM_ADDR, cfg);
  Serial.print("Saved "); Serial.print(sizeof(cfg)); Serial.println(" bytes");
  return true;
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
