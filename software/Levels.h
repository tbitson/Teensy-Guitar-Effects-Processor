/**********************************************************
   Input/Output Level Class

  Version 2.0     18Jan2020


   While the input/out level indicator screen isn't really
   an effect, it uses the same format and accesses the TAL.

   This class provides an input level adjust with input and
   output 'VU' style bargraphs. The color varies with signal,
   and red indicates saturation (which sounds really bad on
   a DSP based system).

   Ref values:
   Input Level Adj: 16 levels with 0 being max;
          0: 3.12 Volts p-p      8: 0.79 Volts p-p
          1: 2.63 Volts p-p      9: 0.67 Volts p-p
          2: 2.22 Volts p-p      10: 0.56 Volts p-p
          3: 1.87 Volts p-p      11: 0.48 Volts p-p
          4: 1.58 Volts p-p      12: 0.40 Volts p-p
  default 5: 1.33 Volts p-p      13: 0.34 Volts p-p
          6: 1.11 Volts p-p      14: 0.29 Volts p-p
          7: 0.94 Volts p-p      15: 0.24 Volts p-p

    Reference: Log(1.0 + f) results:
          f = 0.00   log = 0.00     f = 1.00   log = 0.69
          f = 0.10   log = 0.10     f = 1.10   log = 0.74
          f = 0.20   log = 0.18     f = 1.20   log = 0.79
          f = 0.30   log = 0.26     f = 1.30   log = 0.83
          f = 0.40   log = 0.34     f = 1.40   log = 0.88
          f = 0.50   log = 0.41     f = 1.50   log = 0.92
          f = 0.60   log = 0.47     f = 1.60   log = 0.96
          f = 0.70   log = 0.53     f = 1.70   log = 0.99
          f = 0.80   log = 0.59     f = 1.80   log = 1.03
          f = 0.90   log = 0.64     f = 1.90   log = 1.06


  TAL function:
  bool lineInLevel(uint8_t left, [uint8_t right]);

 ****************************************************************/
#ifndef LEVELS_H
#define LEVELS_H


#include "guiItems.h"
#include <math.h>

extern Encoder paramEncoder;
extern Encoder valueEncoder;


class Levels {
  public:
    Levels();
    void init();
    void printConfig();
    void process(bool);
    void update();
    bool enabled;


  private:
    void convertToSlider();
    void convertFromSlider();
    bool checkValEncoder();
    float calcEqAdjustment();
    void drawScreen(bool);
    void drawVU(int16_t, int16_t, float);

    bool selectedItemChanged;
    bool itemValueChanged;
    uint8_t selectedItem;
    uint8_t lastselectedItem;

    static const uint8_t numSliders = 1;
    static const uint8_t numLabels = 3;
    static const uint8_t numVUs = 2;

    float level;
    float sliderVal;
    int16_t sliderXVal = 40;
    int16_t sliderYVal = 0;

    int16_t vuPosX[2] = {130, 220};
    int16_t vuPosY[2] = {3, 3};

    String labels[3] = {"Input Adj", "Input", "Output"};
    int16_t labelPos[3] = {20, 120, 200};

    float inputPeak;
    float outputPeak;
};



// this is run before audio board is initialized
Levels :: Levels()
{
  selectedItem = 0;
  lastselectedItem = 0;
  selectedItemChanged = false;
  itemValueChanged = false;
}


void Levels :: init()
{
  // set input level to config value
  update();
  printValue("Levels Initialized");
}


void Levels :: printConfig()
{
  Serial.print("Input Level    = ");
  Serial.println(cfg.inputLevel);
}



void Levels :: convertToSlider()
{
  // convert input level values 0 to 15
  // converts to 100% to 0 (inverted, see above
  sliderVal = (float)cfg.inputLevel * (100.0 / 15.0);
}


void Levels :: convertFromSlider()
{
  cfg.inputLevel = (uint8_t)(sliderVal * 15.0 / 100.0);
  cfg.inputLevel = constrain(cfg.inputLevel, 0, 15);
}



