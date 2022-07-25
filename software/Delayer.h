/***************************************************
    Delayer Effect Class

    version external 1.2

   Adds 2 delays each with its own volume and delay time
   Max delay is about 800ms with internal memory on a Teensy
   3.6, or up to 1.5 seconds with optional external SRAM

***************************************************/



#ifndef DELAYER_H
#define DELAYER_H

extern Encoder paramEncoder;
extern Encoder valueEncoder;


class Delayer {
  public:
    Delayer();
    void init();
    void disable();
    void enable();
    void toggle();
    void setRecirculate(float);
    float getRecirculate();
    bool getStatus();
    void printConfig();
    void process(bool);
    void update();
    bool enabled;

  private:
    bool initialScreenDrawn;
    bool selectedItemChanged;
    bool itemValueChanged;
    int8_t selectedItem;
    int8_t lastselectedItem;

    static const uint8_t numDelays = 2;
    static const uint8_t numSliders = 5;

    float sliderVal[numSliders];
    int16_t sliderXPos[numSliders] = {5, 55, 125, 175, 245};
    int16_t sliderYPos = 2;

    String labels[numSliders] = {"D1", "D2", "V1", "V2", "Recirc"};
    int16_t labelPos[numSliders] = {5, 55, 125, 175, 240};

    void convertToSlider();
    void convertFromSlider();
    void drawScreen(bool);
    bool checkEncoders();
};



// run before audio board is initialized
Delayer :: Delayer()
{
  initialScreenDrawn = false;
  selectedItem = 0;
  lastselectedItem = 0;
  selectedItemChanged = false;
}



// run after audio board is initialized
void Delayer :: init()
{
  update();
  disable();

  // future use
  delayExt1.disable(2);
  delayExt1.disable(3);

  printValue("Delayer Initalized");
}


void Delayer :: disable()
{
  mixer8_1.gain(DELAY_IN, 0);
  setRecirculate(0);
  enabled = false;
  printValue("delayer disabled");
}


void Delayer :: enable()
{
  mixer8_1.gain(DELAY_IN, 1.0);
  enabled = true;
  setRecirculate(cfg.recirculate);
  printValue("delayer enabled");
}


void Delayer :: toggle()
{
  if (enabled)
    disable();
  else
    enable();
}



void Delayer :: setRecirculate(float value)
{
  cfg.recirculate = constrain(value, 0, 1.0);
  mixer5.gain(DELAY_FEEDBACK, cfg.recirculate);
}



float Delayer :: getRecirculate()
{
  return cfg.recirculate;
}



bool Delayer :: getStatus()
{
  return enabled;
}


void Delayer :: printConfig()
{
  Serial.print("Delayer Enabled = "); Serial.println(enabled);
  for (uint8_t i = 0; i < numDelays; i++)
  {
    Serial.print("Delayer Delay[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.print(cfg.delayTimes[i]);
    Serial.print("  Vol = ");
    Serial.println(cfg.delayVols[i]);
  }

  Serial.print("Recirculate Level = ");
  Serial.println(cfg.recirculate);
}


// does the actual changes to the audio board
void Delayer :: update()
{
  for (uint8_t i = 0; i < numDelays; i++)
  {
    // set the delay time
    if (cfg.delayTimes[i] > 0)
      delayExt1.delay(i, cfg.delayTimes[i]);
    else
      delayExt1.disable(i);

    // set the delay volume
    mixer3.gain(i, cfg.delayVols[i]);
  }

  setRecirculate(cfg.recirculate);
  printValue("Updated delays");
}


// convert delay values to slider positions
void Delayer :: convertToSlider()
{
  // convert Delay settings, delay 100% = 1000ms, volume 1.0 = 100%
  for (uint8_t i = 0; i < numDelays; i++)
  {
    // delay
    sliderVal[i] = cfg.delayTimes[i] / 10.0;   // 1000ms = 100%
    sliderVal[i] = constrain(sliderVal[i], 0, 100);

    // volume
    sliderVal[i + numDelays] = cfg.delayVols[i] * 100.0;
    sliderVal[i + numDelays] = constrain(sliderVal[i + numDelays], 0, 100);
  }

  // recirculation
  sliderVal[numSliders - 1] = cfg.recirculate * 100.0;
  sliderVal[numSliders - 1] = constrain(sliderVal[numSliders - 1], 0, 100);
}


// convert slider positions to delay values
void Delayer :: convertFromSlider()
{
  for (uint8_t i = 0; i < numDelays ; i++)
  {
    cfg.delayTimes[i] = sliderVal[i] * 10.0;  // 100% = 1000ms
    cfg.delayTimes[i] = constrain(cfg.delayTimes[i], 0, 1000);
    printValue("delay time", i, cfg.delayTimes[i]);

    cfg.delayVols[i] = sliderVal[i + numDelays] / 100.0; // 100% = 1.0
    cfg.delayVols[i] = constrain(cfg.delayVols[i], 0, 1.0);
    printValue("delay vol", i, cfg.delayVols[i]);
  }

  cfg.recirculate = sliderVal[numSliders - 1] / 100.0;
}




// does the actual drawing to the LCD
void Delayer :: drawScreen(bool drawAll)
{
  if (drawAll)
  {
    eraseScreen();

    // add the lables
    drawLabels(numSliders, labelPos, labels);

    // add the title
    drawTitle("Delayer");

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



void Delayer :: process(bool initScreen)
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



bool Delayer :: checkEncoders()
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
