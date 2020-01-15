/**************************************************************
 *  patches. h - audio functions from the Teensy Audio Tool
 *  and a few defines used to reference the audio objects
 *  in the code, so if you change audio configuration, you
 *  *may* only have to update these. (all code has not been
 *  changed to reference these yet)
 *
 * Paste output from the Audio Tool in this file
 *
 *  THIS VERSION APPLYS TEST TONE INSTEAD OF AUDIO INPUT
 *  Used to generate a test tone without needing an audio
 *  input source. It is a temp fix until the bug using a
 *  separate mixer is resolved. Replace patches.h with this file.
 *
 *  version 167t  14Jan2020
 *
 *
 **************************************************************/


#ifndef PATCHES_H
#define PATCHES_H

// use these names in the code if you want so if config
// changes, you only have to adjust these

// mixer names
#define EFFECT_MIXER   mixer1
#define TREMOLO_MIXER  mixer2
#define MIX_MIXER      mixer3
#define DELAY_MIXER    mixer4

// mixer 1 Effects Select channel definitions
#define WAHWAH     0
#define FLANGER    1
#define TREMOLO    2
#define DELAY_CH   3

// mixer 2 tremolo input (probably wont change)
#define T_SINE_IN_CH  0
#define T_DC_IN_CH    1

// mixer 3 Effects Mix channel definitions
#define DRY_CH              0
#define WET_NO_REVERB_CH    1   // with reverb
#define WET_REVERB          2   // without reverb
//
// mixer 4 channel definitions
#define DELAY1     0
#define DELAY2     1
#define DELAY3     2
#define DELAY4     3

// mixer 5 not connected but still in place to
// allow main line code to work
#define AUDIO_IN    0
#define TEST_TONE   1

// off or on macros
#define ZERO_GAIN   0

// ----------------------- Insert GUI Tool Output -----------------------------------

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveformSine   sine2;          //xy=64,247
AudioInputI2S            i2s1;           //xy=72,168
AudioSynthWaveformDc     dc2;            //xy=78,377
AudioSynthWaveformSine   sine1;          //xy=81,444
AudioSynthWaveformDc     dc1;            //xy=81,503
AudioEffectEnvelope      envelope1;      //xy=232,246
AudioMixer4              mixer2;         //xy=280,521
AudioEffectDelay         delay1;         //xy=528,546
AudioEffectMultiply      multiply1;      //xy=641,394
AudioEffectFlange        flange1;        //xy=644,322
AudioFilterStateVariable filter1;        //xy=649,256
AudioAnalyzeNoteFrequency notefreq1;      //xy=662,130
AudioAnalyzePeak         peak1;          //xy=672,64
AudioMixer4              mixer4;         //xy=717,504
AudioMixer4              mixer1;         //xy=915,242
AudioEffectFreeverb      freeverb1;      //xy=1045,415
AudioMixer4              mixer3;         //xy=1150,211
AudioAnalyzePeak         peak2;          //xy=1324,92
AudioOutputI2S           i2s2;           //xy=1345,234
AudioConnection          patchCord1(sine2, envelope1);
AudioConnection          patchCord2(dc2, 0, filter1, 1);
AudioConnection          patchCord3(sine1, 0, mixer2, 0);
AudioConnection          patchCord4(dc1, 0, mixer2, 1);
AudioConnection          patchCord5(envelope1, peak1);
AudioConnection          patchCord6(envelope1, notefreq1);
AudioConnection          patchCord7(envelope1, 0, filter1, 0);
AudioConnection          patchCord8(envelope1, 0, mixer3, 0);
AudioConnection          patchCord9(envelope1, flange1);
AudioConnection          patchCord10(envelope1, 0, multiply1, 0);
AudioConnection          patchCord11(envelope1, delay1);
AudioConnection          patchCord12(mixer2, 0, multiply1, 1);
AudioConnection          patchCord13(delay1, 0, mixer4, 0);
AudioConnection          patchCord14(delay1, 1, mixer4, 1);
AudioConnection          patchCord15(delay1, 2, mixer4, 2);
AudioConnection          patchCord16(delay1, 3, mixer4, 3);
AudioConnection          patchCord17(multiply1, 0, mixer1, 2);
AudioConnection          patchCord18(flange1, 0, mixer1, 1);
AudioConnection          patchCord19(filter1, 0, mixer1, 0);
AudioConnection          patchCord20(mixer4, 0, mixer1, 3);
AudioConnection          patchCord21(mixer1, freeverb1);
AudioConnection          patchCord22(mixer1, 0, mixer3, 1);
AudioConnection          patchCord23(freeverb1, 0, mixer3, 2);
AudioConnection          patchCord24(mixer3, 0, i2s2, 0);
AudioConnection          patchCord25(mixer3, 0, i2s2, 1);
AudioConnection          patchCord26(mixer3, peak2);
AudioControlSGTL5000     audioShield;    //xy=75,113
// GUItool: end automatically generated code



//---------------------------------------------------------------------------------------



#endif
