/**********************************************************
   Chorus Effect Class
   Interface for the Chorus Effect.
   Provides adjustments to the number of voices &
   level of the chorus effect

    Version 1.1     08Aug2020



   Audio chain:
   I2SIn -> Mixer5 -> Effects -> chorus-> mixer8-1 -> I2SOut

 * **************************************************************/

#ifndef CHORUS_H
#define CHORUS_H


#include "guiItems.h"

extern Encoder paramEncoder;
extern Encoder valueEncoder;

#define CHORUS_DELAY_LENGTH (16 * AUDIO_BLOCK_SAMPLES)


class Chorus {
  public:
    Chorus();
    void init();
    void disable();
    void enable();
    void toggle();
    bool getStatus();
    void printConfig();
    void process(bool);
    void setValues(uint8_t voices, float volume);
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

    String labels[numSliders] = {"Voices", "Volume"};
    int16_t labelPos[numSliders] = {75, 195};

    // default chorus timing & voices
    short delayline[CHORUS_DELAY_LENGTH];

    void convertToSlider();
    void convertFromSlider();
    void drawScreen(bool);
    bool checkEncoders();
};




// this is run before audio board is initialized
Chorus :: Chorus()
{
  initialScreenDrawn = false;
  selectedItem = 0;
  lastselectedItem = 0;
  selectedItemChanged = false;
}



// run after audio board is initialized
void Chorus :: init()
{
  chorus1.voices(cfg.chorusVoices);
  update();
  disable();
  printValue("chorus initialized");
}




void Chorus :: disable()
{
  // set volume to 0
  mixer8_1.gain(CHORUS_IN, 0);
  enabled = false;
  printValue("chorus disabled");
}



void Chorus :: enable()
{
  // enable chrous mixer channel
  mixer8_1.gain(CHORUS_IN, cfg.chorusVolume);
  enabled = true;
  printValue("chorus enabled");
}


void Chorus :: toggle()
{
  if (enabled)
    disable();
  else
    enable();
}




bool Chorus :: getStatus()
{
  return enabled;
}



void Chorus :: setValues(uint8_t voices, float volume)
{
  cfg.chorusVoices = voices;
  cfg.chorusVolume = volume;
}



void Chorus :: printConfig()
{
  Serial.print("Chorus Enabled = "); Serial.println(enabled);
  Serial.print("Chorus Voices   = "); Serial.println(cfg.chorusVoices);
  Serial.print("Chorus Volume   = "); Serial.println(cfg.chorusVolume);
}


// does the actual changes to the audio board
void Chorus :: update()
{
  chorus1.voices(cfg.chorusVoices);

  if (enabled)
    enable();
  else
    disable();
  printValue("Chorus Settings Updated");
}



// convert values to slider positions
void Chorus :: convertToSlider()
{
  // speed is a float 0.0 to max speed of 8 hz (at these settings)
  // depth is an int16, 0 to 10 or more depending on memory allocation
  sliderVal[0] = cfg.chorusVoices * 20.0;     // max 5 voices
  sliderVal[1] = cfg.chorusVolume * 100.0;

}



// convert slider positions to values
void Chorus :: convertFromSlider()
{
  // convert slider 0 to 100.0 to flanger values 0.0 to 1.0
  cfg.chorusVoices = sliderVal[0] / 20.0;
  cfg.chorusVolume = sliderVal[1] / 100.0;

  cfg.chorusVoices = constrain(cfg.chorusVoices, 0, 5);
  cfg.chorusVolume = constrain(cfg.chorusVolume, 0, 1.0);
}



// does the actual drawing to the LCD
void Chorus :: drawScreen(bool drawAll)
{
  if (drawAll)
  {
    Serial.println("Drawing Chorus Screen");
    eraseScreen();

    // add the lables
    drawLabels(numSliders, labelPos, labels);

    // add the title
    drawTitle("Chorus");

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



void Chorus :: process(bool initScreen)
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



bool Chorus :: checkEncoders()
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
