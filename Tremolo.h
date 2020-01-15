/*
    Tremolo. h - code to implement the Tremolo Effect
    also called the Compressor

    version 2.0B1
    Dec 2019



*/


#include "guiItems.h"


// prototypes
void initTremolo();
void enableTremolo();
void disableTremolo();
void drawInitialTremoloScreen();
void updateTremoloScreen();



// global vars
boolean tremoloActive    = false;




void initTremolo()
{

  // setup Tremolo - mixer 2 combines dc level with sine
  sine1.amplitude(cfg.tremoloDepth);
  sine1.frequency(cfg.tremoloSpeed);
  dc1.amplitude(0.7);

  // mix sine with pot dc level
  mixer2.gain(0, UNITY_GAIN);
  mixer2.gain(1, UNITY_GAIN);

  disableTremolo();
  
  Serial.println("Tremolo Initialized");
}


void enableTremolo()
{
  mixer1.gain(TREMOLO, 1.0);
  tremoloActive = true;
  Serial.println("Tremolo On");
}


void disableTremolo()
{
  mixer1.gain(TREMOLO, 0.0);
  tremoloActive = false;
  Serial.println("Tremolo On");
}




void drawInitialTremoloScreen()
{
  int pos;
  tft.fillScreen(GUI_FILL_COLOR);
  tft.setTextColor(GUI_ITEM_COLOR);
  tft.setFont(Arial_14);
  tft.setCursor(20, 200);
  tft.print("Speed   Depth");
  tft.setCursor(120, 220);
  tft.setTextColor(ILI9341_WHITE);
  tft.print("Tremolo");

  // Draw 2 Sliders
  cfg.tremoloSpeed = constrain(cfg.tremoloSpeed, 0.5, 8);
  sine1.frequency(cfg.tremoloSpeed);

  pos = 194 - (int)((cfg.tremoloSpeed * 24.0)); //scale 0.5 -> 8 into 182 -> 0
  tft.fillRoundRect(29, 0, 29, 192, 8, GUI_SHAPE_COLOR);
  tft.fillRoundRect(29, pos, 29, 10, 8, GUI_FOCUS_ITEM_COLOR);
  tft.fillRect(29, pos + 3, 29, 4, GUI_FILL_COLOR);

  cfg.tremoloDepth = constrain(cfg.tremoloDepth, 0, 0.6);
  sine1.amplitude(cfg.tremoloDepth);

  pos = 182 - (int)(cfg.tremoloDepth * 300.0);
  tft.fillRoundRect(100, 0, 29, 192, 8, GUI_SHAPE_COLOR);
  tft.fillRoundRect(100, pos, 29, 10, 8, GUI_ITEM_COLOR);
  tft.fillRect(100, pos + 3, 29, 4, GUI_FILL_COLOR);
  
  lastTremoloScreenFocusItem = -1;
  tremoloValueChanged = false;
  tremoloScreenFocusItemChanged = false;
  initialScreenDrawn = true;
}





