/*
    Delayer.h

    handles the Delay screen drawing and updates
    the Delay settings in the audio chip

    version 0.7


*/



#include "guiItems.h"



// prototypes
void initDelayer();
void disableDelayer();
void enableDelayer();
void toggleDelayer();
void printDelayConfig();
void setDelay(uint8_t channel, float timeMS, float volume);
void updateDelays();
void convertDelayToSlider();
void convertSliderToDelay();
void doDelayScreen();
void drawDelayScreen();
bool checkEncoders3(uint8_t numItems);



// constants
// 4 delays, 2 items each
const uint8_t numDelays = 4;
const uint8_t numDelayItems = numDelays * 2;



// global vars
bool delayerActive = false;


int16_t selectedDelayItem = 0;
int16_t lastselectedDelayItem = 0;
bool selectedDelayItemChanged = false;
float delaySliderPos[numDelayItems];






void initDelayer()
{
  updateDelays();
  disableDelayer();
  Serial.println("delayer Initialized");
}


void disableDelayer()
{
  // mixer 1 turns on/off delay effects
  mixer1.gain(DELAY_CH, 0);
  delayerActive = false;
  Serial.println("delayer disabled");
}


void enableDelayer()
{
  mixer1.gain(DELAY_CH, 1.0);
  delayerActive = true;
  Serial.println("delayer enabled");
}


void toggleDelayer()
{
  if (delayerActive)
    disableDelayer();
  else
    enableDelayer();
}


// does the actual changes to the audio board
void updateDelays()
{
  for (uint8_t i = 0; i < numDelays; i++)
  {
    delay1.delay(i, cfg.delayTimes[i]);
    mixer4.gain(i, cfg.delayVols[i]);
  }
  printValue("Updated delays");
}


// dev use, probably discard
void setDelay(uint8_t channel, float timeMS, float volume)
{
  if (channel < numDelays)
  {
    timeMS = constrain(timeMS, 0, 1000);
    volume = constrain(volume, 0, 1.0);

    cfg.delayTimes[channel] = timeMS;
    cfg.delayVols[channel] = volume;
  }

  delay1.delay(channel, cfg.delayTimes[channel]);
  mixer4.gain(channel,  cfg.delayVols[channel]);
}


void printDelayConfig()
{
  for (uint8_t i = 0; i < numDelays; i++)
  {
    Serial.print("Delay time "); Serial.print(i); Serial.print(" = "); Serial.print(cfg.delayTimes[i]);
    Serial.print(" vol = "); Serial.println(cfg.delayVols[i]);
  }
}



// main Delay screen loop
void doDelayScreen()
{
  selectedDelayItemChanged = false;
  itemValueChanged = false;

  lastParamEncVal = paramEncVal;

  // set the sliders values to the current stored Delay settings
  if (!initialScreenDrawn)
    convertDelayToSlider();

  // read encoders and check for changes
  checkEncoders3(numDelayItems);

  // draw the screen
  if (selectedDelayItemChanged || itemValueChanged || !initialScreenDrawn)
  {
    convertSliderToDelay();
    drawDelayScreen();
    convertSliderToDelay();
    updateDelays();
    initialScreenDrawn = true;

    // reset encoder values for new delta
    lastParamEncVal = paramEncVal;
    lastValEncVal = valEncVal;
  }
}


// display the 4 delay sliders followed by 4 volume sliders
void convertDelayToSlider() // updateSliderValues()
{
  // convert Delay settings, delay 100% = 1000ms, volume 1.0 = 100%
  for (uint8_t i = 0; i < 4; i++)
  {
    // delay
    delaySliderPos[i] = cfg.delayTimes[i] / 10;   // 1000ms = 100%
    printValue("slider pos", i, delaySliderPos[i]);
    delaySliderPos[i] = constrain(delaySliderPos[i], 0, 100);

    // volume
    delaySliderPos[i + 4] = cfg.delayVols[i] * 100;
    printValue("slider pos", i + 4, delaySliderPos[i + 4]);
    delaySliderPos[i + 4] = constrain(delaySliderPos[i + 4], 0, 100);
  }
}




void convertSliderToDelay() //  updateDelayValues()
{
  for (uint8_t i = 0; i < 4; i++)
  {
    cfg.delayTimes[i] = delaySliderPos[i] * 10;  // 100% = 1000ms
    printValue("delay time", i, cfg.delayTimes[i]);
    cfg.delayTimes[i] = constrain(cfg.delayTimes[i], 0, 1000);

    cfg.delayVols[i] = delaySliderPos[i + 4] / 100.0; // 100% = 1.0
    printValue("delay vol", i, cfg.delayVols[i]);
    cfg.delayVols[i] = constrain(cfg.delayVols[i], 0, 1.0);
  }
}




void drawDelayScreen()
{
  // slider spacing
  uint16_t SliderXSeparation = 40;

  // slider x positions
  uint16_t SliderX[numDelayItems];
  for (uint8_t i = 0; i < numDelayItems; i++)
    SliderX[i] = i * SliderXSeparation + 5;

  // slider y positions
  uint16_t SliderY = 0;

  if (!initialScreenDrawn)
  {
    Serial.println("Initializing Delay Screen");
    selectedDelayItem = 0;

    // reset encoders
    paramEncoder.write(0);
    valueEncoder.write(0);
    lastParamEncVal = 0;
    lastValEncVal = 0;

    // draw screen text
    tft.fillScreen(GUI_FILL_COLOR);
    tft.setTextColor(GUI_ITEM_COLOR);
    tft.setFont(Arial_9);
    tft.setCursor(5, 200);
    tft.print("DLY1  DLY2  DLY3  DLY4  VOL1  VOL2  VOL2  VOL4");

    tft.setFont(Arial_16);
    tft.setCursor(85, 220);
    tft.setTextColor(GUI_TEXT_COLOR);
    tft.print("4 Channel Delay");
    tft.drawFastVLine(130, 0, 220, GUI_ITEM_COLOR);
  }

  // first time we need to draw all items
  if (!initialScreenDrawn || selectedDelayItemChanged)
  {
    for (uint8_t i = 0; i < numDelayItems; i++)
    {
      if (i == selectedDelayItem)
        drawSlider(SliderX[i], SliderY, delaySliderPos[i], true);
      else
        drawSlider(SliderX[i], SliderY, delaySliderPos[i], false);
    }
  }
  // otherwise only draw selected item
  else
    drawSlider(SliderX[selectedDelayItem], SliderY, delaySliderPos[selectedDelayItem], true);
}



// this will eventually reside in the gui class file
bool checkEncoders3(uint8_t numItems)
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
    lastselectedDelayItem = selectedDelayItem;

    // and then move to next or previous item, wraps-around
    if (paramEncVal > lastParamEncVal)
      selectedDelayItem++;
    else if (paramEncVal < lastParamEncVal)
      selectedDelayItem--;

    selectedDelayItem = selectedDelayItem % numItems;
    printValue("selectedDelayItem", selectedDelayItem);
    selectedDelayItemChanged = true;
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
        delaySliderPos[selectedDelayItem] += 4;

      else if (valEncVal < lastValEncVal)
        delaySliderPos[selectedDelayItem] -= 4;

      // keep it in the slider's range
      delaySliderPos[selectedDelayItem] = constrain(delaySliderPos[selectedDelayItem], 0, 100.0);
      itemValueChanged = true;
      printValue("delaySliderPos[selectedDelayItem]", delaySliderPos[selectedDelayItem]);
    }
    return true;
  }

  delay(50);
  lastParamEncVal = paramEncVal;
  lastValEncVal = valEncVal;

  // nothing changed
  return false;
}
