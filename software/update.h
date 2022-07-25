/***********************************************
    uipdate.h - updates general audio controls

    Version 1.1 . 25Feb2020

    By keeping all the global audio settings in one
    file, the logic is easier to maintain & debug.
    Individual effects are still handled in the effect's
    classs file


 *************************************************/



// audio off or on (unity gain)
#define AUDIO_OFF   0.0
#define AUDIO_ON    1.0




void updateMix(int pot)
{
  // Wet starts at 0 & goes to 100% (fully cw)
  // dry starts at 100% and goes to 0% (fully cw)
  float wetLevel = (float)pot / 1023.0;
  wetLevel = constrain(wetLevel, 0, 1.0);
  float dryLevel = 1.0 - wetLevel;
  dryLevel = constrain(dryLevel, 0, 1.0);

  mixer4.gain(DRY_OUT, dryLevel);
  mixer4.gain(WET_OUT, wetLevel);

  printValue("dry", dryLevel);
  printValue("wet", wetLevel);

  lastMixPot = pot;
}


void updateWahWah(int pot)
{
  wahwah.update(pot);
  printValue("wahWahPot", pot);
  lastWahWahPot = pot;
}



void updateAudio()
{
  // input mixer 1 standard settings
  mixer1.gain(LEFT_IN, AUDIO_ON);     // left channel input
  mixer1.gain(TEST_TONE, AUDIO_OFF);  // test tone input

  // delay input
  mixer5.gain(DELAY_DRY_IN, 1.0);

  // update settings for each effect
  compressor.update();
  eq.update();
  reverb.update();
  chorus.update();
  delayer.update();
  tremolo.update();
  flanger.update();
  levels.update();
}
