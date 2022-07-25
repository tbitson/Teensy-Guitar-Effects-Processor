/*
    guiElements.h - gui drawing elements and function

    version: 1.2
    Dec 2019

    Notes:
    - screen is 240 wide (x) by 320 wide (y)
    - 0,0 is upper left, which means that items such as sliders that
      move "up" have decreasing values. They need to be 'inverted'

    Available Font Sizes:
    Arial_8 through 14, 16, 18, 20, 24, 28, 32, 40, 48, 60, 72, 96

*/



#ifndef GUI_ITEMS_H
#define GUI_ITEMS_H

#include <font_Arial.h>
#include "hardware.h"

extern ILI9341_t3 tft;
extern boolean msgFlag;
extern String procType;



// prototypes
void eraseScreen();
void drawBorder(uint16_t);
void drawSlider(int16_t x, int16_t y, float position, bool focus);
void drawRadioButtons(int16_t x, int16_t y, uint8_t numButtons, String labels[], uint8_t selectedButton);
void drawButton(int16_t x, int16_t y, bool focus);
void drawTitle(String);
void drawLabel(int16_t x, int16_t y, String text, bool selected);
void drawLabels(uint8_t numlabels, int16_t labelPos[], String labelNames[]);
void drawLabelsSelected(uint8_t numlabels, int16_t labelPos[], String labelNames[], int8_t selected);
void splashScreen();
void tftMessage(String);


// display size
#define LCD_WIDTH   320
#define LCD_HEIGHT  240

// gui color settings
#define GUI_SHAPE_COLOR      ILI9341_BLUE
#define GUI_TEXT_COLOR       ILI9341_WHITE
#define GUI_ITEM_COLOR       ILI9341_YELLOW
#define GUI_FOCUS_ITEM_COLOR ILI9341_RED
#define GUI_FILL_COLOR       ILI9341_BLACK


// sliders
const int16_t sliderWidth = 29;
const int16_t sliderHeight = 182;
const int16_t sliderRadius = 8;
const int16_t handleHeight = 10;
const int16_t handleAccentHeight = 4;
const float sliderFullScale = 100.0;


// radio buttons
const int16_t  BUTTON_SIZE = 10;



void initLCD()
{
  tft.begin();
  delay(2000);
  tft.setRotation(3);
  delay(1000);
  tft.fillScreen(GUI_FILL_COLOR);
  delay(200);
}


void eraseScreen()
{
  Serial.println("erase");
  tft.fillScreen(GUI_FILL_COLOR);
  drawBorder(ILI9341_RED);
  Serial.println("done erase");
}


void drawBorder(uint16_t borderColor)
{
  tft.drawRect(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 4, borderColor);
}


void drawVLine(int16_t x, int16_t y1, int16_t y2, uint16_t color)
{
  tft.drawFastVLine(x, y1, y2, color);
}



void drawSlider(int16_t x, int16_t y, float setting, bool focus)
{
  // x, y - starting pos
  // setting in %. 0 = 0,  full scale = 100
  // focus = true if item selected
  // for scale of 0 to 256 use x 2 / 5
  // for scale of 0.0 to 1.0 scale just x 100.0

  // if x position is < 0, skip this slider
  if (x < 0)
    return;

  // draw outer rect
  tft.fillRoundRect(x, y, sliderWidth, sliderHeight + 10, sliderRadius, GUI_SHAPE_COLOR);

  // calc handle position
  // to keep the handle inside the rect, make its range a bit smaller
  // scale 0 to 100%. remember, the top is 0, so need to invert
  setting = constrain(setting, 0, 100.0);
  int16_t level = (int16_t)((100.0 - setting) * ((float)sliderHeight - 6.0) / 100.0) + 3.0;
  //printValue("slider handle pos", level);

  // draw handle
  if (focus)
    tft.fillRoundRect(x, level, sliderWidth, handleHeight, sliderRadius, GUI_FOCUS_ITEM_COLOR);
  else
    tft.fillRoundRect(x, level, sliderWidth, handleHeight, sliderRadius, GUI_ITEM_COLOR);

  tft.fillRect(x, level + 3, sliderWidth, handleAccentHeight, GUI_FILL_COLOR);
}





