/*
    Flanger. h - code to implement the Flanger effect

    version 2.0B1
    Dec 2019



*/


#include "guiItems.h"




// prototype
void initFlanger();
void disableFlanger();
void enableFlanger();
void drawInitialFlangerScreen();
void updateFlangerScreen();



// global vars
boolean flangerActive    = false;



void initFlanger()
{

  // start flanger  
  flange1.begin(delayline, FLANGE_DELAY_LENGTH, SIDX, cfg.flangerDepth, cfg.flangerSpeed);
  disableFlanger();

  //printValue("Audio Blocks", AUDIO_BLOCK_SAMPLES);
  //printValue("Flanger Max Depth", MAX_FLANGER_DEPTH);
  //printValue("Flanger Delay Length", FLANGE_DELAY_LENGTH);
  //printValue("Flanger Offset", SIDX);
  //printValue("Flanger Depth", cfg.flangerDepth);
  //printValue("Flanger Speed", cfg.flangerSpeed);

  Serial.println("Flanger Initialized");
}



void disableFlanger()
{
  mixer1.gain(FLANGER, 0);
  flange1.voices(FLANGE_DELAY_PASSTHRU, 0, 0);
  flangerActive = false;
  Serial.println("Flanger Off");
}



void enableFlanger()
{
  flange1.voices(SIDX, cfg.flangerDepth, cfg.flangerSpeed);
  mixer1.gain(FLANGER, 1.0);
  flangerActive = true;
  Serial.println("Flanger On");
}



void toggleFlanger()
{
  if (flangerActive)
  {
    disableFlanger();
  }
  else
  {
    enableFlanger();
  }
}



// in work TBD
void changeFlangerVoice()
{
  // more of a chorus-like effect for flanger
  //const int FLANGE_DELAY_LENGTH = 12 * AUDIO_BLOCK_SAMPLES;
  //const int MAX_FLANGER_DEPTH   = FLANGE_DELAY_LENGTH / 8;
  //const int SIDX                = 3 * FLANGE_DELAY_LENGTH / 4;

  // voices(offset, depth, delayRate);
  flange1.voices(SIDX, cfg.flangerDepth, cfg.flangerSpeed);
}





void drawInitialFlangerScreen()
{
  int pos;
  float sf;
  int ofs;
  tft.fillScreen(GUI_FILL_COLOR);
  tft.setTextColor(GUI_ITEM_COLOR);
  tft.setFont(Arial_14);
  tft.setCursor(20, 200);
  tft.print("Speed   Depth");
  tft.setCursor(120, 220);
  tft.setTextColor(ILI9341_WHITE);
  tft.print("Flanger");

  // Draw 2 Sliders
  cfg.flangerSpeed = constrain(cfg.flangerSpeed, 0.15, 2);
  // scale 0.15 -> 2 into 182 -> 0 = 1/(2.0 - 0.15) * 182 = 98.4
  pos = 214 - (int)((cfg.flangerSpeed * 98.4));

  tft.fillRoundRect(29, 0, 29, 192, 8, GUI_SHAPE_COLOR);
  tft.fillRoundRect(29, pos, 29, 10, 8, GUI_FOCUS_ITEM_COLOR);
  tft.fillRect(29, pos + 3, 29, 4, GUI_FILL_COLOR);

  cfg.flangerDepth = constrain(cfg.flangerDepth, 48, MAX_FLANGER_DEPTH);
  sf = 182.0 / (float)(MAX_FLANGER_DEPTH - 48);
  ofs = 182 + (int)(48 * sf);
  pos = ofs - (int)(cfg.flangerDepth * sf);
  tft.fillRoundRect(100, 0, 29, 192, 8, GUI_SHAPE_COLOR);
  tft.fillRoundRect(100, pos, 29, 10, 8, GUI_ITEM_COLOR);
  tft.fillRect(100, pos + 3, 29, 4, GUI_FILL_COLOR);

  lastFlangerScreenFocusItem = -1;
  flangerValueChanged = false;
  flangerScreenFocusItemChanged = false;
  initialScreenDrawn = true;
  flange1.voices(SIDX, cfg.flangerDepth, cfg.flangerSpeed);
}





