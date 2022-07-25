/**********************************************************
   Flanger Effect Class
   Interface for the Flanger Effect.
   Provides adjustments to Speed and Depth of the effect.
   This effect is built-in to the SGTL5000 audio IC. The
    flanger uses a delay line, combining the original input
    with only one sample from the delay line, but the position
    of that sample varies sinusoidally. The sine input is
    adjustable via the 'Speed' slider, while the 'Depth' of
    the effect is adjust using a 2nd slider.

    Version 2.0     18Jan2020


    Create a flanger by specifying the address of the delayline,
    the total number of samples in the delay line (often done as
    an integer multiple of AUDIO_BLOCK_SAMPLES), the offset (how
    far back the flanged sample is from the original voice), the
    modulation depth (larger values give a greater variation) and
    the modulation frequency, in Hertz.


   Audio chain:
   I2SIn -> Mixer1 -> Effects -> Flange11 -> Mixer8-1 -> I2SOut
   

 * **************************************************************/

#ifndef FLANGER_H
#define FLANGER_H


#include "guiItems.h"

extern Encoder paramEncoder;
extern Encoder valueEncoder;


class Flanger {
  public:
    Flanger();
    void init();
    void disable();
    void enable();
    void toggle();
    bool getStatus();
    void printConfig();
    void process(bool);
    void setValues(float s, float d);
    void update();
    bool enabled;

  private:
    bool initialScreenDrawn;
    bool selectedItemChanged;
    bool itemValueChanged;
    int8_t selectedItem;
    int8_t lastselectedItem;
    
    static const uint8_t numSliders = 2;

    float sliderVal[numSliders];
    int16_t sliderXPos[numSliders] = {80, 200};
    int16_t sliderYPos = 2;

    String labels[numSliders] = {"Speed", "Depth"};
    int16_t labelPos[numSliders] = {75, 195};



    // a typical flanger effect
    static const int FLANGE_DELAY_LENGTH = 8 * AUDIO_BLOCK_SAMPLES;  // * 2 to 12
    static const int MAX_FLANGER_DEPTH   = FLANGE_DELAY_LENGTH / 4;  // * 1 to 3
    static const int SIDX                = FLANGE_DELAY_LENGTH / 4;  // / 4 to 8

    // more of a chorus-like effect
    //static const int FLANGE_DELAY_LENGTH = 12 * AUDIO_BLOCK_SAMPLES;
    //static const int MAX_FLANGER_DEPTH   = FLANGE_DELAY_LENGTH / 8;
    //static const int SIDX                = 3 * FLANGE_DELAY_LENGTH / 4;

    // create the delay buffer in memory
    short delayline[FLANGE_DELAY_LENGTH];

    void convertToSlider();
    void convertFromSlider();
    void drawScreen(bool);
    bool checkEncoders();
};




// this is run before audio board is initialized
Flanger :: Flanger()
{
  initialScreenDrawn = false;
  selectedItem = 0;
  lastselectedItem = 0;
  selectedItemChanged = false;
}



// run after audio board is initialized
void Flanger :: init()
{
  // start flanger
  flange1.begin(delayline, FLANGE_DELAY_LENGTH, SIDX, cfg.flangerDepth, cfg.flangerSpeed);
  update();
  disable();
  printValue("flanger initialized");
}




void Flanger :: disable()
{
  // set volume to 0
  mixer8_1.gain(FLANGER_IN, 0);

  enabled = false;
  printValue("flanger disabled");
}



void Flanger :: enable()
{
  // flanger is either enabled (gain = 1.0) or disabled (gain = 0)
  mixer8_1.gain(FLANGER_IN, 1.0);
  enabled = true;
  printValue("flanger enabled");
}


void Flanger :: toggle()
{
  if (enabled)
    disable();
  else
    enable();
}




bool Flanger :: getStatus()
{
  return enabled;
}



void Flanger :: setValues(float s, float d)
{
  cfg.flangerSpeed = s;
  cfg.flangerDepth = d;
}



void Flanger :: printConfig()
{
  printValue("Flanger:");
  printValue("Audio Blocks     ", AUDIO_BLOCK_SAMPLES);
  printValue("Flanger Max Depth", MAX_FLANGER_DEPTH);
  printValue("Flanger Delay Len", FLANGE_DELAY_LENGTH);
  printValue("Flanger Offset   ", SIDX);
  printValue("Flanger Depth    ", cfg.flangerDepth);
  printValue("Flanger Speed    ", cfg.flangerSpeed);
  
  Serial.print("Flanger Enabled = "); Serial.println(enabled);
  Serial.print("Flanger Speed   = "); Serial.println(cfg.flangerSpeed);
  Serial.print("Flanger Depth   = "); Serial.println(cfg.flangerDepth);
}


