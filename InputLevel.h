/*
    InputLevel. h - code for the input level adjustment

    version 2.0B1
    Dec 2019



*/


#include "guiItems.h"



// prototypes
void initInputLevel();
void printInputConfig();
void drawInitialInputTrimScreen();
void updateInputTrimScreen();



// global vars
float maxEQ = 0;
float Vu;
float maxInVu, maxOutVu;
int Vu_Color;
int Vu_Height;





void initInputLevel()
{
  Serial.println("Input Level Initialized");
}


void printInputConfig()
{
      Serial.print(F("Input Level Adj = "));  Serial.println(cfg.inputValueAdj);

}



void drawInputLevelScreen()
{
  int pos;
  tft.fillScreen(GUI_FILL_COLOR);
  tft.setTextColor(GUI_ITEM_COLOR);
  tft.setFont(Arial_14);
  tft.setCursor(10, 200);
  tft.print(" Input Adj      Input      Output");

  // Draw Slider
  cfg.inputValueAdj = constrain(cfg.inputValueAdj, 0, 15);
  pos = 182 - cfg.inputValueAdj * 12; //scale 0 -> 15 into 182 -> 0
  tft.fillRoundRect(40, 0, 29, 192, 8, GUI_SHAPE_COLOR);
  tft.fillRoundRect(40, pos, 29, 10, 8, GUI_FOCUS_ITEM_COLOR);
  tft.fillRect(40, pos + 3, 29, 4, GUI_FILL_COLOR);

  // draw outline around Vu Input Meter
  tft.drawRect(121, 0, 35, 190, GUI_TEXT_COLOR);
  tft.drawRect(221, 0, 35, 190, GUI_TEXT_COLOR);


  // find max EQ level. Used in calc for VU level
  maxEQ = 0;
  for (uint8_t i = 0; i < numEqItems; i++)
  {
    if (cfg.eqBandVals[i] > maxEQ)
      maxEQ = cfg.eqBandVals[i];
  }


  Input_TrimChanged = false;
  initialScreenDrawn = true;
}


void updateInputLevelScreen()
{
  int pos;

  //  handle Value Encoder
  valEncVal = valueEncoder.read() >> 2;
  if (valEncVal > lastValEncVal)
  {
    cfg.inputValueAdj += 1;
    cfg.inputValueAdj = constrain(cfg.inputValueAdj, 0, 15);

    pos = 182 - cfg.inputValueAdj * 12;  //scale 0 -> 15 into 182 -> 0
    tft.fillRoundRect(40, 0, 29, 192, 8, GUI_SHAPE_COLOR);
    tft.fillRoundRect(40, pos, 29, 10, 8, GUI_FOCUS_ITEM_COLOR);
    tft.fillRect(40, pos + 3, 29, 4, GUI_FILL_COLOR);

    audioShield.lineInLevel(cfg.inputValueAdj);
    Input_TrimChanged = true;
    lastValEncVal = valEncVal;
  }

  if (valEncVal < lastValEncVal)
  {
    cfg.inputValueAdj -= 1;
    cfg.inputValueAdj = constrain(cfg.inputValueAdj, 0, 15);

    pos = 182 - cfg.inputValueAdj * 12;  //scale 0 -> 15 into 182 -> 0
    tft.fillRoundRect(40, 0, 29, 192, 8, GUI_SHAPE_COLOR);
    tft.fillRoundRect(40, pos, 29, 10, 8, GUI_FOCUS_ITEM_COLOR);
    tft.fillRect(40, pos + 3, 29, 4, GUI_FILL_COLOR);

    audioShield.lineInLevel(cfg.inputValueAdj);
    Input_TrimChanged = true;
    lastValEncVal = valEncVal;
  }

  if (peak1.available())
  {
    Vu = peak1.readPeakToPeak();
    Vu = Vu  * exp10(maxEQ / 20.0);    // ADJUST FOR MAX EQ SETTING AS A SAFETY FACTOR IN PREVENTING OVERLOAD
    if (Vu > 0)
    {
        // peak-hold display
      if (Vu > maxInVu)
        maxInVu = Vu;
      else
        maxInVu *= 0.92;

      Vu_Height = (int)((1.0 + log(maxInVu)) * 90.0); // convert to a VU-style logarithmic readout
      Vu_Height = constrain(Vu_Height, 0, 182);

      if (Vu_Height < 90)
        Vu_Color = ILI9341_GREEN;
      if (Vu_Height > 90)
        Vu_Color = ILI9341_YELLOW;
      if (Vu_Height > 140)
        Vu_Color = ILI9341_RED;

      tft.fillRoundRect(122, 2, 29, 184 - Vu_Height, 4, GUI_FILL_COLOR);
      tft.fillRoundRect(122, 184 - Vu_Height, 29, Vu_Height, 4, Vu_Color);
      //tft.fillRectVGradient(120, 184 - vuHeight, 29, vuHeight, ILI9341_RED, ILI9341_GREEN);
    }
  }

  if (peak2.available())
  {
    Vu = peak2.readPeakToPeak();
    Vu = Vu  * exp10(maxEQ / 20.0);    // ADJUST FOR MAX EQ SETTING AS A SAFETY FACTOR IN PREVENTING OVERLOAD
    if (Vu > 0)
    {
                // peak-hold display
      if (Vu > maxOutVu)
        maxOutVu = Vu;
      else
        maxOutVu *= 0.92;
        
      Vu_Height = (int)((1.0 + log(maxOutVu)) * 90.0); // convert to a VU-style logarithmic readout
      Vu_Height = constrain(Vu_Height, 0, 182);

      if (Vu_Height < 90)
        Vu_Color = ILI9341_GREEN;
      if (Vu_Height > 90)
        Vu_Color = ILI9341_YELLOW;
      if (Vu_Height > 140)
        Vu_Color = ILI9341_RED;

      tft.fillRoundRect(222, 2, 29, 184 - Vu_Height, 4, GUI_FILL_COLOR);
      tft.fillRoundRect(222, 184 - Vu_Height, 29, Vu_Height, 4, Vu_Color);
      //tft.fillRectVGradient(220, 184 - vuHeight, 29, vuHeight, ILI9341_RED, ILI9341_GREEN);
    }
  }
}
