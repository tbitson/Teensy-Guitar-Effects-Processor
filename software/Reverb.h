
/***********************************************
   Reverb Effect Class

   Interface to the 'freeverb' reverb effect
   ioncluded in the TAL.Provides adjustment
   for volume (level), roomsize, and damping.

   version 1.0    16Jan2020

   sub-classes effect

   TAL settings:
   roomsize: float 0.0 to 1.0
   damping : float 0.0 to 1.0
   volume: uses mixer input, float 0.0 to 1.0

   Audio chain:
   I2SIn -> Mixer1 -> Reverb -> Mixer8_1 -> I2SOut

 * *******************************************/

#ifndef REVERB_H
#define REVERB_H


#include "guiItems.h"

extern Encoder paramEncoder;
extern Encoder valueEncoder;


class Reverb {
  public:
    Reverb();
    void init();
    void disable();
    void enable();
    void toggle();
    bool getStatus();
    void printConfig();
    void process(bool);
    void setValues(float v, float r, float d);
    void update();
    bool enabled;


  private:
    bool initialScreenDrawn;
    bool selectedItemChanged;
    bool itemValueChanged;
    int8_t selectedItem;
    int8_t lastselectedItem;

    static const uint8_t numSliders = 3;
    float sliderVal[3];

    int16_t sliderXPos[numSliders] = {35, 135, 235};
    int16_t sliderYPos = 2;

    String labels[numSliders] = {"Volume", "Roomsize", "Damping"};
    int16_t labelPos[numSliders] = {20, 120, 220};

    void convertToSlider();
    void convertFromSlider();
    bool checkEncoders();
    void drawScreen(bool);
};



// this is run before audio board is initialized
Reverb :: Reverb()
{
  initialScreenDrawn = false;
  selectedItem = 0;
  lastselectedItem = 0;
  selectedItemChanged = false;
  printValue("Reverb inited");
}



// run after audio board is initialized
void Reverb :: init()
{
  update();
  disable();
}


void Reverb :: disable()
{
  // bypass reverb section with same level
  mixer8_1.gain(REVERB_IN, 0);
  enabled = false;
  printValue("Reverb disabled");
}


void Reverb :: enable()
{
  // adjust audio reverb vs dry ratio
  mixer8_1.gain(REVERB_IN, cfg.reverbVolume);
  enabled = true;
  printValue("Reverb enabled");
}


void Reverb :: toggle()
{
  if (enabled)
    disable();
  else
    enable();
}



bool Reverb :: getStatus()
{
  return enabled;
}


void Reverb :: setValues(float v, float r, float d)
{
  cfg.reverbVolume = v;
  cfg.reverbRoomsize = r;
  cfg.reverbDamping = d;
}



void Reverb :: printConfig()
{
  Serial.print("Reverb Enabled  = "); Serial.println(enabled);
  Serial.print("Reverb Volume   = "); Serial.println(cfg.reverbVolume);
  Serial.print("Reverb Roomsize = "); Serial.println(cfg.reverbRoomsize);
  Serial.print("Reverb Damping  = "); Serial.println(cfg.reverbDamping);
}


// does the actual changes to the audio board
void Reverb :: update()
{
  freeverb1.roomsize(cfg.reverbRoomsize);
  freeverb1.damping(cfg.reverbDamping);
  // enable/disable bypass
  if (enabled)
    enable();
  else
    disable();

  printValue("Reverb Settings Updated");
}



// convert values to slider positions
void Reverb :: convertToSlider()
{
  // convert reverb values 0.0 to 1.0 to slider value 0 to 100.0
  sliderVal[0] = cfg.reverbVolume   * 100.0;
  sliderVal[1] = cfg.reverbRoomsize * 100.0;
  sliderVal[2] = cfg.reverbDamping  * 100.0;
}



// convert slider positions to values
void Reverb :: convertFromSlider()
{
  // convert slider 0 to 100.0 to reverb values 0.0 to 1.0
  cfg.reverbVolume   = sliderVal[0] / 100.0;
  cfg.reverbRoomsize = sliderVal[1] / 100.0;
  cfg.reverbDamping  = sliderVal[2] / 100.0;

  cfg.reverbVolume   = constrain(cfg.reverbVolume,   0, 1.0);
  cfg.reverbRoomsize = constrain(cfg.reverbRoomsize, 0, 1.0);
  cfg.reverbDamping  = constrain(cfg.reverbDamping,  0, 1.0);
}




void Reverb :: process(bool initScreen)
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



// does the actual drawing to the LCD
void Reverb :: drawScreen(bool drawAll)
{
  if (drawAll)
  {
    eraseScreen();

    // add the lables
    drawLabels(numSliders, labelPos, labels);

    // add the title
    drawTitle("Reverb");

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



bool Reverb :: checkEncoders()
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
