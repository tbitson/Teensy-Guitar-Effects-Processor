/**********************************************************
   Compressor Effect Class

   Interface to the audio compressor, built-in to the
   SGTL-5000 audio IC.

   WARNING: The compressor settings are very finicky and
   I have been unable to acheive satisfactory results.
   Future changes expected

   Since this is the first screen that combines buttons and
   sliders, (and I waited to do it last), its a bit diferent
   in format. The logic for radio buttons is fun! Experiment
   to see if this is better

   version 1.0  Jan2020

   sub-classes effect

   adjustments:
       gain: 0 = 0dB, 1 = 6dB, 2 = 12dB
       response (integration time): 0 = 0ms, 1 = 25ms, 2 = 50 ms, 3 = 100 ms (values > 3 permissible)
       hardLimit: 0 = use "soft knee', 1 = hard limit (don't allow values > threshold)
       threshold: 0 to -96 in dBFS
       attack: controls decrease in gain in dB per second  (0 to 1.0 ???)
       decay: how fast gain is restored in dB per second   (0 to 1.0 ???)
       example: audioShield.autoVolumeControl(1, 1, 0, -18, 0.5, 0.5);

      uint16_t autoVolumeControl(uint8_t maxGain, uint8_t lbiResponse, uint8_t hardLimit, float threshold, float attack, float decay);



 * **************************************************************/

#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include "guiItems.h"

extern Encoder paramEncoder;
extern Encoder valueEncoder;


class Compressor {
  public:
    Compressor();
    void init();
    void disable();
    void enable();
    void toggle();
    bool getStatus();
    void printConfig();
    void process(bool);
    void setValues(uint8_t g, uint8_t r, float t, float a, float d);
    void update();
    bool enabled;

  private:
    bool initialScreenDrawn;
    bool selectedItemChanged;
    bool itemValueChanged;
    int8_t selectedItem;
    int8_t lastselectedItem;


    static const uint8_t numItems = 5;      // number of selectable items
    static const uint8_t numSliders = 3;


    // OK, this is a bit of a mess....

    // gain Buttons
    uint8_t numGainButtons = 3;
    int16_t gainButtonsX = 20;
    int16_t gainButtonsY = 40;
    String gainLabels[3] = {"0dB", "6dB", "12dB"};

    // response Buttons
    uint8_t numResponseButtons = 4;
    int16_t responseButtonsX = 80;
    int16_t responseButtonsY = 40;
    String responseLabels[4] = {"0ms", "25ms", "50ms", "100ms"};


    // sliders
    float sliderVal[3];
    int16_t sliderXPos[3] = {145, 205, 265};
    int16_t sliderYPos = 2;

    String labels[numItems] = {"Gain", "Resp", "Thresh", "Attack", "Decay"};
    int16_t labelXPos[numItems] = {5, 63, 128, 193, 254 };


    void convertToSlider();
    void convertFromSlider();
    bool checkEncoders();
    void drawScreen(bool drawAll);
};



// this is run before audio board is initialized
Compressor :: Compressor()
{
  initialScreenDrawn = false;
  selectedItem = 0;
  lastselectedItem = 0;
  selectedItemChanged = false;
}



// run after audio board is initialized
void Compressor :: init()
{
  update();
  disable();
  printValue("Compressor initialized");
}



void Compressor :: disable()
{
  audioShield.autoVolumeDisable();
  enabled = false;
  printValue("Compressor disabled");
}



void Compressor :: enable()
{
  audioShield.autoVolumeEnable();
  enabled = true;
  printValue("Compressor enabled");
}


void Compressor :: toggle()
{
  if (enabled)
    disable();
  else
    enable();
}



bool Compressor :: getStatus()
{
  return enabled;
}


// used from serial terminal or future multiple config settings
void Compressor :: setValues(uint8_t g, uint8_t r, float t, float a, float d)
{
  cfg.compGain      = g;      // 0, 1, 2 = 0, 6, 12db
  cfg.compResponse  = r;
  cfg.compThreshold = t;
  cfg.compAttack    = a;
  cfg.compDecay     = d;
  update();
}


// this does the actual changes to the audio board
void Compressor :: update()
{
  audioShield.autoVolumeControl(cfg.compGain, cfg.compResponse, cfg.compLimit, cfg.compThreshold, cfg.compAttack, cfg.compDecay);
  printValue("Compressor Settings Updated");
}



