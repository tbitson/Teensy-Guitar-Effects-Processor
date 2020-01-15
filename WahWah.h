/*
    WahWah. h - code for the input level adjustment

    version 1.0
    Dec 2019

   NOTE: No GUI for the wah-wah

*/




// prototypes
void initWahWah();
void disableWahWah();
void enableWahWah();




// global vars
boolean wahWahActive     = false;




void initWahWah()
{
  // Wah-Wah filter params
  filter1.frequency(WAH_WAH_CENTER_FREQ);  //  filter center frequency
  filter1.resonance(WAH_WAH_GAIN);
  filter1.octaveControl(WAH_WAH_OCTAVES); // +/- 2 octaves over pedal range
  
  disableWahWah();
  Serial.println("WahWah Initialized");
}



void disableWahWah()
{
  mixer1.gain(WAHWAH, 0.0);
  wahWahActive = false;

}


void enableWahWah()
{
  mixer1.gain(WAHWAH, 1.0);
  wahWahActive = true;
}
