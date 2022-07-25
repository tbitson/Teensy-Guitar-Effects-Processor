/**************************************************************
 *  patches. h - audio functions from the Teensy Audio Tool
 *  and a few defines used to reference the audio objects
 *  in the code, so if you change audio configuration, you
 *  *may* only have to update these. (all code has not been
 *  changed to reference these yet)
 *
 * Paste output from the Audio Tool in this file
 *
 *  version 180  Sept 2020
 *
 *
 **************************************************************/


#ifndef PATCHES_H
#define PATCHES_H

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveformSine   sine1;          //xy=59.5,385
AudioSynthWaveformDc     dc1;            //xy=60.5,445
AudioSynthWaveformDc     dc2;            //xy=63.5,316
AudioSynthWaveformSine   sine2;          //xy=75.5,188
AudioInputI2S            i2s1;           //xy=76.5,140
AudioMixer4              mixer1;         //xy=227.5,193
AudioMixer4              mixer2;         //xy=262.5,442
AudioMixer4              mixer5;         //xy=322,525
AudioAnalyzePeak         peak1;          //xy=415.5,107
AudioAnalyzeNoteFrequency notefreq1;      //xy=417.5,57
AudioEffectChorus        chorus1;        //xy=477.5,286
AudioEffectFreeverb      freeverb1;      //xy=479.5,233
AudioEffectDelayExternal delayExt1;      //xy=478.5,526
AudioFilterStateVariable filter1;        //xy=479.5,340
AudioEffectFlange        flange1;        //xy=480.5,392
AudioEffectMultiply      multiply1;      //xy=481.5,439
AudioMixer4              mixer3;         //xy=485.5,645
AudioMixer8              mixer8_1;       //xy=708.5,382
AudioMixer4              mixer4;         //xy=829.5,216
AudioFilterBiquad        biquad1;        //xy=980.5,214
AudioAnalyzePeak         peak2;          //xy=982.5,114
AudioOutputI2S           i2s2;           //xy=1132.5,199
AudioConnection          patchCord1(sine1, 0, mixer2, 0);
AudioConnection          patchCord2(dc1, 0, mixer2, 1);
AudioConnection          patchCord3(dc2, 0, filter1, 1);
AudioConnection          patchCord4(sine2, 0, mixer1, 1);
AudioConnection          patchCord5(i2s1, 0, mixer1, 0);
AudioConnection          patchCord6(mixer1, 0, filter1, 0);
AudioConnection          patchCord7(mixer1, peak1);
AudioConnection          patchCord8(mixer1, flange1);
AudioConnection          patchCord9(mixer1, 0, multiply1, 0);
AudioConnection          patchCord10(mixer1, freeverb1);
AudioConnection          patchCord11(mixer1, chorus1);
AudioConnection          patchCord12(mixer1, 0, mixer4, 0);
AudioConnection          patchCord13(mixer1, notefreq1);
AudioConnection          patchCord14(mixer1, 0, mixer5, 0);
AudioConnection          patchCord15(mixer2, 0, multiply1, 1);
AudioConnection          patchCord16(mixer5, delayExt1);
AudioConnection          patchCord17(chorus1, 0, mixer8_1, 1);
AudioConnection          patchCord18(freeverb1, 0, mixer8_1, 0);
AudioConnection          patchCord19(delayExt1, 0, mixer3, 0);
AudioConnection          patchCord20(delayExt1, 1, mixer3, 1);
AudioConnection          patchCord21(filter1, 0, mixer8_1, 2);
AudioConnection          patchCord22(flange1, 0, mixer8_1, 3);
AudioConnection          patchCord23(multiply1, 0, mixer8_1, 4);
AudioConnection          patchCord24(mixer3, 0, mixer8_1, 5);
AudioConnection          patchCord25(mixer3, 0, mixer5, 1);
AudioConnection          patchCord26(mixer8_1, 0, mixer4, 1);
AudioConnection          patchCord27(mixer4, biquad1);
AudioConnection          patchCord28(mixer4, peak2);
AudioConnection          patchCord29(biquad1, 0, i2s2, 0);
AudioControlSGTL5000     audioShield;    //xy=72.5,540
// GUItool: end automatically generated code




// input mixer 1 channels
#define INPUT_MIXER     1
#define LEFT_IN         0
#define TEST_TONE       1
#define DELAY_FEEDBACK  2


// tremolo control mixer 2
#define TREMOLO_MIXER   2
#define SINE_IN         0
#define DC_IN           1


// delay output mixer 3
#define DELAY_OUT_MIXER 3
#define DELAY_1         0
#define DELAY_2         1


// output/effects mixer 8_1
#define EFFECTS_MIXER8  1
#define REVERB_IN       0
#define CHORUS_IN       1
#define WAH_WAH_IN      2
#define FLANGER_IN      3
#define TREMOLO_IN      4
#define DELAY_IN        5


// delay input mixer
#define DELAY_MIXER     5
#define DELAY_DRY_IN    0
#define DALAY_FEEDBACK  1

// output mixer 4 channels
#define OUTPUT_MIXER    4
#define DRY_OUT         0
#define WET_OUT         1


#endif
