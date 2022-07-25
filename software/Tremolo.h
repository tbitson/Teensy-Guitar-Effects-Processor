/**********************************************************
   Tremolo Effect Class

   Interface to the tremolo effect. Provides adjustment for
   speed (frequency), and depth (amplitude) by ajusting a
   sine wave generater's frequncy and alitude and using
   that to control a 'multiply' block that has the dry
   audio passing through which modulates the audio level.

   version 1.0    16Jan2020


   Audio chain:
   I2SIn -> Mixer1 -> Multiply1 -> Mixer8_1 -> I2SOut
                                    ^
   Sine1 -> Mixer2 -----------------|
   DC1   ----^

   adjustments:
      tremoloVolume, float, 0 to 1.0,   <- slider -> mixer1(2, x)
      tremoloSpeed, float, 0.5 to 8.0   <- sine1.frequency();
      tremoloDepth, float, 0 to 1       <- sine1.amplitude();


 * **************************************************************/

#ifndef TREMOLO_H
#define TREMOLO_H


#include "guiItems.h"

extern Encoder paramEncoder;
extern Encoder valueEncoder;



class Tremolo {
  public:
    Tremolo();
    void init();
    void disable();
    void enable();
    void toggle();
    bool getStatus();
    void printConfig();
    void process(bool);
    void setValues(float v, float s, float d);
    void update();
    bool enabled;


  private:
    bool initialScreenDrawn;
    bool selectedItemChanged;
    bool itemValueChanged;
    int8_t selectedItem;
    int8_t lastselectedItem;

    static const uint8_t numSliders = 3;
    float sliderVal[numSliders];
    int16_t sliderXPos[numSliders] = {35, 135, 235};
    int16_t sliderYPos = 2;

    String labels[numSliders] = {"Volume", "Speed", "Depth"};
    int16_t labelPos[numSliders] = {20, 120, 220};

    void convertToSlider();
    void convertFromSlider();
    bool checkEncoders();
    void drawScreen(bool);
};



// this is run before audio board is initialized
Tremolo :: Tremolo()
{
  initialScreenDrawn = false;
  selectedItem = 0;
  lastselectedItem = 0;
  selectedItemChanged = false;
}



// run after audio board is initialized
void Tremolo :: init()
{
  update();
  disable();
  printValue("tremolo initialized");
}




void Tremolo :: disable()
{
  // set volume to 0
  mixer8_1.gain(TREMOLO_IN, 0);

  enabled = false;
  printValue("tremolo disabled");
}



void Tremolo :: enable()
{
  mixer8_1.gain(TREMOLO_IN, cfg.tremoloVolume);
  enabled = true;
  printValue("tremolo enabled");
}


void Tremolo :: toggle()
{
  if (enabled)
    disable();
  else
    enable();
}



bool Tremolo :: getStatus()
{
  return enabled;
}



void Tremolo :: setValues(float v, float s, float d)
{
  cfg.tremoloVolume = v;
  cfg.tremoloSpeed = s;
  cfg.tremoloDepth = d;
}



void Tremolo :: printConfig()
{
  Serial.print("Tremolo Enabled = "); Serial.println(enabled);
  Serial.print("Tremolo Volume  = "); Serial.println(cfg.tremoloVolume);
  Serial.print("Tremolo Speed   = "); Serial.println(cfg.tremoloSpeed);
  Serial.print("Tremolo Depth   = "); Serial.println(cfg.tremoloDepth);
}


// does the actual changes to the audio board
void Tremolo :: update()
{
  sine1.frequency(cfg.tremoloSpeed);
  sine1.amplitude(cfg.tremoloDepth);

  if (enabled)
    enable();
  else
    disable();

  printValue("Tremolo Settings Updated");
}



// convert values to slider positions
void Tremolo :: convertToSlider()
{
  // convert tremolo values 0.0 to 1.0 to slider value 0 to 100.0
  // max speed is 8 hz, 100/8 = 12.5
  sliderVal[0] = cfg.tremoloVolume * 100.0;
  sliderVal[1] = cfg.tremoloSpeed  * 12.5;
  sliderVal[2] = cfg.tremoloDepth  * 100.0;
}



// convert slider positions to values
void Tremolo :: convertFromSlider()
{
  // convert slider 0 to 100.0 to tremolo values 0.0 to 1.0
  cfg.tremoloVolume = sliderVal[0] / 100.0;
  cfg.tremoloSpeed  = sliderVal[1] / 12.5;
  cfg.tremoloDepth  = sliderVal[2] / 100.0;

  cfg.tremoloVolume = constrain(cfg.tremoloVolume, 0.0, 1.0);
  cfg.tremoloSpeed = constrain(cfg.tremoloSpeed, 0.0, 8.0);
  cfg.tremoloDepth = constrain(cfg.tremoloDepth, 0.0, 1.0);
}


// does the actual drawing to the LCD
void Tremolo :: drawScreen(bool drawAll)
{
  if (drawAll)
  {
    eraseScreen();

    // add the lables
    drawLabels(numSliders, labelPos, labels);

    // add the title
    drawTitle("Tremolo");

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



void Tremolo :: process(bool initScreen)
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


bool Tremolo :: checkEncoders()
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
