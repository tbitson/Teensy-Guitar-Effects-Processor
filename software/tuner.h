/******************************************************
 *  Tuner. h - code to implement the Guitar Tuner
 *  version 1.3.2   Sept 2020
 * 
 *  Based on Teensy Digital Guitar Amplifier / Effects Processor
 *  by Brian Miller Circuit Cellar July 2017
 * 
 * 
 ******************************************************/


#include "guiItems.h"



extern Bounce tunerSwitch;
extern ILI9341_t3 tft;


const PROGMEM float NoteFreq[35] =
{
  61.714,  65.385,  69.268,  73.425,  82.410,
  87.273,  92.500,  98.000,  103.784, 110.000,
  116.522, 123.249, 130.769, 138.537, 146.830,
  155.556, 164.800, 174.545, 185.000, 196.000,
  207.568, 220.000, 233.043, 246.940, 261.538,
  277.073, 293.699, 311.111, 329.630, 349.091,
  370.000, 392.000, 415.135, 440.000, 466.087
};


const PROGMEM char NoteName[36][4] =
{
  "C2 ", "C#2", "D2 ", "D#2", "E2 ",
  "F2 ", "F#2", "G2 ", "G#2", "A2 ",
  "A#2", "B2 ", "C3 ", "C#3", "D3 ",
  "D#3", "E3 ", "F3 ", "F#3", "G3 ",
  "G#3", "A3 ", "A#3", "B3 ", "C4 ",
  "C#4", "D4 ", "D#4", "E4 ", "F4 ",
  "F#4", "G4 ", "G#4", "A4 ", "A#4",
  "B4 "
};




void guitarTuner()
{
  float abs_delta, delta, note,  minimum_Delta, minimum_abs_Delta, higher_Note, lower_Note, cents;
  String note_String, last_Note_String;
  uint16_t cents_Graphic;

  higher_Note = 0;
  lower_Note = 0;
  minimum_Delta = 0;
  printValue("Guitar Tuna");

  tft.fillScreen(GUI_FILL_COLOR);
  tft.setFont(Arial_32);
  tft.setCursor(30, 0);
  tft.print("Guitar Tuna");
  tft.setFont(Arial_60);
  tft.setTextColor(ILI9341_WHITE);

  // may halt until a freq is detected
  notefreq1.begin(0.15);
  delay(100);

  bool tuningMode = true;
  while (tuningMode)
  {
    if (notefreq1.available())
    {
      minimum_abs_Delta = 9999;
      note = notefreq1.read();

      printValue("freq", note);

      for (int i = 0; i < 35; i++)
      {
        delta = note - NoteFreq[i];
        abs_delta = abs(delta);
        if (abs_delta < minimum_abs_Delta)
        {
          minimum_abs_Delta = abs_delta;
          minimum_Delta = delta;
          note_String = String(NoteName[i]);
          if (delta > 0)
          {
            higher_Note = NoteFreq[i + 1];
            lower_Note = NoteFreq[i];
          }
          else
          {
            higher_Note = NoteFreq[i];
            lower_Note = NoteFreq[i - 1];
          }
        }
      }

      if (minimum_Delta > 0)
        cents = (int)(100 * (note - lower_Note) / (higher_Note - lower_Note));
      else
        cents = (int)(100 * ( note - higher_Note) / (higher_Note - lower_Note));

      // transpose +-50 cents to 0-320 X co-ordinates
      cents_Graphic = (uint16_t)((50.0 + cents) * 3.2);
      cents_Graphic = constrain(cents_Graphic, 4, 315);

      // draw background bar graph
      tft.fillRect(  0, 50, 64, 50, ILI9341_RED);
      tft.fillRect( 64, 50, 80, 50, ILI9341_YELLOW);
      tft.fillRect(144, 50, 32, 50, ILI9341_GREEN);
      tft.fillRect(176, 50, 80, 50, ILI9341_YELLOW);
      tft.fillRect(256, 50, 63, 50, ILI9341_RED);

      // highlight the tuning in black
      tft.fillRect(cents_Graphic - 4, 50, 8, 50, ILI9341_BLACK);

      // display the note and octave in large font
      if (note_String != last_Note_String)
      {
        tft.fillRect(99, 149, 160, 80, ILI9341_BLACK);
        tft.setCursor(100, 150);
        tft.print(note_String);
        last_Note_String = note_String;
      }
      delay(150);   // display around 5 readings per sec
    }

    // if tuner switch is pressed again, exit
    tunerSwitch.update();
    if (tunerSwitch.fallingEdge())
    {
      printValue("Exiting Tuner");
      tuningMode = false;
    }
  }

  // shut off notefreq1 processing
  notefreq1.begin(-1.0);
 }
