/*
    EqScreen.h

    handles the EQ screen drawing and updates
    the EQ settings in the audio chip

    version 2.0.2 Jan2020

*/



#include "guiItems.h"



// prototypes
void initEqualizer();
void calcParametricEQ();
void convertEqToSlider();
void doEqScreen();
void drawEqScreen();
void convertSliderToEq();
bool checkEncoders(uint8_t);



// constants
const uint8_t numEqItems = 5;
// EQ band frequecies
const float eqBandFrequencies[] = { 100.0, 250.0, 600.0, 1300.0, 3000.0 };

int16_t selectedEqItem = 0;
int16_t lastselectedEqItem = 0;
bool selectedEqItemChanged = false;
bool itemValueChanged = false;
float sliderPos[numEqItems];



#ifdef TEST
uint8_t value = 50;
int8_t dir = 1;
#endif






void initEqualizer()
{
  // set up the eq bands and ranges
  audioShield.eqSelect(1); //select parametric EQ
  calcParametricEQ();
  initialScreenDrawn = false;
  
  Serial.println("Equalizer Initialized");
}



void calcParametricEQ()
{
  // calc and update the EQ parameters for the audio chip
  for (uint8_t i = 0; i < numEqItems; i++)
  {
    calcBiquad(FILTER_PARAEQ, eqBandFrequencies[i], cfg.eqBandVals[i], 1, 524288, 44100, cfg.updateFilter);
    audioShield.eqFilter(i + 1, cfg.updateFilter);
  }
}




// main EQ screen loop
void doEqScreen()
{
  selectedEqItemChanged = false;
  itemValueChanged = false;

  lastParamEncVal = paramEncVal;

  // set the sliders values to the current stored eq settings
  if (!initialScreenDrawn)
    convertEqToSlider();

  // read encoders and check for changes
  checkEncoders(numEqItems);

  // draw the screen
  if (selectedEqItemChanged || itemValueChanged || !initialScreenDrawn)
  {
    convertSliderToEq();
    drawEqScreen();
    convertSliderToEq();
    calcParametricEQ();
    initialScreenDrawn = true;

    // reset encoder values for new delta
    lastParamEncVal = paramEncVal;
    lastValEncVal = valEncVal;
  }
}



void convertEqToSlider() // updateSliderValues()
{
  // convert eq settings -1.0 to +1.0 to slider level of 0 to 100%
  for (uint8_t i = 0; i < numEqItems; i++)
  {
    sliderPos[i] = (cfg.eqBandVals[i] + 1.0) * 50;
    printValue("slider pos", i,sliderPos[i]); 
    sliderPos[i] = constrain(sliderPos[i], 0, 100);
  }
}




void convertSliderToEq() //  updateEqValues()
{
  // convert slider position to EQ values
  // sliderPos = 0 to 100%, convert to +/- 1.0

  for (uint8_t i = 0; i < numEqItems; i++)
  {
    cfg.eqBandVals[i] = sliderPos[i] / 50.0 - 1.0;
    printValue("eqBandVals", i,cfg.eqBandVals[i]);
    cfg.eqBandVals[i] = constrain(cfg.eqBandVals[i], -1.0, 1.0);
  }
    
}




void drawEqScreen()
{
  // slider spacing
  uint16_t SliderXSeparation = 61;

  // slider x positions
  uint16_t SliderX[numEqItems];
  for (uint8_t i = 0; i < numEqItems; i++)
    SliderX[i] = i * SliderXSeparation + 21;

  // slider y positions
  uint16_t SliderY = 0;

  if (!initialScreenDrawn)
  {
    Serial.println("Initializing EQ Screen");
    selectedEqItem = 0;

    // reset encoders
    paramEncoder.write(0);
    valueEncoder.write(0);
    lastParamEncVal = 0;
    lastValEncVal = 0;

    // draw screen text
    tft.fillScreen(GUI_FILL_COLOR);
    tft.setTextColor(GUI_ITEM_COLOR);
    tft.setFont(Arial_14);
    tft.setCursor(20, 200);  // 0, 9, 17, 24, 32
    tft.print("100      250     600    1300    3000");
    tft.setCursor(90, 220);
    tft.setTextColor(GUI_TEXT_COLOR);
    tft.print("Parametric EQ");
  }

  // first time we need to draw all items
  if (!initialScreenDrawn || selectedEqItemChanged)
  {
    for (uint8_t i = 0; i < numEqItems; i++)
    {
      if (i == selectedEqItem)
        drawSlider(SliderX[i], SliderY, sliderPos[i], true);
      else
        drawSlider(SliderX[i], SliderY, sliderPos[i], false);
    }
  }
  // otherwise only draw selected item
  else
    drawSlider(SliderX[selectedEqItem], SliderY, sliderPos[selectedEqItem], true);
}



// this will eventually reside in the gui class file
bool checkEncoders(uint8_t numItems)
{
  // checks both param & value encoders for changes
  // read the param encoder first, which selects the
  // gui item. If changed, exits with the new
  // "selected item". If no changes, the value encoder
  // is read. Changes to value encode increase or decrease
  // the value of the "selected item"

  paramEncVal = readParamEncoder();

  if (paramEncVal != lastParamEncVal)
  {
    printValue("paramEncVal", paramEncVal);
    printValue("lastParamEncVal", lastParamEncVal);

    // save the current selected item
    lastselectedEqItem = selectedEqItem;

    // and then move to next or previous item, wraps-around
    if (paramEncVal > lastParamEncVal)
      selectedEqItem++;
    else if (paramEncVal < lastParamEncVal)
      selectedEqItem--;
      
    selectedEqItem = selectedEqItem % numItems;
    printValue("selectedEqItem", selectedEqItem);
    selectedEqItemChanged = true;
    return true;
  }

  else
  {
    // if no param encoder changes, then read Value Encoder
    valEncVal = readValueEncoder() / 2;
    if (valEncVal != lastValEncVal)
    {
      // increase or decrease the slider's new position (need to re-draw later)
      if (valEncVal > lastValEncVal)
        sliderPos[selectedEqItem] += 4;

      else if (valEncVal < lastValEncVal)
        sliderPos[selectedEqItem] -= 4;

      // keep it in the slider's range
      sliderPos[selectedEqItem] = constrain(sliderPos[selectedEqItem], 0, 100.0);
      itemValueChanged = true;
      printValue("sliderPos[selectedEqItem]", sliderPos[selectedEqItem]);
    }
    return true;
  }

  delay(50);
  lastParamEncVal = paramEncVal;
  lastValEncVal = valEncVal;

  // nothing changed
  return false;
}
