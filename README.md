Teensy Guitar Effects Processor

This is an in-work project!

The Teensy Guitar Effects Processor (Teensy GEP or just GEP) is a microcontroller based sound effects processor designed for enhancing and modifying electric guitar sounds. It uses a Teensy microcontroller paired with a dedicated sound processing chip to provide a multitude of effects. 

The base design uses a Teensy 3.2 along with the Teensy Audio Shield from PJRC (pjrc.com) to process stereo audio at a full 16-bit 44.1kHz sample rate. The audio shield supports line-level-in, line-level out, and has an on-board headphone jack. The shield also contain a micro-SD card slot to allow recording and playback of sound. A separate battery powered pre-amp module interfaces most common electric guitar pickups to the line-level input. 

The design inspiration for the Teensy GEP is based on a project that appeared in the July 2017 edition of Circuit Cellar magazine by Brian Miller.  The original version of the source code (dubbed 1.0) does not seem to have any significant change since the article was published, and no longer compiles with the current version of the Teensy compiler and/or audio libraries. Significant changes to the baseline code were required to a) bring up-to-date with current library versions, 2) match the current compiler versions, and 3) to simplify the code so that my limited brain power could understand how the code worked (or at least how I thought I worked). 

Starting with Version 1.4, the memory requirements exceeded the Teensy 3.2, and either a Teensy 3.5 or 3.6 must be used. The devices are pin-for-pin compatable, however the 3.5 and 3.6 are both physically long with additional pins. A re-spin of the PC board will include the new footprint. This project is NOT electrically compatible with Teensy 4.0, but the software is. Change the #define in hardware.h to select your device.

The next milestone is to create a class hierarchy for the effects to allow easy implementation of new effects and/or changes to existing effects.
