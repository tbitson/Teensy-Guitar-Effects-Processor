/**********************************************************
    Equalizer Effects Class - EQ.h

    handles the EQ screen drawing and updates
    the EQ settings in the audio chip

    version 2.0.0 Jan2020

 ***************************************************************/


#ifndef EQ_H
#define EQ_H

#include "guiItems.h"

extern Encoder paramEncoder;
extern Encoder valueEncoder;


class EQ {
  public:
    EQ();
    void init();
    void disable();
    void enable();
    void toggle();
    bool getStatus();
    void printConfig();
    void process(bool);
    static const uint8_t numEqBands = 5;
    void update();
    bool enabled;

  private:
    bool initialScreenDrawn;
    bool selectedItemChanged;
    bool itemValueChanged;
    uint8_t selectedItem;
    uint8_t lastselectedItem;

    static const uint8_t numSliders = 5;

    float sliderVal[numSliders];
    int16_t sliderXPos[numSliders] = {20, 80, 140, 200, 260};
    int16_t sliderYPos = 2;

    String labels[numSliders] = {"100", "250", "600", "1300", "3000"};
    int16_t labelXPos[numSliders] = {10, 70, 130, 190, 250};

    const float eqBandFrequencies[numEqBands] = { 100.0, 250.0, 600.0, 1300.0, 3000.0 };
    int coeffcients[numEqBands];

    void calcEqCoeffecients();
    void convertToSlider();
    void convertFromSlider();
    bool checkEncoders();
    void drawScreen(bool);
};



// this is run before audio board is initialized
EQ :: EQ()
{
  initialScreenDrawn = false;
  selectedItem = 0;
  lastselectedItem = 0;
  selectedItemChanged = false;
}



// run after audio board is initialized
void EQ :: init()
{
  update();
  disable();
  printValue("EQ initialized");
}




void EQ :: disable()
{
  audioShield.eqSelect(FLAT_FREQUENCY);   // disable equalizer
  enabled = false;
  printValue("EQ disabled");
}



void EQ :: enable()
{
  audioShield.eqSelect(GRAPHIC_EQUALIZER);  // 5-band Graphic Equalizer
  calcEqCoeffecients();
  enabled = true;
  printValue("EQ enabled");
}


void EQ :: toggle()
{
  if (enabled)
    disable();
  else
    enable();
}


bool EQ :: getStatus()
{
  return enabled;
}




void EQ :: calcEqCoeffecients()
{
  // calc and store the EQ parameters for the audio chip
  for (uint8_t i = 0; i < numEqBands; i++)
  {
    calcBiquad(FILTER_PARAEQ, eqBandFrequencies[i], cfg.eqBandVals[i], 1, 524288, 44100,  coeffcients);
    audioShield.eqFilter(i, coeffcients);
  }
}



void EQ :: printConfig()
{
  Serial.print(F("EQ Enabled     = "));   Serial.println(enabled);
  Serial.print(F("EQ Bass        = "));   Serial.println(cfg.eqBandVals[BASS]);
  Serial.print(F("EQ Mid-Bass    = "));   Serial.println(cfg.eqBandVals[MID_BASS]);
  Serial.print(F("EQ Midrange    = "));   Serial.println(cfg.eqBandVals[MIDRANGE]);
  Serial.print(F("EQ Mid-Treble  = "));   Serial.println(cfg.eqBandVals[MID_TREBLE]);
  Serial.print(F("EQ Treble      = "));   Serial.println(cfg.eqBandVals[TREBLE]);
}



// does the actual changes to the audio board
void EQ :: update()
{
  calcEqCoeffecients();
  printValue("EQ Settings Updated");
}



// convert values to slider positions
void EQ :: convertToSlider()
{
  // convert eq settings -1.0 to +1.0 to slider level of 0 to 100%
  for (uint8_t i = 0; i < numSliders; i++)
  {
    sliderVal[i] = (cfg.eqBandVals[i] + 1.0) * 50;
    sliderVal[i] = constrain(sliderVal[i], 0, 100);
  }
}



// convert slider positions to values
void EQ :: convertFromSlider()
{
  // convert slider position to EQ values +/- 1.0
  for (uint8_t i = 0; i < numSliders; i++)
  {
    cfg.eqBandVals[i] = sliderVal[i] / 50.0 - 1.0;
    cfg.eqBandVals[i] = constrain(cfg.eqBandVals[i], -1.0, 1.0);
  }
}




void EQ :: process(bool initScreen)
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
void EQ :: drawScreen(bool drawAll)
{
  if (drawAll)
  {
    eraseScreen();

    // add the lables
    drawLabels(numSliders, labelXPos, labels);

    // add the title
    drawTitle("Parametric Equalizer");

    for (uint8_t i = 0; i < numSliders; i++)
    {
      drawSlider(sliderXPos[i], sliderYPos, sliderVal[i],  i == selectedItem ? true : false);
    }
  }
  else
  {
    // only draw selected item
    drawSlider(sliderXPos[selectedItem], sliderYPos, sliderVal[selectedItem], true);
  }
}


bool EQ :: checkEncoders()
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
    {
      selectedItemChanged = true;

      if (selectedItem < numSliders - 1)
        selectedItem++;
      else
        selectedItem = 0;
    }

    else if (paramEncVal < lastParamEncVal)
    {
      selectedItemChanged = true;

      if (selectedItem == 0)
        selectedItem = numSliders - 1;
      else
        selectedItem--;
    }

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
