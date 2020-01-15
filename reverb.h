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
void printReverbConfig();
void convertReverbToSliders();
void convertSlidersToReverb();
void updateReverbSettings();
void drawReverbScreen();
bool checkEncoders2(uint8_t);



// constants
#define NUM_REVERB_ITEMS 3


// global vars
int16_t selectedReverbItem = 0;
int16_t lastselectedReverbItem = 0;
bool selectedReverbItemChanged = false;
bool reverbItemValueChanged = false;
float reverbSliderPos[NUM_REVERB_ITEMS];

boolean reverbActive     = false;





void initReverb()
{
  updateReverbSettings();
  disableReverb();

  Serial.println("Reverb Initialized");
}


void disableReverb()  // bypass reverb
{
  mixer3.gain(WET_NO_REVERB_CH, 1.0);
  mixer3.gain(WET_REVERB, 0);
  reverbActive = false;
  Serial.println("Reverb Disabled");
}


void enableReverb()
{
  mixer3.gain(WET_NO_REVERB_CH, 0);
  mixer3.gain(WET_REVERB, cfg.reverbVolume);
  reverbActive = true;
  Serial.println("Reverb Enabled");
}


void toggleReverb()
{
  if (reverbActive)
    disableReverb();
  else
    enableReverb();
}



void printReverbConfig()
{
  Serial.print(F("Reverb Volume   = "));  Serial.println(cfg.reverbVolume);
  Serial.print(F("Reverb Roomsize = "));  Serial.println(cfg.reverbRoomsize);
  Serial.print(F("Reverb Damping  = "));  Serial.println(cfg.reverbDamping);
}


void updateReverbSettings()
{
  freeverb1.roomsize(cfg.reverbRoomsize);
  freeverb1.damping(cfg.reverbDamping);
  if (reverbActive)
  {
    mixer3.gain(WET_REVERB, cfg.reverbVolume);
    mixer3.gain(WET_NO_REVERB_CH, 0);
  }
  else
  {
    mixer3.gain(WET_REVERB, 0);
    mixer3.gain(WET_NO_REVERB_CH, 1.0);
  }
  Serial.println("Reverb Settings Updated");
}



// main reverb screen loop
void doReverbScreen()
{
  selectedReverbItemChanged = false;
  reverbItemValueChanged = false;

  // set the sliders values to the current stored eq settings
  if (!initialScreenDrawn)
    convertReverbToSliders();

  // read encoders and check for changes
  checkEncoders2(NUM_REVERB_ITEMS);

  // draw the screen
  if (selectedReverbItemChanged || reverbItemValueChanged || !initialScreenDrawn)
  {
    convertSlidersToReverb();
    drawReverbScreen();
    convertReverbToSliders();
    updateReverbSettings();

    lastParamEncVal = paramEncVal;
    lastValEncVal = valEncVal;
  }
}



void convertReverbToSliders() // updatereverbSliderPosues()
{
  // convert reverb values 0.0 to 1.0 to slider value 0 to 100.0
  reverbSliderPos[0] = cfg.reverbVolume   * 100.0;
  reverbSliderPos[1] = cfg.reverbRoomsize * 100.0;
  reverbSliderPos[2] = cfg.reverbDamping  * 100.0;
}




void convertSlidersToReverb() //  updateEqValues()
{
  // convert slider 0 to 100.0 to reverb values 0.0 to 1.0
  cfg.reverbVolume   = reverbSliderPos[0] / 100.0;
  cfg.reverbRoomsize = reverbSliderPos[1] / 100.0;
  cfg.reverbDamping  = reverbSliderPos[2] / 100.0;
}




void drawReverbScreen()
{
  // slider spacing
  uint16_t xInitial = 29;
  uint16_t xSep = 90;

  // slider x positions
  uint16_t sx[NUM_REVERB_ITEMS];
  for (uint8_t i = 0; i < NUM_REVERB_ITEMS; i++)
    sx[i] = i * xSep + xInitial;

  // slider y positions
  uint16_t sy = 0;

  if (!initialScreenDrawn)
  {
    Serial.println("Initializing Reverb Screen");
    selectedReverbItem = 0;

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
  if (!initialScreenDrawn || selectedReverbItemChanged)
  {

    for (uint8_t i = 0; i < NUM_REVERB_ITEMS; i++)
    {
      if (i == selectedReverbItem)
        drawSlider(sx[i], sy, reverbSliderPos[i], true);
      else
        drawSlider(sx[i], sy, reverbSliderPos[i], false);
    }
  }
  // otherwise only draw selected item
  else
    drawSlider(sx[selectedReverbItem], sy, reverbSliderPos[selectedReverbItem], true);

  initialScreenDrawn = true;
}



// this will eventually reside in the gui class file
bool checkEncoders2(uint8_t numItems)
{
  // checks both param & value encoders for changes
  // read the param encoder first, which selects the
  // gui item. If changed, exits with the new
  // "selected item". If no changes, the value encoder
  // is read. Changes to value encode increase or decrease
  // the value of the "selected item"

  paramEncVal = readParamEncoder() / 2;

  if (paramEncVal != lastParamEncVal)
  {
    printValue("paramEncVal", paramEncVal);
    printValue("lastParamEncVal", lastParamEncVal);

    // save the current selected item
    lastselectedReverbItem = selectedReverbItem;

    // and then move to next or previous item, wraps-around
    if (paramEncVal > lastParamEncVal)
      selectedReverbItem++;
    else if (paramEncVal < lastParamEncVal)
      selectedReverbItem--;

    selectedReverbItem = selectedReverbItem % numItems;
    printValue("selectedReverbItem", selectedReverbItem);
    selectedReverbItemChanged = true;
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
        reverbSliderPos[selectedReverbItem] += 4;

      else if (valEncVal < lastValEncVal)
        reverbSliderPos[selectedReverbItem] -= 4;

      // keep it in the slider's range
      reverbSliderPos[selectedReverbItem] = constrain(reverbSliderPos[selectedReverbItem], 0, 100.0);
      reverbItemValueChanged = true;
      printValue("reverbSliderPos[selectedReverbItem]", reverbSliderPos[selectedReverbItem]);
    }
    return true;
  }

  delay(50);
  lastParamEncVal = paramEncVal;
  lastValEncVal = valEncVal;

  // nothing changed
  return false;
}
