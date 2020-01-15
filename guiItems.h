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
#include "logo.c"


extern ILI9341_t3 tft;

extern long readParamEncoder();
extern long readValueEncoder();
extern boolean initialScreenDrawn;
extern boolean msgFlag;


// Hardcoded font dimensions
#define FONT_WIDTH  5
#define FONT_HEIGHT 7


// Horizontal and Vertical alignment enumerations
enum HORIZ_ALIGNMENT { HLEFT, HCENTER, HRIGHT };
enum VERT_ALIGNMENT  { VTOP, VMIDDLE, VBOTTOM };




// prototypes
void drawSlider(uint16_t x, uint16_t y, float position, bool focus);
bool updateSliderEncoders(uint8_t numItems);
void drawButton(uint16_t x, uint16_t y, bool focus);
void splashScreen();
void drawBorder(uint16_t);
void tftMessage(String);




// gui string functions
void printAlignedString(const char *string, int x, int y, int width, int height, HORIZ_ALIGNMENT hAlign, VERT_ALIGNMENT vAlign);
void printCenteredString(const char *string, int x, int y, int width, int height);
void printCenteredInt(int i, int x, int y, int width, int height);
void clearAndPrintCenteredInt(int i, int x, int y, int width, int height);
void clearAndPrintCenteredString(char *string, int x, int y, int width, int height);



// gui color settings
#define GUI_SHAPE_COLOR      ILI9341_BLUE
#define GUI_TEXT_COLOR       ILI9341_WHITE
#define GUI_ITEM_COLOR       ILI9341_YELLOW
#define GUI_FOCUS_ITEM_COLOR ILI9341_RED
#define GUI_FILL_COLOR       ILI9341_BLACK



// gui menu screens
// enum screens {EQ_SCREEN, COMPRESSOR_SCREEN, TREMOLO_SCREEN, REVERB_SCREEN, FLANGER_SCREEN, INPUT_SCREEN};


// old gui globals vars - phasing out
boolean tremoloValueChanged = false;
boolean tremoloScreenFocusItemChanged = false;

boolean flangerValueChanged = false;
boolean flangerScreenFocusItemChanged = false;

boolean reverbScreenFocusItemChanged = false;
boolean reverbValueChanged = false;
int ReverbChanged;

int Input_TrimChanged;

int EqScreenFocusItem;
int lastEqScreenFocusItem;

int tremoloScreenFocusItem;
int lastTremoloScreenFocusItem;

int flangerScreenFocusItem;
int lastFlangerScreenFocusItem;

int compScreenFocusItem;
int lastCompScreenFocusItem;

int reverbScreenFocusItemValue[5];
int reverbScreenFocusItem;
int lastReverbScreenFocusItem;



// new gui vars
//float maxEQ;
// uint16_t pos[5];
//int16_t selectedItem = 0;
//int16_t lastSelectedItem = 0;
//bool selectedItemChanged = false;
//bool guiValueChanged = false;

// maximum number of slider
//float slidePos[8];



// constants
const uint16_t sliderWidth = 29;
const uint16_t sliderHeight = 182;
const uint16_t sliderRadius = 8;
const uint16_t handleHeight = 10;
const uint16_t handleAccentHeight = 4;

float sliderFullScale = 100.0; 
float sliderCenter = sliderFullScale / 2.0;


const uint16_t  BUTTON_WIDTH = 20;
const uint16_t  BUTTON_HEIGHT = 10;



void initLCD()
{
  tft.begin();
  delay(100);
  tft.setRotation(3);
  delay(100);
  tft.fillScreen(GUI_FILL_COLOR);
}




void drawSlider(uint16_t x, uint16_t y, float setting, bool focus)
{
  // x, y - starting pos
  // setting in %. 0 = 0,  full scale = 100
  // focus = true if item selected
  // for scale of 0 to 256 use x 2 / 5
  // for scale of 0.0 to 1.0 scale just x 100.0

  // draw outer rect
  tft.fillRoundRect( x, y, sliderWidth, sliderHeight + 10, sliderRadius, GUI_SHAPE_COLOR);

  // calc handle position
  // to keep the handle inside the rect, make its range a bit smaller
  // scale 0 to 100%. remember, the top is 0, so need to invert
  setting = constrain(setting, 0, 100.0);
  uint16_t level = (uint16_t)((100.0 - setting) * ((float)sliderHeight - 6.0) / 100.0) + 3.0;
  //printValue("slider handle pos", level);

  // draw handle
  if (focus)
    tft.fillRoundRect(x, level, sliderWidth, handleHeight, sliderRadius, GUI_FOCUS_ITEM_COLOR);
  else
    tft.fillRoundRect(x, level, sliderWidth, handleHeight, sliderRadius, GUI_ITEM_COLOR);

  tft.fillRect(x, level + 3, sliderWidth, handleAccentHeight, GUI_FILL_COLOR);

}



