/* status.h - displays the status of
 *  all efeccts. This file is now a placeholder
 *  for an in-work effects selection screen
 *  using touch 'buttons' for enable/disable
 *  the various effects. These will repolace the 4
 *  hardweare buttons, freeing them up for something
 *  else
 *  
 */
#ifndef STATUS_H
#define STATUS_H

#include "guiItems.h"




void statusScreen(bool initScreen)
{
  uint8_t numStatusButtons = 8;
  int16_t statusButtonsX = 20;
  int16_t statusButtonsY[numStatusButtons] = {20, 45, 70, 95, 120, 145, 170, 195};
  String statusLabels[numStatusButtons] = {"Compressor", "Reverb", "Equalizer", "Flanger", "Tremolo", "Wah-Wah", "Delayer", "Chorus"};
  bool status[numStatusButtons];

  if (initScreen)
  {
    eraseScreen();
    drawTitle("Effects Status");
  }

  status[0] = compressor.getStatus();
  status[1] = reverb.getStatus();
  status[2] = eq.getStatus();
  status[3] = flanger.getStatus();
  status[4] = tremolo.getStatus();
  status[5] = wahwah.getStatus();
  status[6] = delayer.getStatus();
  status[7] = chorus.getStatus();

  for (uint8_t i = 0; i < numStatusButtons; i++)
  {
    drawButton(statusButtonsX, statusButtonsY[i], status[i]);
    drawLabel(statusButtonsX + 20, statusButtonsY[i] - 5, statusLabels[i], false);
  }
}

#endif