void drawRadioButtons(int16_t x, int16_t y, uint8_t numButtons, String labels[], uint8_t selectedButton)
{
  // x, y - starting pos
  // buttons are drawn vertically
  // number of buttons up to 4
  // buttonXPos1[numGrp1Buttons] = {20, 20, 20};
  // selectedButton = radio button highlighted
  // isSelected = is text highlighted


  bool selected = false;

  for (uint8_t i = 0; i < numButtons; i++)
  {
    if (i == selectedButton)
      selected = true;
    else
      selected = false;

    int16_t yOffset = y + (i * 50);

    drawButton(x, yOffset, selected);
    drawLabel(x - 16, yOffset - 30, labels[i], false);
  }
}





//void drawButton(int16_t x, int16_t y, const char *text, bool selected)
void drawButton(int16_t x, int16_t y, bool selected)
{
  uint16_t color = ILI9341_WHITE;
  if (selected)
    color = ILI9341_RED;

  tft.fillCircle(x,  y, BUTTON_SIZE, color);

  // set up default button text size
  tft.setFont(Arial_14);

  //tft.setTextColor(GUI_TEXT_COLOR);
  //printCenteredString(text, x, y, BUTTON_WIDTH, BUTTON_HEIGHT);
}


// draw a single label, assumes text size is already set
void drawLabel(int16_t x, int16_t y, String text, bool selected)
{
  tft.setCursor(x, y);
  if (selected)
    tft.setTextColor(GUI_FOCUS_ITEM_COLOR);
  else
    tft.setTextColor(GUI_ITEM_COLOR);
  tft.print(text);
}



// draw an array of lables
void drawLabels(uint8_t numlabels, int16_t labelPos[], String labelNames[])
{
  tft.setFont(Arial_14);

  for (int8_t i = 0; i < numlabels; i++)
  {
    tft.setCursor(labelPos[i], 204);
    tft.setTextColor(GUI_ITEM_COLOR);
    tft.print(labelNames[i]);
  }
}



// draw an array of lables with one selected
void drawLabelsSelected(uint8_t numlabels, int16_t labelPos[], String labelNames[], int8_t selected)
{
  tft.setFont(Arial_14);

  for (int8_t i = 0; i < numlabels; i++)
  {
    tft.setCursor(labelPos[i], 204);
    if (i == selected)
      tft.setTextColor(GUI_FOCUS_ITEM_COLOR);
    else
      tft.setTextColor(GUI_ITEM_COLOR);
    tft.print(labelNames[i]);
  }
}


// screen title
void drawTitle(String text)
{
  // assume font width of 9 for arial_14
  // calc center x, y, width x, width y
  int16_t w = text.length() * 10 + 4;
  int16_t h = 16;
  int16_t x = (LCD_WIDTH / 2) - (w / 2);
  int16_t y = LCD_HEIGHT - h;

  tft.fillRect(x - 2, y, w, h, GUI_FILL_COLOR);

  tft.setFont(Arial_14);
  tft.setTextColor(GUI_TEXT_COLOR);
  tft.setCursor(x, y);
  tft.print(text);
}



// display splash Screen
void splashScreen()
{
  tft.drawRect(0, 0, 319, 239, ILI9341_RED);

  tft.setTextColor(ILI9341_BLUE);
  tft.setCursor(90, 25);
  tft.setFont(Arial_14);
  tft.print("BSH Electronics");

  tft.setTextColor(ILI9341_YELLOW);
  tft.setCursor(20, 70);
  tft.setFont(Arial_20);
  tft.print("Teensy Guitar Effects");

  tft.setCursor(100, 110);
  tft.print("Processor");

  tft.setTextColor(ILI9341_RED);
  tft.setCursor(105, 150);
  tft.setFont(Arial_14);
  tft.print(VERSION);

  tft.setCursor(105, 180);
  tft.setFont(Arial_14);
  tft.print(procType);
}



// show message on the lcd
void tftMessage(String message)
{
  tft.fillScreen(ILI9341_BLACK);
  tft.drawRect(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, ILI9341_RED);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setCursor(80, 60);
  tft.setFont(Arial_16);
  tft.print(message);
  delay(900);
  tft.fillScreen(ILI9341_BLACK);
  msgFlag = false;
}

#endif
