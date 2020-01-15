/*
    Delayer.h

    handles the Delay screen drawing and updates
    the Delay settings in the audio chip

    version 0.6

*/



#include "guiItems.h"



// prototypes
void initDelayer();
void disableDelayer();
void enableDelayer();
void updateDelayer();
void setDelay(uint8_t channel, float timeMS, float volume);
void showDelays();


//void convertDelayToSlider();
//void doDelayScreen();
//void drawDelayScreen();
//void convertSliderToDelay();
//bool checkEncoders3(uint8_t);



// constants
const uint8_t numDelayItems = 8;



// global vars
bool delayerEnabled = false;


//int16_t selectedDelayItem = 0;
//int16_t lastselectedDelayItem = 0;
//bool selectedDelayItemChanged = false;
////bool itemValueChanged = false;
//float delaySliderPos[numDelayItems];



// NOT IN config.h YET, delay config not saved
float delayTimes[4];
float delayVols[4];




void initDelayer()
{
  for (uint8_t i = 0; i < 4; i++)
  {
    delayTimes[i] = (float)i * 10.0;
    delayVols[i]  = 0;
  }

  updateDelayer();
  // initialScreenDrawn = false;
  Serial.println("delayer Initialized");
  disableDelayer();
}


void disableDelayer()
{
  mixer3.gain(3, 0);
  delayerEnabled = false;
  Serial.println("delayer disabled");
}


void enableDelayer()
{
  mixer3.gain(3, 1.0);
  delayerEnabled = true;
  Serial.println("delayer enabled");
}


void updateDelayer()
{
  delay1.delay(0, delayTimes[0]);
  delay1.delay(1, delayTimes[1]);
  delay1.delay(2, delayTimes[2]);
  delay1.delay(3, delayTimes[3]);

  mixer4.gain(0, delayVols[0]);
  mixer4.gain(1, delayVols[1]);
  mixer4.gain(2, delayVols[2]);
  mixer4.gain(3, delayVols[3]);
}



void setDelay(uint8_t channel, float timeMS, float volume)
{
  if (channel < 4)
  {
    timeMS = constrain(timeMS, 0, 1000);
    volume = constrain(volume, 0, 1.0);

    delayTimes[channel] = timeMS;
    delayVols[channel] = volume;
  }

  updateDelayer();
}


void showDelays()
{
  for (uint8_t i = 0; i < 4; i++)
  {
    Serial.print("Delay time ");
    Serial.print(i);
    Serial.print(" = ");
    Serial.print(delayTimes[i]);
    Serial.print(" vol = ");
    Serial.println(delayVols[i]);
  }
}

/*


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
    initialScreenDrawn = true;

    // reset encoder values for new delta
    lastParamEncVal = paramEncVal;
    lastValEncVal = valEncVal;
  }
  }



  void convertDelayToSlider() // updateSliderValues()
  {
  // convert Delay settings -1.0 to +1.0 to slider level of 0 to 100%
  for (uint8_t i = 0; i < numDelayItems; i++)
  {
    sliderPos[i] = (cfg.DelayBandVals[i] + 1.0) * 50;
    printValue("slider pos", i, sliderPos[i]);
    sliderPos[i] = constrain(sliderPos[i], 0, 100);
  }
  }




  void convertSliderToDelay() //  updateDelayValues()
  {
  // convert slider position to Delay values
  // sliderPos = 0 to 100%, convert to +/- 1.0

  for (uint8_t i = 0; i < numDelayItems; i++)
  {
    cfg.DelayBandVals[i] = sliderPos[i] / 50.0 - 1.0;
    printValue("slider pos", i, sliderPos[i]);
    cfg.DelayBandVals[i] = constrain(cfg.DelayBandVals[i], -1.0, 1.0);
  }

  }




  void drawDelayScreen()
  {
  // slider spacing
  uint16_t SliderXSeparation = 40;

  // slider x positions
  uint16_t SliderX[numDelayItems];
  for (uint8_t i = 0; i < numDelayItems; i++)
    SliderX[i] = i * SliderXSeparation + 10;

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
    tft.setFont(Arial_14);
    tft.setCursor(20, 200);  // 0, 9, 17, 24, 32
    tft.print("DLY1 Vol1 DLY2 Vol2 DLY3 Vol3 DLY4 Vol4");
    tft.setCursor(90, 220);
    tft.setTextColor(GUI_TEXT_COLOR);
    tft.print("4 Channel Delay");
  }

  // first time we need to draw all items
  if (!initialScreenDrawn || selectedDelayItemChanged)
  {
    for (uint8_t i = 0; i < numDelayItems; i++)
    {
      if (i == selectedDelayItem)
        drawSlider(SliderX[i], SliderY, sliderPos[i], true);
      else
        drawSlider(SliderX[i], SliderY, sliderPos[i], false);
    }
  }
  // otherwise only draw selected item
  else
    drawSlider(SliderX[selectedDelayItem], SliderY, sliderPos[selectedDelayItem], true);
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

  paramEncVal = readParamEncoder();

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
        sliderPos[selectedDelayItem] += 4;

      else if (valEncVal < lastValEncVal)
        sliderPos[selectedDelayItem] -= 4;

      // keep it in the slider's range
      sliderPos[selectedDelayItem] = constrain(sliderPos[selectedDelayItem], 0, 100.0);
      itemValueChanged = true;
      printValue("sliderPos[selectedDelayItem]", sliderPos[selectedDelayItem]);
    }
    return true;
  }

  delay(50);
  lastParamEncVal = paramEncVal;
  lastValEncVal = valEncVal;

  // nothing changed
  return false;
  }

*/
