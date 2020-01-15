/*
    Compressor. h - code for the Auto Volume Control (AVC)
    also called the Compressor

    version 2.0B1
    Dec 2019

*/


#include "guiItems.h"


// prototypes
void updateCompressorSettings();
void disableCompressor();
void enableCompressor();
void updateExpanderGain();
void updateExpanderResponse();
void updateCompressorThreshold();
void updateCompressorAttack();
void updateCompressorDecay();



// external hardware globals
extern Encoder valueEncoder;
extern Encoder paramEncoder;


// global vars in this file
boolean compScreenInitialized;
float  CompressorAdjustValue;

boolean compScreenFocusItemChanged = false;
boolean compressorValueChanged = false;

bool    compEnabled;
uint8_t compGain;
uint8_t compResponse;
uint8_t compHardLimit;
float   compThreshold;
float   compAttack;
float   compDecay;





void initCompressor()
{
  updateCompressorSettings();
  compScreenInitialized = false;
  disableCompressor();

  Serial.println("Compressor Initialized");
}



void disableCompressor()
{
  audioShield.autoVolumeDisable();
  cfg.compEnabled = false;
  Serial.println("Compressor OFF");
}



void enableCompressor()
{
  audioShield.autoVolumeEnable();
  cfg.compEnabled = true;
  Serial.println("Compressor ON");
}


void toggleCompressor()
{
  if (cfg.compEnabled)
    disableCompressor();
  else
    enableCompressor();
}


// primary compressor settings function
void updateCompressorSettings()
{
  // example: audioShield.autoVolumeControl(2, 1, 0, -5, 0.5, 0.5);
  //
  // configure the compressor with current settings
  // gain: 0 = 0dB, 1 = 6dB, 2 = 12dB
  // response (integration time): 0 = 0ms, 1 = 25ms, 2 = 50 ms, 3 = 100 ms (values > 3 permissible)
  // hardLimit: 0 = use "soft knee', 1 = hard limit (don't allow values > threshold)
  // threshold: 0 to -96 in dBFS
  // attack: controls decrease in gain in dB per second  (0 to 1.0 ???)
  // decay: how fast gain is restored in dB per second   (0 to 1.0 ???)

  audioShield.autoVolumeControl(cfg.compGain, cfg.compResponse, cfg.compHardLimit, cfg.compThreshold, cfg.compAttack, cfg.compDecay);
}



uint8_t gain[] = {0, 1, 2};
uint8_t response[] = {0, 1, 2, 3};
uint8_t limit[] = {0, 1};



void drawInitialCompressorScreen()
{
  int pos;
  compScreenFocusItem = 0;
  lastCompScreenFocusItem = -1;
  paramEncoder.write(0);
  valueEncoder.write(0);
  lastParamEncVal = 0;
  lastValEncVal = 0;


  tft.fillScreen(GUI_FILL_COLOR);
  tft.setFont(Arial_14);
  tft.setTextColor(GUI_FOCUS_ITEM_COLOR);
  tft.setCursor(0, 200);
  tft.print("Gain");
  tft.setTextColor(GUI_ITEM_COLOR);
  tft.print("    Resp      Thresh Attack Decay");
  tft.setCursor(0, 220);
  tft.setTextColor(ILI9341_WHITE);
  tft.print("  Expander             Compressor");
  tft.drawFastVLine(130, 0, 239, ILI9341_CYAN);

  switch (cfg.compGain)
  {
    case 0:
      tft.fillCircle(20, 60, 10, ILI9341_RED);
      tft.fillCircle(20, 120, 10, ILI9341_WHITE);
      tft.fillCircle(20, 180, 10, ILI9341_WHITE);
      break;

    case 1:
      tft.fillCircle(20, 60, 10, ILI9341_WHITE);
      tft.fillCircle(20, 120, 10, ILI9341_RED);
      tft.fillCircle(20, 180, 10, ILI9341_WHITE);
      break;

    case 2:
      tft.fillCircle(20, 60, 10, ILI9341_WHITE);
      tft.fillCircle(20, 120, 10, ILI9341_WHITE);
      tft.fillCircle(20, 180, 10, ILI9341_RED);
  }

  tft.setCursor(0, 30);
  tft.print("0 dB");
  tft.setCursor(0, 90);
  tft.print("6 dB");
  tft.setCursor(0, 150);
  tft.print("12 dB");

  switch (cfg.compResponse)
  {
    case 0:
      tft.fillCircle(80, 40, 10, ILI9341_RED);
      tft.fillCircle(80, 90, 10, ILI9341_WHITE);
      tft.fillCircle(80, 140, 10, ILI9341_WHITE);
      tft.fillCircle(80, 190, 10, ILI9341_WHITE);
      break;

    case 1:
      tft.fillCircle(80, 40, 10, ILI9341_WHITE);
      tft.fillCircle(80, 90, 10, ILI9341_RED);
      tft.fillCircle(80, 140, 10, ILI9341_WHITE);
      tft.fillCircle(80, 190, 10, ILI9341_WHITE);
      break;

    case 2:
      tft.fillCircle(80, 40, 10, ILI9341_WHITE);
      tft.fillCircle(80, 90, 10, ILI9341_WHITE);
      tft.fillCircle(80, 140, 10, ILI9341_RED);
      tft.fillCircle(80, 190, 10, ILI9341_WHITE);
      break;

    case 3:
      tft.fillCircle(80, 40, 10, ILI9341_WHITE);
      tft.fillCircle(80, 90, 10, ILI9341_WHITE);
      tft.fillCircle(80, 140, 10, ILI9341_WHITE);
      tft.fillCircle(80, 190, 10, ILI9341_RED);
  }

  tft.setCursor(65, 10);
  tft.print("0 ms");
  tft.setCursor(65, 60);
  tft.print("25 ms");
  tft.setCursor(65, 110);
  tft.print("50 ms");
  tft.setCursor(65, 160);
  tft.print("100 ms");

  // Draw 3 sliders
  pos = 182 - (int)(cfg.compThreshold * (182.0 / -96.0)); 
  tft.fillRoundRect(145, 0, 29, 192, 8, GUI_SHAPE_COLOR);
  tft.fillRoundRect(145, pos, 29, 10, 8, GUI_ITEM_COLOR);
  tft.fillRect(145, pos + 3, 29, 4, GUI_FILL_COLOR);

  pos = 182 - ((int)(cfg.compAttack * 182));
  tft.fillRoundRect(203, 0, 29, 192, 8, GUI_SHAPE_COLOR);
  tft.fillRoundRect(203, pos, 29, 10, 8, GUI_ITEM_COLOR);
  tft.fillRect(203, pos + 3, 29, 4, GUI_FILL_COLOR);

  pos = 182 - ((int)(cfg.compDecay * 182));
  tft.fillRoundRect(261, 0, 29, 192, 8, GUI_SHAPE_COLOR);
  tft.fillRoundRect(261, pos, 29, 10, 8, GUI_ITEM_COLOR);
  tft.fillRect(261, pos + 3, 29, 4, GUI_FILL_COLOR);

  compScreenFocusItemChanged = false;
  compressorValueChanged = false;
}



