/*
    patches. h - audio functions from the Teensy Audio Tool

    Paste output from the Audio Tool in this file

    version 2.0
    Dec 2019



*/



#ifndef PATCHES_H
#define PATCHES_H

// use these names in the code if you want so if config
// changes, you only have to adjust these

// mixer names
#define EFFECT_MIXER   mixer1
#define TREMOLO_MIXER  mixer2
#define MIX_MIXER      mixer3
#define DELAY_MIXER    mixer4

// mixer 1 channel definitions
#define REVERB      0
#define TREMOLO     3
#define FLANGER     2
#define WAHWAH      1

// mixer 2 tremolo input (probably wont change)
#define T_SINE_IN_CH  0
#define T_DC_IN_CH    1

// mixer 3 channel definitions
#define DRY_CH     0
#define WET_CH     1
#define TONE_CH    2
#define DELAY_CH   3

// mixer 4 channel definitions
#define DELAY1     0
#define DELAY2     1
#define DELAY3     2
#define DELAY4     3

// off or on macros
#define ZERO_GAIN   0

// ----------------------- Insert GUI Tool Output -----------------------------------

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=150,195
AudioSynthWaveformDc     dc2;            //xy=150,279
AudioSynthWaveformSine   sine1;          //xy=156,361
AudioSynthWaveformDc     dc1;            //xy=156,420
AudioSynthWaveformSine   sine2;          //xy=159,529
AudioMixer4              mixer2;         //xy=355,438
AudioAnalyzePeak         peak1;          //xy=430,34
AudioAnalyzeNoteFrequency notefreq1;      //xy=430,95
AudioEffectFlange        flange1;        //xy=525,336
AudioEffectFreeverb      freeverb1;      //xy=526,212
AudioFilterStateVariable filter1;        //xy=526,274
AudioEffectMultiply      multiply1;      //xy=527,410
AudioEffectEnvelope      envelope1;      //xy=726,526
AudioMixer4              mixer1;         //xy=808,268
AudioEffectDelay         delay1;         //xy=1022,491
AudioMixer4              mixer3;         //xy=1132,181
AudioMixer4              mixer4;         //xy=1213,487
AudioAnalyzePeak         peak2;          //xy=1301,98
AudioOutputI2S           i2s2;           //xy=1317,229
AudioConnection          patchCord1(i2s1, 0, multiply1, 0);
AudioConnection          patchCord2(i2s1, 0, flange1, 0);
AudioConnection          patchCord3(i2s1, 0, notefreq1, 0);
AudioConnection          patchCord4(i2s1, 0, peak1, 0);
AudioConnection          patchCord5(i2s1, 0, filter1, 0);
AudioConnection          patchCord6(i2s1, 0, freeverb1, 0);
AudioConnection          patchCord7(i2s1, 0, mixer3, 0);
AudioConnection          patchCord8(i2s1, 0, delay1, 0);
AudioConnection          patchCord9(dc2, 0, filter1, 1);
AudioConnection          patchCord10(sine1, 0, mixer2, 0);
AudioConnection          patchCord11(dc1, 0, mixer2, 1);
AudioConnection          patchCord12(sine2, envelope1);
AudioConnection          patchCord13(mixer2, 0, multiply1, 1);
AudioConnection          patchCord14(flange1, 0, mixer1, 2);
AudioConnection          patchCord15(freeverb1, 0, mixer1, 0);
AudioConnection          patchCord16(filter1, 0, mixer1, 1);
AudioConnection          patchCord17(multiply1, 0, mixer1, 3);
AudioConnection          patchCord18(envelope1, 0, mixer3, 2);
AudioConnection          patchCord19(mixer1, 0, mixer3, 1);
AudioConnection          patchCord20(delay1, 0, mixer4, 0);
AudioConnection          patchCord21(delay1, 1, mixer4, 1);
AudioConnection          patchCord22(delay1, 2, mixer4, 2);
AudioConnection          patchCord23(delay1, 3, mixer4, 3);
AudioConnection          patchCord24(mixer3, 0, i2s2, 0);
AudioConnection          patchCord25(mixer3, 0, i2s2, 1);
AudioConnection          patchCord26(mixer3, peak2);
AudioConnection          patchCord27(mixer4, 0, mixer3, 3);
AudioControlSGTL5000     audioShield;    //xy=155,665
// GUItool: end automatically generated code

// GUItool: end automatically generated code

//---------------------------------------------------------------------------------------



#endif