void Compressor :: printConfig()
{
  Serial.print(F("Comp Enabled   = "));   Serial.println(enabled);
  Serial.print(F("Comp Gain      = "));   Serial.println(cfg.compGain);
  Serial.print(F("Comp Response  = "));   Serial.println(cfg.compResponse);
  Serial.print(F("Comp Limit     = "));   Serial.println(cfg.compLimit);
  Serial.print(F("Comp Threshold = "));   Serial.println(cfg.compThreshold);
  Serial.print(F("Comp Attack    = "));   Serial.println(cfg.compAttack);
  Serial.print(F("Comp Decay     = "));   Serial.println(cfg.compDecay);
}



// convert values to slider positions
void Compressor :: convertToSlider()
{
  // convert Compressor values 0.0 to 1.0 to slider value 0 to 100.0
  sliderVal[0] = cfg.compThreshold * (100.0 / -96.0);     // range is 0 to -96
  sliderVal[1] = cfg.compAttack  * (100.0 / 100.0);       // range is 0 to 100 ?
  sliderVal[2] = cfg.compDecay  * (100.0 / 100.0);        // range is 0 to 100 ?
}





// convert slider positions to values
void Compressor :: convertFromSlider()
{
  // convert slider 0 to 100.0 to Compressor values 0.0 to 1.0
  cfg.compThreshold = sliderVal[0] * (-96.0 / 100.0);
  cfg.compAttack    = sliderVal[1] * 1.0;
  cfg.compDecay     = sliderVal[2] * 1.0;
}




// does the actual drawing to the LCD
void Compressor :: drawScreen(bool drawAll)
{
  if (drawAll)
  {
    eraseScreen();

    // draw separator
    drawVLine(127, 0, 239, ILI9341_RED);

    // add the labels
    drawLabelsSelected(numItems, labelXPos, labels, selectedItem);

    // add the title
    drawTitle("Compressor");

    // draw gain buttons
    drawRadioButtons(gainButtonsX, gainButtonsY, numGainButtons, gainLabels, cfg.compGain);

    // draw response buttons
    drawRadioButtons(responseButtonsX, responseButtonsY, numResponseButtons, responseLabels, cfg.compResponse);

    // draw sliders
    for (uint8_t i = 0; i < numSliders; i++)
      drawSlider(sliderXPos[i], sliderYPos, sliderVal[i], false);

  }
  else
  {
    // only draw selected item

    if (selectedItem == 0)
      drawRadioButtons(gainButtonsX, gainButtonsY, numGainButtons, gainLabels, cfg.compGain);

    else if (selectedItem == 1)
      drawRadioButtons(responseButtonsX, responseButtonsY, numResponseButtons, responseLabels, cfg.compResponse);

    else if (selectedItem >= 2)
    {
      drawSlider(sliderXPos[selectedItem - 2], sliderYPos, sliderVal[selectedItem - 2], true);
    }
  }
}




void Compressor :: process(bool initScreen)
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




bool Compressor :: checkEncoders()
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

    selectedItem = selectedItem % numItems;
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
      {
        switch (selectedItem)
        {
          // gain buttons
          case 0:
            if (cfg.compGain < 2)
              cfg.compGain += 1;
            break;

          // response buttons
          case 1:
            if (cfg.compResponse < 3)
              cfg.compResponse += 1;
            break;

          // threshold
          case 2:
            sliderVal[0] += 4;
            sliderVal[0] = constrain(sliderVal[0], 0, 100.0);
            break;

          // attack
          case 3:
            sliderVal[1] += 4;
            sliderVal[1] = constrain(sliderVal[1], 0, 100.0);
            break;

          // decay
          case 4:
            sliderVal[2] += 4;
            sliderVal[2] = constrain(sliderVal[2], 0, 100.0);
            break;
        }
        itemValueChanged = true;
      }

      else if (valEncVal < lastValEncVal)
      {
        switch (selectedItem)
        {
          // gain buttons
          case 0:
            if (cfg.compGain > 0)
              cfg.compGain -= 1;
            break;

          // response buttons
          case 1:
            if (cfg.compResponse > 0)
              cfg.compResponse -= 1;
            break;

          // threshold
          case 2:
            sliderVal[0] -= 4;
            sliderVal[0] = constrain(sliderVal[0], 0, 100.0);
            break;

          // attack
          case 3:
            sliderVal[1] -= 4;
            sliderVal[1] = constrain(sliderVal[1], 0, 100.0);
            break;

          // decay
          case 4:
            sliderVal[2] -= 4;
            sliderVal[2] = constrain(sliderVal[2], 0, 100.0);
            break;
        }
        itemValueChanged = true;
      }

      return itemValueChanged;
    }
    return false;
  }
}

#endif