void updateCompressorScreen()
{
  // First handle parameter encoder changes
  paramEncVal = paramEncoder.read() >> 2;
  if (paramEncVal > lastParamEncVal)
  {
    lastCompScreenFocusItem = compScreenFocusItem;
    compScreenFocusItem++;
    compScreenFocusItem = constrain(compScreenFocusItem, 0, 5);
    compScreenFocusItemChanged = true;
    lastParamEncVal = paramEncVal;
  }

  if (paramEncVal < lastParamEncVal)
  {
    lastCompScreenFocusItem = compScreenFocusItem;
    compScreenFocusItem--;
    compScreenFocusItem = constrain(compScreenFocusItem, 0, 5);
    compScreenFocusItemChanged = true;
    lastParamEncVal = paramEncVal;
  }

  if (compScreenFocusItemChanged == true)
  {
    switch (compScreenFocusItem)
    {
      case 0:
        tft.setTextColor(GUI_FOCUS_ITEM_COLOR);
        tft.setCursor(0, 200);
        tft.print("Gain");
        tft.setTextColor(GUI_ITEM_COLOR);
        tft.print("    Resp      Thresh Attack Decay");
        break;

      case 1:
        tft.setCursor(0, 200);
        tft.setTextColor(GUI_ITEM_COLOR);
        tft.print("Gain");
        tft.setTextColor(GUI_FOCUS_ITEM_COLOR);
        tft.print("    Resp");
        tft.setTextColor(GUI_ITEM_COLOR);
        tft.print("      Thresh Attack Decay");
        break;

      case 2:
        tft.setCursor(0, 200);
        tft.setTextColor(GUI_ITEM_COLOR);
        tft.print("Gain    Resp");
        tft.setTextColor(GUI_FOCUS_ITEM_COLOR);
        tft.print("      Thresh ");
        tft.setTextColor(GUI_ITEM_COLOR);
        tft.print("Attack Decay");
        break;

      case 3:
        tft.setCursor(0, 200);
        tft.setTextColor(GUI_ITEM_COLOR);
        tft.print("Gain    Resp      Thresh ");
        tft.setTextColor(GUI_FOCUS_ITEM_COLOR);
        tft.print("Attack ");
        tft.setTextColor(GUI_ITEM_COLOR);
        tft.print("Decay");
        break;

      case 4:
        tft.setCursor(0, 200);
        tft.setTextColor(GUI_ITEM_COLOR);
        tft.print("Gain    Resp      Thresh Attack ");
        tft.setTextColor(GUI_FOCUS_ITEM_COLOR);
        tft.print("Decay");
        break;
    }
    compScreenFocusItemChanged = false;
  }

  // Next handle Value encoder changes
  valEncVal = valueEncoder.read() >> 2;
  if (valEncVal > lastValEncVal)
  {
    CompressorAdjustValue = 1;
    compressorValueChanged = true;
    lastValEncVal = valEncVal;
  }

  if (valEncVal < lastValEncVal)
  {
    CompressorAdjustValue = -1;
    compressorValueChanged = true;
    lastValEncVal = valEncVal;
  }

  if (compressorValueChanged == true)
  {
    switch (compScreenFocusItem)
    {
      case  0:
        updateExpanderGain();
        break;

      case  1:
        updateExpanderResponse();
        break;

      case  2:
        updateCompressorThreshold();
        break;

      case  3:
        updateCompressorAttack();
        break;

      case  4:
        updateCompressorDecay();
        break;
    }
    compressorValueChanged = false;
  }
}



