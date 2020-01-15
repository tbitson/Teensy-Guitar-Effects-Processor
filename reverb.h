/*
    reverbScreen.h

   handles the freeverb screen drawing and updates
   the reverb settings

   version 2.0  Dec 2019

*/



#include "guiItems.h"



// prototypes
void initReverb();
void disableReverb();
void enableReverb();
void convertReverbToSliders();
void convertSlidersToReverb();
void updateReverbSettings();
void drawReverbScreen();
bool checkEncoders2(uint8_t);



// constants
#define NUM_GUI_ITEMS 3



// global vars
int16_t selectedUiItem = 0;
int16_t lastselectedUiItem = 0;
bool selectedUiItemChanged = false;
bool UiItemValueChanged = false;
float sliderVal[NUM_GUI_ITEMS];

boolean reverbActive     = false;





void initReverb()
{
  updateReverbSettings();
  disableReverb();
  
  Serial.println("Reverb Initialized");
}


void disableReverb()
{
  mixer1.gain(REVERB, 0);
  reverbActive = false;
  Serial.println("Reverb Disabled");
}


void enableReverb()
{
  mixer1.gain(REVERB, cfg.reverbVolume);
  reverbActive = true;
  Serial.println("Reverb Enabled");
}



void updateReverbSettings()
{
  freeverb1.roomsize(cfg.reverbRoomsize);
  freeverb1.damping(cfg.reverbDamping);
  if (reverbActive)
    mixer1.gain(REVERB, cfg.reverbVolume);
  else
    mixer1.gain(REVERB, 0);

  Serial.println("Reverb Settings Updated");
}



// main reverb screen loop
void doReverbScreen()
{
  selectedUiItemChanged = false;
  UiItemValueChanged = false;

  // set the sliders values to the current stored eq settings
  if (!initialScreenDrawn)
    convertReverbToSliders();

  // read encoders and check for changes
  checkEncoders(NUM_GUI_ITEMS);

  // draw the screen
  if (selectedUiItemChanged || UiItemValueChanged || !initialScreenDrawn)
  {
    convertSlidersToReverb();
    drawReverbScreen();
    convertReverbToSliders();
    updateReverbSettings();

    lastParamEncVal = paramEncVal;
    lastValEncVal = valEncVal;
  }
}



void convertReverbToSliders() // updateSliderValues()
{
  // convert reverb values 0.0 to 1.0 to slider value 0 to 100.0
  sliderVal[0] = cfg.reverbVolume   * 100.0;
  sliderVal[1] = cfg.reverbRoomsize * 100.0;
  sliderVal[2] = cfg.reverbDamping  * 100.0;
}




void convertSlidersToReverb() //  updateEqValues()
{
  // convert slider 0 to 100.0 to reverb values 0.0 to 1.0
  cfg.reverbVolume   = sliderVal[0] / 100.0;
  cfg.reverbRoomsize = sliderVal[1] / 100.0;
  cfg.reverbDamping  = sliderVal[2] / 100.0;
}




void drawReverbScreen()
{
  // slider spacing
  uint16_t xInitial = 29;
  uint16_t xSep = 90;

  // slider x positions
  uint16_t sx[NUM_GUI_ITEMS];
  for (uint8_t i = 0; i < NUM_GUI_ITEMS; i++)
    sx[i] = i * xSep + xInitial;

  // slider y positions
  uint16_t sy = 0;

  if (!initialScreenDrawn)
  {
    Serial.println("Initializing Reverb Screen");
    selectedUiItem = 0;

    // reset encoders
    paramEncoder.write(0);
    valueEncoder.write(0);
    lastParamEncVal = 0;
    lastValEncVal = 0;

    // draw screen text
    tft.fillScreen(GUI_FILL_COLOR);
    tft.setTextColor(GUI_ITEM_COLOR);
    tft.setFont(Arial_14);
    tft.setCursor(20, 200);
    tft.print("Volume  RoomSize   Damping");
    tft.setCursor(120, 220);
    tft.setTextColor(GUI_TEXT_COLOR);
    tft.print("Freeverb");
  }


  // first time we need to draw all items
  if (!initialScreenDrawn || selectedUiItemChanged)
  {

    for (uint8_t i = 0; i < NUM_GUI_ITEMS; i++)
    {
      if (i == selectedUiItem)
        drawSlider(sx[i], sy, sliderVal[i], true);
      else
        drawSlider(sx[i], sy, sliderVal[i], false);
    }
  }
  // otherwise only draw selected item
  else
    drawSlider(sx[selectedUiItem], sy, sliderVal[selectedUiItem], true);

  initialScreenDrawn = true;
}




bool checkEncoders2(uint8_t numItems)
{
  // checks both param & value encoders for changes
  // read the param encoder first, which selects the
  // gui item. If changed, exits with the new
  // "selected item". If no changes, the value encoder
  // is read. Changes to value encode increase or decrease
  // the value of the "selected item"

  // store the current value as baseline and read Parameter Encoder
  lastParamEncVal = paramEncVal;
  paramEncVal = readParamEncoder();

  if (paramEncVal != lastParamEncVal)
  {
    printValue("paramEncVal", paramEncVal);
    printValue("lastParamEncVal", lastParamEncVal);

    // save the current selected item
    lastselectedUiItem = selectedUiItem;

    // and then move to next or previous item
    if (paramEncVal > lastParamEncVal)
      selectedUiItem++;
    else if (paramEncVal < lastParamEncVal)
      selectedUiItem--;

    // make sure we stay in bounds
    selectedUiItem = constrain(selectedUiItem, 0, numItems - 1);
    printValue("selectedUiItem", selectedUiItem);

    selectedUiItemChanged = true;
    return true;
  }

  else

  {
    // if no param encoder changes, then read Value Encoder
    lastValEncVal = valEncVal;
    valEncVal = readValueEncoder();

    if (valEncVal != lastValEncVal)
    {
      printValue("valEncVal", valEncVal);
      printValue("lastValEncVal", lastValEncVal);

      // increase or decrease the slider's new position (need to re-draw later)
      if (valEncVal > lastValEncVal)
        sliderVal[selectedUiItem] += 8;

      else if (valEncVal < lastValEncVal)
        sliderVal[selectedUiItem] -= 8;

      // keep it in the slider's range
      sliderVal[selectedUiItem] = constrain(sliderVal[selectedUiItem], 0, 100.0);
      printValue("sliderVal[selectedUiItem]", sliderVal[selectedUiItem]);
      UiItemValueChanged = true;
    }
    return true;
  }

  // nothing changed
  return false;
}