void updateFlangerScreen()
{
  int pos;
  float sf;
  int ofs;


  // first handle Parameter Encoder
  paramEncVal = paramEncoder.read() >> 2;
  if (paramEncVal > lastParamEncVal)
  {
    lastFlangerScreenFocusItem = flangerScreenFocusItem;
    flangerScreenFocusItem++;
    flangerScreenFocusItem = flangerScreenFocusItem & 1;
    flangerScreenFocusItemChanged = true;
    lastParamEncVal = paramEncVal;
  }

  if (paramEncVal < lastParamEncVal)
  {
    lastFlangerScreenFocusItem = flangerScreenFocusItem;
    flangerScreenFocusItem--;
    flangerScreenFocusItem = flangerScreenFocusItem & 1;
    flangerScreenFocusItemChanged = true;
    lastParamEncVal = paramEncVal;
  }

  if ((lastFlangerScreenFocusItem != -1) && (flangerScreenFocusItemChanged == true))
  {
    switch (lastFlangerScreenFocusItem)
    {
      case 0:
        cfg.flangerSpeed = constrain(cfg.flangerSpeed, 0.15, 2);
        pos = 214 - (int)(cfg.flangerSpeed * 98.4);
        tft.fillRoundRect(29, 0, 29, 192, 8, GUI_SHAPE_COLOR);
        tft.fillRoundRect(29, pos, 29, 10, 8, GUI_ITEM_COLOR);
        tft.fillRect(29, pos + 3, 29, 4, GUI_FILL_COLOR);

        cfg.flangerDepth = constrain(cfg.flangerDepth, 48, MAX_FLANGER_DEPTH);
        sf = 182.0 / (float)(MAX_FLANGER_DEPTH - 48);
        ofs = 182 + (int)(48 * sf);
        pos = ofs - (int)(cfg.flangerDepth * sf);
        tft.fillRoundRect(100, 0, 29, 192, 8, GUI_SHAPE_COLOR);
        tft.fillRoundRect(100, pos, 29, 10, 8, GUI_FOCUS_ITEM_COLOR);
        tft.fillRect(100, pos + 3, 29, 4, GUI_FILL_COLOR);
        break;

      case 1:
        cfg.flangerDepth = constrain(cfg.flangerDepth, 48, MAX_FLANGER_DEPTH);
        sf = 182.0 / (float)(MAX_FLANGER_DEPTH - 48);
        ofs = 181 + (int)(48 * sf);
        pos = ofs - (int)(cfg.flangerDepth * sf);
        tft.fillRoundRect(100, 0, 29, 192, 8, GUI_SHAPE_COLOR);
        tft.fillRoundRect(100, pos, 29, 10, 8, GUI_ITEM_COLOR);
        tft.fillRect(100, pos + 3, 29, 4, GUI_FILL_COLOR);

        cfg.flangerSpeed = constrain(cfg.flangerSpeed, 0.15, 2);
        pos = 214 - (int)((cfg.flangerSpeed * 98.4));
        tft.fillRoundRect(29, 0, 29, 192, 8, GUI_SHAPE_COLOR);
        tft.fillRoundRect(29, pos, 29, 10, 8, GUI_FOCUS_ITEM_COLOR);
        tft.fillRect(29, pos + 3, 29, 4, GUI_FILL_COLOR);
    }
  }

  flangerScreenFocusItemChanged = false;
  // next handle Value Encoder
  valEncVal = valueEncoder.read() >> 2;

  if (valEncVal > lastValEncVal)
  {
    switch (flangerScreenFocusItem)
    {
      case 0:
        cfg.flangerSpeed *= 1.1;
        cfg.flangerSpeed = constrain(cfg.flangerSpeed, 0.15, 2);
        pos = 214 - (int)((cfg.flangerSpeed * 98.4));
        tft.fillRoundRect(29, 0, 29, 192, 8, GUI_SHAPE_COLOR);
        tft.fillRoundRect(29, pos, 29, 10, 8, GUI_FOCUS_ITEM_COLOR);
        tft.fillRect(29, pos + 3, 29, 4, GUI_FILL_COLOR);
        break;

      case 1:
        cfg.flangerDepth += 10;
        cfg.flangerDepth = constrain(cfg.flangerDepth, 48, MAX_FLANGER_DEPTH);
        sf = 182.0 / (float)(MAX_FLANGER_DEPTH - 48);
        ofs = 182 + (int)(48 * sf);
        pos = ofs - (int)(cfg.flangerDepth * sf);
        tft.fillRoundRect(100, 0, 29, 192, 8, GUI_SHAPE_COLOR);
        tft.fillRoundRect(100, pos, 29, 10, 8, GUI_FOCUS_ITEM_COLOR);
        tft.fillRect(100, pos + 3, 29, 4, GUI_FILL_COLOR);
    }

    flangerValueChanged = true;
    lastValEncVal = valEncVal;
    flange1.voices(SIDX, cfg.flangerDepth, cfg.flangerSpeed);
    printValue("Flanger Speed", cfg.flangerSpeed);
    printValue("Flanger Depth", cfg.flangerDepth);

  }

  if (valEncVal < lastValEncVal)
  {
    switch (flangerScreenFocusItem)
    {
      case 0:
        cfg.flangerSpeed *= 0.9;
        cfg.flangerSpeed = constrain(cfg.flangerSpeed, 0.15, 2);
        pos = 214 - (int)((cfg.flangerSpeed * 98.4));
        tft.fillRoundRect(29, 0, 29, 192, 8, GUI_SHAPE_COLOR);
        tft.fillRoundRect(29, pos, 29, 10, 8, GUI_FOCUS_ITEM_COLOR);
        tft.fillRect(29, pos + 3, 29, 4, GUI_FILL_COLOR);
        break;

      case 1:
        cfg.flangerDepth -= 10;
        cfg.flangerDepth = constrain(cfg.flangerDepth, 48, MAX_FLANGER_DEPTH);
        sf = 182.0 / (float)(MAX_FLANGER_DEPTH - 48);
        ofs = 182 + (int)(48 * sf);
        pos = ofs - (int)(cfg.flangerDepth * sf);
        tft.fillRoundRect(100, 0, 29, 192, 8, GUI_SHAPE_COLOR);
        tft.fillRoundRect(100, pos, 29, 10, 8, GUI_FOCUS_ITEM_COLOR);
        tft.fillRect(100, pos + 3, 29, 4, GUI_FILL_COLOR);
    }

    flangerValueChanged = true;
    lastValEncVal = valEncVal;
    flange1.voices(SIDX, cfg.flangerDepth, cfg.flangerSpeed);
    printValue("Flanger Speed", cfg.flangerSpeed);
    printValue("Flanger Depth", cfg.flangerDepth);
  }
}