void updateExpanderGain()
{
  cfg.compGain += CompressorAdjustValue;
  cfg.compGain = constrain(cfg.compGain, 0, 2);
  switch (cfg.compGain) {
    case 0:
      tft.fillCircle(20,  60, 10, ILI9341_RED);
      tft.fillCircle(20, 120, 10, ILI9341_WHITE);
      tft.fillCircle(20, 180, 10, ILI9341_WHITE);
      break;

    case 1:
      tft.fillCircle(20,  60, 10, ILI9341_WHITE);
      tft.fillCircle(20, 120, 10, ILI9341_RED);
      tft.fillCircle(20, 180, 10, ILI9341_WHITE);
      break;

    case 2:
      tft.fillCircle(20,  60, 10, ILI9341_WHITE);
      tft.fillCircle(20, 120, 10, ILI9341_WHITE);
      tft.fillCircle(20, 180, 10, ILI9341_RED);
  }

  updateCompressorSettings();
  printValue("comp gain", cfg.compGain);
}



void updateExpanderResponse()
{
  cfg.compResponse += CompressorAdjustValue;
  cfg.compResponse = constrain(cfg.compResponse, 0, 3);

  switch (cfg.compResponse)
  {
    case 0:
      tft.fillCircle(80,  40, 10, ILI9341_RED);
      tft.fillCircle(80,  90, 10, ILI9341_WHITE);
      tft.fillCircle(80, 140, 10, ILI9341_WHITE);
      tft.fillCircle(80, 190, 10, ILI9341_WHITE);
      break;

    case 1:
      tft.fillCircle(80,  40, 10, ILI9341_WHITE);
      tft.fillCircle(80,  90, 10, ILI9341_RED);
      tft.fillCircle(80, 140, 10, ILI9341_WHITE);
      tft.fillCircle(80, 190, 10, ILI9341_WHITE);
      break;

    case 2:
      tft.fillCircle(80,  40, 10, ILI9341_WHITE);
      tft.fillCircle(80,  90, 10, ILI9341_WHITE);
      tft.fillCircle(80, 140, 10, ILI9341_RED);
      tft.fillCircle(80, 190, 10, ILI9341_WHITE);
      break;

    case 3:
      tft.fillCircle(80,  40, 10, ILI9341_WHITE);
      tft.fillCircle(80,  90, 10, ILI9341_WHITE);
      tft.fillCircle(80, 140, 10, ILI9341_WHITE);
      tft.fillCircle(80, 190, 10, ILI9341_RED);
  }

  updateCompressorSettings();
  printValue("comp response", cfg.compResponse);
}



void updateCompressorThreshold()
{
  int pos;
  cfg.compThreshold -= CompressorAdjustValue / 2.0;
  cfg.compThreshold = constrain(cfg.compThreshold, -96, 0);

  pos = 182 - (int) ((float) cfg.compThreshold * (182.0 / -96.0)); // scale 0->65535 into  182 ->0
  tft.fillRoundRect(145, 0, 29, 192, 8, GUI_SHAPE_COLOR);
  tft.fillRoundRect(145, pos, 29, 10, 8, GUI_ITEM_COLOR);
  tft.fillRect(145, pos + 3, 29, 4, GUI_FILL_COLOR);

  updateCompressorSettings();
  printValue("Comp Threshold", cfg.compThreshold);
}



void updateCompressorAttack()
{
  int pos;
  cfg.compAttack += CompressorAdjustValue / 10;
  cfg.compAttack = constrain(cfg.compAttack, 0, 1);
  
  pos = 182 - ((int)(cfg.compAttack * 182));  // scale 0 -> 1024 into 182 -> 0
  tft.fillRoundRect(203, 0, 29, 192, 8, GUI_SHAPE_COLOR);
  tft.fillRoundRect(203, pos, 29, 10, 8, GUI_ITEM_COLOR);
  tft.fillRect(203, pos + 3, 29, 4, GUI_FILL_COLOR);

  updateCompressorSettings();
  printValue("comp attack", cfg.compAttack);
}


void updateCompressorDecay()
{
  int pos;
  cfg.compDecay += CompressorAdjustValue / 10;
  cfg.compDecay = constrain(cfg.compDecay, 0, 1);

  pos = 182.0 - ((int)(cfg.compDecay * 182)); 
  tft.fillRoundRect(261, 0, 29, 192, 8, GUI_SHAPE_COLOR);
  tft.fillRoundRect(261, pos, 29, 10, 8, GUI_ITEM_COLOR);
  tft.fillRect(261, pos + 3, 29, 4, GUI_FILL_COLOR);

  updateCompressorSettings();
  printValue("comp decay", cfg.compDecay);
}
