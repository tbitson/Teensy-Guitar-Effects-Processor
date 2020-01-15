/*
    patches. h - audio functions from the Teensy Audio Tool

    Paste output from the Audio Tool in this file

    version 2.0
    Dec 2019



*/

#ifndef PATCHES_H
#define PATCHES_H

// use these names in the code so if config
// changes, you only have to adjust these

// mixer names
#define EFFECT_MIXER   mixer1
#define TREMOLO_MIXER  mixer2
#define MIX_MIXER      mixer3

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

// off or on macros
//#define UNITY_GAIN 1.0
#define ZERO_GAIN   0

// ----------------------- Insert GUI Tool Output -----------------------------------
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
//#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=120.82356262207031,216.00001525878906
AudioSynthWaveformDc     dc2;            //xy=120.82356262207031,300.0000457763672
AudioSynthWaveformSine   sine1;          //xy=126.82356262207031,382.0000457763672
AudioSynthWaveformDc     dc1;            //xy=126.82356262207031,441.0000457763672
AudioSynthWaveformSine   sine2;          //xy=129.8235626220703,550.0000152587891
AudioMixer4              mixer2;         //xy=325.82359313964844,459.00001525878906
AudioAnalyzePeak         peak1;          //xy=400.8235626220703,55.00001525878906
AudioAnalyzeNoteFrequency notefreq1;      //xy=400.8235626220703,116.00001525878906
AudioEffectFlange        flange1;        //xy=495.8235626220703,357.00001525878906
AudioEffectFreeverb      freeverb1;      //xy=496.8235626220703,233.00001525878906
AudioFilterStateVariable filter1;        //xy=496.8235626220703,295.00001525878906
AudioEffectMultiply      multiply1;      //xy=497.8235626220703,431.00001525878906
AudioEffectEnvelope      envelope1;      //xy=762.8235626220703,547.0000152587891
AudioMixer4              mixer1;         //xy=778.8235626220703,289.0000457763672
AudioMixer4              mixer3;         //xy=982.8236236572266,196.00001525878906
AudioOutputI2S           i2s2;           //xy=1164.8235626220703,251.00001525878906
AudioAnalyzePeak         peak2;          //xy=1173.8235168457031,89.82353210449219
AudioConnection          patchCord1(i2s1, 0, multiply1, 0);
AudioConnection          patchCord2(i2s1, 0, flange1, 0);
AudioConnection          patchCord3(i2s1, 0, notefreq1, 0);
AudioConnection          patchCord4(i2s1, 0, peak1, 0);
AudioConnection          patchCord5(i2s1, 0, filter1, 0);
AudioConnection          patchCord6(i2s1, 0, freeverb1, 0);
AudioConnection          patchCord7(i2s1, 0, mixer3, 0);
AudioConnection          patchCord8(dc2, 0, filter1, 1);
AudioConnection          patchCord9(sine1, 0, mixer2, 0);
AudioConnection          patchCord10(dc1, 0, mixer2, 1);
AudioConnection          patchCord11(sine2, envelope1);
AudioConnection          patchCord12(mixer2, 0, multiply1, 1);
AudioConnection          patchCord13(flange1, 0, mixer1, 2);
AudioConnection          patchCord14(freeverb1, 0, mixer1, 0);
AudioConnection          patchCord15(filter1, 0, mixer1, 1);
AudioConnection          patchCord16(multiply1, 0, mixer1, 3);
AudioConnection          patchCord17(envelope1, 0, mixer3, 2);
AudioConnection          patchCord18(mixer1, 0, mixer3, 1);
AudioConnection          patchCord19(mixer3, 0, i2s2, 0);
AudioConnection          patchCord20(mixer3, 0, i2s2, 1);
AudioConnection          patchCord21(mixer3, peak2);
AudioControlSGTL5000     audioShield;    //xy=125.82356262207031,686.0000152587891
// GUItool: end automatically generated code


//---------------------------------------------------------------------------------------



#endif