float Levels :: calcEqAdjustment()
{
  float eqAdjust = 0;

  // find the largest eq value adjustment
  for (uint8_t i = 0; i < NUM_EQ_BANDS; i++)
  {
    if (cfg.eqBandVals[i] > eqAdjust)
      eqAdjust = cfg.eqBandVals[i];
  }

  eqAdjust = exp10(eqAdjust / 20.0);
  return eqAdjust;
}



void Levels :: process(bool initScreen)
{
  itemValueChanged = false;
  // first time thru, set the slider to stored value
  if (initScreen)
    convertToSlider();

  // read encoders and check for changes
  checkValEncoder();
  if (itemValueChanged)
  {
    convertFromSlider();
    update();
  }

  // unlike other screens, this screen must be conmtinously refreshed
  drawScreen(initScreen);

  // reset encoder values for new delta
  lastParamEncVal = paramEncVal;
  lastValEncVal = valEncVal;
}



// since there's only one slider we don't have to read
// the param encoder
bool Levels :: checkValEncoder()
{
  valEncVal = readValueEncoder() / 2;
  if (valEncVal != lastValEncVal)
  {
    // increase or decrease the slider's new position (need to re-draw later)
    if (valEncVal > lastValEncVal)
      sliderVal += 4;

    else if (valEncVal < lastValEncVal)
      sliderVal -= 4;

    // keep it in the slider's range
    sliderVal = constrain(sliderVal, 0, 100.0);
    itemValueChanged = true;

    printValue("sliderVal", sliderVal);
    return itemValueChanged;
  }
  return false;
}



// does the actual changes to the audio board
void Levels :: update()
{
  audioShield.lineInLevel(cfg.inputLevel);
  printValue("lineInLevel", cfg.inputLevel);
}




void Levels :: drawScreen(bool drawAll)
{
  float vuLevel = 0;
  float eqAdjust = 0;


  if (drawAll)
  {
    eraseScreen();

    // add the lables
    drawLabels(numLabels, labelPos, labels);

    // add the title
    drawTitle("Audio Levels");
  }

  if (drawAll || itemValueChanged)
  {
    // draw single slider
    drawSlider(sliderXVal, sliderYVal, sliderVal, true);
  }

  // calc effect of EQ gains
  eqAdjust = calcEqAdjustment();

  // input level
  vuLevel = 0;
  if (peak1.available())
    vuLevel = peak1.readPeakToPeak();

  // factor in the gain from the EQ since it can affect clipping in
  // downstream audio processing
  vuLevel = vuLevel * eqAdjust;

  // peak-hold display, retains 92% of previous level
  if (vuLevel > inputPeak)
    inputPeak = vuLevel;
  else
    inputPeak *= 0.92;

  drawVU(vuPosX[0], vuPosY[0], inputPeak);


  // output level
  vuLevel = 0;
  if (peak2.available())
    vuLevel = peak2.readPeakToPeak();

  // factor in the max gain from the EQ since they're not represented in the input
  vuLevel = vuLevel * eqAdjust;

  // peak-hold display, retains 92% of previous level
  if (vuLevel > outputPeak)
    outputPeak = vuLevel;
  else
    outputPeak *= 0.92;

  drawVU(vuPosX[1], vuPosY[1], outputPeak);
}



void Levels :: drawVU(int16_t xPos, int16_t yPos, float vu)
{
  int16_t vuHeight;
  uint16_t vuColor;

  if (vu > 0)
  {
    // convert to a VU-style logarithmic readout
    float logVu = log(vu + 1.0) * 100;
    logVu = constrain(logVu, 1, 184);
    vuHeight = (int16_t)logVu;
  }
  else
    vuHeight = 1;

  // determine color
  vuColor = ILI9341_GREEN;
  if (vuHeight > 90)
    vuColor = ILI9341_YELLOW;
  if (vuHeight > 140)
    vuColor = ILI9341_RED;

  // drawn from top down, erase top portion, fill bottom
  tft.drawRect(xPos, yPos, 31, 185, ILI9341_YELLOW);
  tft.fillRect(xPos + 1, yPos + 1,       29, 185 - vuHeight, ILI9341_BLACK);
  tft.fillRect(xPos + 1, 185 - vuHeight, 29, vuHeight,       vuColor);
}

#endif