// does the actual changes to the audio board
void Flanger :: update()
{
  flange1.voices(SIDX, cfg.flangerDepth, cfg.flangerSpeed);
  printValue("Flanger Settings Updated");
}


// convert values to slider positions
void Flanger :: convertToSlider()
{
  // speed is a float 0.0 to max speed of 8 hz (at these settings)
  // depth is an int16, 0 to 10 or more depending on memory allocation
  sliderVal[0] = (cfg.flangerSpeed - 0.15) * 100.0 / 4.0;
  sliderVal[1] = (cfg.flangerDepth - 48) * 100 / MAX_FLANGER_DEPTH;

}



// convert slider positions to values
void Flanger :: convertFromSlider()
{
  // convert slider 0 to 100.0 to flanger values 0.0 to 1.0
  cfg.flangerSpeed = (sliderVal[0] / 25.0) + 0.15;
  cfg.flangerDepth = sliderVal[1] / (100.0 / (MAX_FLANGER_DEPTH - 48));

  cfg.flangerSpeed = constrain(cfg.flangerSpeed, 0.15, 4);
  cfg.flangerDepth = constrain(cfg.flangerDepth, 48, MAX_FLANGER_DEPTH);
}




// does the actual drawing to the LCD
void Flanger :: drawScreen(bool drawAll)
{
  if (drawAll)
  {
    eraseScreen();

    // add the lables
    drawLabels(numSliders, labelPos, labels);

    // add the title
    drawTitle("Flanger");

    for (uint8_t i = 0; i < numSliders; i++)
    {
      if (i == selectedItem)
        drawSlider(sliderXPos[i], sliderYPos, sliderVal[i], true);
      else
        drawSlider(sliderXPos[i], sliderYPos, sliderVal[i], false);
    }
  }
  else
  {
    drawSlider(sliderXPos[selectedItem], sliderYPos, sliderVal[selectedItem], true);
  }
}



void Flanger :: process(bool initScreen)
{
  selectedItemChanged = false;
  itemValueChanged = false;

  // set the sliders values to the current stored settings
  if (initScreen)
    convertToSlider();


  // allow some time for user to rotate encoders
  delay(50);

  // read encoders and check for changes
  checkEncoders();

  if (selectedItemChanged || initScreen)
  {
    drawScreen(true);
    printConfig();
  }
  else if (itemValueChanged)
  {
    convertFromSlider();
    update();
    drawScreen(false);
  }

  // reset encoder values for new delta
  lastParamEncVal = paramEncVal;
  lastValEncVal = valEncVal;
}



bool Flanger :: checkEncoders()
{
  // checks both param & value encoders for changes.
  // Read the param encoder first, which selects the
  // gui item. If changed, exits
  // If no changes, the value encoder
  // is read. Changes to value encoder increase or decrease
  // the value of the "selected item"

  paramEncVal = readParamEncoder() / 2;

  if (paramEncVal != lastParamEncVal)
  {
    printValue("paramEncVal", paramEncVal);
    printValue("lastParamEncVal", lastParamEncVal);

    // save the current selected item
    lastselectedItem = selectedItem;

    // and then move to next or previous item, wraps-around
    if (paramEncVal > lastParamEncVal)
      selectedItem++;
    else if (paramEncVal < lastParamEncVal)
      selectedItem--;

    selectedItem = selectedItem % numSliders;
    printValue("selectedItem", selectedItem);
    selectedItemChanged = true;
    return selectedItemChanged;
  }
  else
  {
    // if no param encoder changes, then read Value Encoder
    valEncVal = readValueEncoder() / 2;
    if (valEncVal != lastValEncVal)
    {
      // increase or decrease the slider's new position (need to re-draw later)
      if (valEncVal > lastValEncVal)
        sliderVal[selectedItem] += 4;

      else if (valEncVal < lastValEncVal)
        sliderVal[selectedItem] -= 4;

      // keep it in the slider's range
      sliderVal[selectedItem] = constrain(sliderVal[selectedItem], 0, 100.0);
      itemValueChanged = true;
      printValue("sliderVal[selectedItem]", sliderVal[selectedItem]);
      return itemValueChanged;
    }
    return false;
  }
}

#endif