void drawButton(uint16_t x, uint16_t y, const char *text, bool selected)
{
  uint16_t color = ILI9341_WHITE;
  if (selected)
    color = ILI9341_RED;

  tft.fillCircle(x,  y, 10, color);
  tft.setTextColor(GUI_TEXT_COLOR);
  printCenteredString(text, x, y, BUTTON_WIDTH, BUTTON_HEIGHT);
}


// depricated - use drawButton()
void Button(int x, int y, int w, int h, uint16_t textColor, uint16_t bkgColor, const char *text)
{
  tft.fillRect(x, y, w, h, bkgColor);
  tft.drawRect(x, y, w, h, GUI_ITEM_COLOR);
  tft.setTextColor(textColor);
  printCenteredString(text, x, y, w, h);
}





// display splash Screen
void splashScreen()
{
  tft.drawRect(0, 0, 319, 239, ILI9341_RED);
  tft.writeRect(80, 20, logo.width, logo.height, (uint16_t*)(logo.pixel_data));

  tft.setTextColor(ILI9341_YELLOW);
  tft.setCursor(100, 100);
  tft.setFont(Arial_20);
  tft.print("Teensy");

  tft.setCursor(10, 140);
  tft.setFont(Arial_20);
  tft.print("Guitar Effects Processor");

  tft.setCursor(85, 180);
  tft.setFont(Arial_14);
  tft.print(VERSION);

  if (devMode)
  {
    tft.setCursor(80, 210);
    tft.setFont(Arial_14);
    tft.setTextColor(ILI9341_RED);
    tft.print("Dev Mode Enabled");
  }
}


void drawBorder(uint16_t borderColor)
{
  tft.drawRect(0, 0, 319, 239, borderColor);
}



// show message on the lcd
void tftMessage(String message)
{
  tft.fillScreen(ILI9341_BLACK);
  tft.drawRect(0, 0, 319, 239, ILI9341_RED);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setCursor(80, 60);
  tft.setFont(Arial_16);
  tft.print(message);
  delay(1000);
  tft.fillScreen(ILI9341_BLACK);
  msgFlag = false;
}

// Specialized string printing functions


void printAlignedString(const char *string, int x, int y, int width, int height, HORIZ_ALIGNMENT hAlign, VERT_ALIGNMENT vAlign)
{
  int length = strlen(string);

  // Determine font dimensions from stock 5x7 font
  int factor = tft.getTextSize();
  int fontWidth  = (FONT_WIDTH  + 1) * factor;
  int fontHeight = (FONT_HEIGHT + 1) * factor;

  int aX = x;
  int aY = y;

  // Process horizontal alignment
  switch (hAlign)
  {
    case HLEFT:
      break;

    case HCENTER:
      aX = x + (width / 2) - ((length * fontWidth) / 2);
      break;

    case HRIGHT:
      aX = x + width - (length * fontWidth);
      break;
  }

  switch (vAlign)
  {
    case VTOP:
      break;

    case VMIDDLE:
      aY = y + (height / 2) - (fontHeight / 2);
      break;

    case VBOTTOM:
      aY = y + height - fontHeight;
      break;
  }

  // Print the string
  tft.setCursor(aX, aY);
  tft.print(string);
}



// Prints string centered both horizontally and vertically in specified area
void printCenteredString(const char *string, int x, int y, int width, int height)
{
  printAlignedString(string, x, y, width, height, HCENTER, VMIDDLE);
}



// Prints an integer centered both horizontally and vertically in specified area
void printCenteredInt(int i, int x, int y, int width, int height)
{
  char buffer[10];

  // Convert int to string in buffer
  itoa(i, buffer, 10);

  printCenteredString(buffer, x, y, width, height);
}



// Helper function to clear field and print new int value
void clearAndPrintCenteredInt(int i, int x, int y, int width, int height)
{
  printCenteredInt(i, x, y, width, height);
}



// Helper function to clear field and print new string value
void clearAndPrintCenteredString(char *string, int x, int y, int width, int height)
{
  printCenteredString(string, x, y, width, height);
}

#endif