void updateTremoloScreen()
{
  int pos;
  
  // first handle Parameter Encoder
  paramEncVal = paramEncoder.read() >> 4;
  if (paramEncVal > lastParamEncVal)
  {
    lastTremoloScreenFocusItem = tremoloScreenFocusItem;
    tremoloScreenFocusItem++;
    tremoloScreenFocusItem = tremoloScreenFocusItem & 1;
    tremoloScreenFocusItemChanged = true;
    lastParamEncVal = paramEncVal;
  }

  if (paramEncVal < lastParamEncVal)
  {
    lastTremoloScreenFocusItem = tremoloScreenFocusItem;
    tremoloScreenFocusItem--;
    tremoloScreenFocusItem = tremoloScreenFocusItem & 1;
    tremoloScreenFocusItemChanged = true;
    lastParamEncVal = paramEncVal;
  }

  if ((lastTremoloScreenFocusItem != -1) && (tremoloScreenFocusItemChanged == true))
  {
    switch (lastTremoloScreenFocusItem)
    {
      case 0:
        pos = 194 - (int)((cfg.tremoloSpeed * 24.0)); //scale 0.5 -> 8 into 182 -> 0
        tft.fillRoundRect(29, 0, 29, 192, 8, GUI_SHAPE_COLOR);
        tft.fillRoundRect(29, pos, 29, 10, 8, GUI_ITEM_COLOR);
        tft.fillRect(29, pos + 3, 29, 4, GUI_FILL_COLOR);

        pos = 182 - (int)(cfg.tremoloDepth * 300.0);
        tft.fillRoundRect(100, 0, 29, 192, 8, GUI_SHAPE_COLOR);
        tft.fillRoundRect(100, pos, 29, 10, 8, GUI_FOCUS_ITEM_COLOR);
        tft.fillRect(100, pos + 3, 29, 4, GUI_FILL_COLOR);
        break;

      case 1:
        pos = 194 - (int)(cfg.tremoloSpeed * 24.0); //scale 0.5 -> 8 into 182 -> 0
        tft.fillRoundRect(29, 0, 29, 192, 8, GUI_SHAPE_COLOR);
        tft.fillRoundRect(29, pos, 29, 10, 8, GUI_FOCUS_ITEM_COLOR);
        tft.fillRect(29, pos + 3, 29, 4, GUI_FILL_COLOR);

        pos = 182 - (int)(cfg.tremoloDepth * 300.0);
        tft.fillRoundRect(100, 0, 29, 192, 8, GUI_SHAPE_COLOR);
        tft.fillRoundRect(100, pos, 29, 10, 8, GUI_ITEM_COLOR);
        tft.fillRect(100, pos + 3, 29, 4, GUI_FILL_COLOR);
        break;
    }
    tremoloScreenFocusItemChanged = false;
  }

  // next handle Value Encoder
  valEncVal = valueEncoder.read() >> 2;
  if (valEncVal > lastValEncVal)
  {
    switch (tremoloScreenFocusItem)
    {
      case 0:
        cfg.tremoloSpeed += 0.2;
        cfg.tremoloSpeed = constrain(cfg.tremoloSpeed, 0.5, 8);
        sine1.frequency(cfg.tremoloSpeed);

        pos = 194 - (int)((cfg.tremoloSpeed * 24.0)); //scale 0.5 -> 8 into 182 -> 0
        tft.fillRoundRect(29, 0, 29, 192, 8, GUI_SHAPE_COLOR);
        tft.fillRoundRect(29, pos, 29, 10, 8, GUI_FOCUS_ITEM_COLOR);
        tft.fillRect(29, pos + 3, 29, 4, GUI_FILL_COLOR);
        break;

      case 1:
        cfg.tremoloDepth += 0.02;
        cfg.tremoloDepth = constrain(cfg.tremoloDepth, 0, 0.6);
        sine1.amplitude(cfg.tremoloDepth);

        pos = 182 - (int)(cfg.tremoloDepth * 300.0);
        tft.fillRoundRect(100, 0, 29, 192, 8, GUI_SHAPE_COLOR);
        tft.fillRoundRect(100, pos, 29, 10, 8, GUI_FOCUS_ITEM_COLOR);
        tft.fillRect(100, pos + 3, 29, 4, GUI_FILL_COLOR);
    }
    tremoloValueChanged = true;
    lastValEncVal = valEncVal;
  }

  if (valEncVal < lastValEncVal)
  {
    switch (tremoloScreenFocusItem)
    {
      case 0:
        cfg.tremoloSpeed -= 0.2;
        cfg.tremoloSpeed = constrain(cfg.tremoloSpeed, 0.5, 8);
        sine1.frequency(cfg.tremoloSpeed);

        pos = 194 - (int)((cfg.tremoloSpeed * 24.0)); //scale 0.5 -> 8 into 182 -> 0
        tft.fillRoundRect(29, 0, 29, 192, 8, GUI_SHAPE_COLOR);
        tft.fillRoundRect(29, pos, 29, 10, 8, GUI_FOCUS_ITEM_COLOR);
        tft.fillRect(29, pos + 3, 29, 4, GUI_FILL_COLOR);
        break;

      case 1:
        cfg.tremoloDepth -= 0.02;
        cfg.tremoloDepth = constrain(cfg.tremoloDepth, 0, 0.6);
        sine1.amplitude(cfg.tremoloDepth);

        pos = 181 - (int)(cfg.tremoloDepth * 300.0);
        tft.fillRoundRect(100, 0, 29, 192, 8, GUI_SHAPE_COLOR);
        tft.fillRoundRect(100, pos, 29, 10, 8, GUI_FOCUS_ITEM_COLOR);
        tft.fillRect(100, pos + 3, 29, 4, GUI_FILL_COLOR);
    }
    tremoloValueChanged = true;
    lastValEncVal = valEncVal;
  }
}
