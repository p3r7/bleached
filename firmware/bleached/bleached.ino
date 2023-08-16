/* Sonoclast pd knobs for TeensyLC

  * Modified by andrew for bleached (Teensy 3.2) 02/22/2019


   Copyright March 2019 - havencking@gmail.com

     This program is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program.  If not, see <http://www.gnu.org/licenses/>.

   The Sonoclast pd knobs is a MIDI CC controller designed particularly
   for use with pure data.  However it could be used to control any
   software that recognizes MIDI CC messages.  Customize the Control
   Channels below if you like.

   Compiling instructions:

   Follow these instructions to install the Arduino software and Tensyduino:
   https://www.pjrc.com/teensy/td_download.html

   In the Arduino software be sure to select the following:
   Tools > Board > Teensy3.2
   Tools > USB Type > Serial + MIDI
*/


// ------------------------------------------------------------------------
// deps

// #include <EEPROM.h> // unused

#include "config.h"


// ------------------------------------------------------------------------
// macros

// wrap code to be executed only under DEBUG conditions in D()
#ifdef DEBUG
#define D(x) x
#else
#define D(x)
#endif


// ------------------------------------------------------------------------
// main

bool shouldSendForcedControlUpdate = false;
uint8_t sendForcedControlAt;

IntervalTimer midiWriteTimer;
IntervalTimer midiReadTimer;
int midiInterval = 1000; // 1ms
bool shouldDoMidiRead = false;
bool shouldDoMidiWrite = false;
bool forceMidiWrite = false;


// Track the knob state.
uint16_t pot_val[7] = {0xffff,
                       0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff
};
uint16_t prev_pot_val[7] = {0xffff,
                            0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff
};

// Prevent jitter when reading the potentiometers.
// Higher value is less chance of jitter but also less precision.
const uint8_t nbrhd = 5;

// Count the number of main loops.
uint16_t loop_count = 0;

void setup() {
  // serial monitoring for debugging
  // Serial.begin(38400);

  // potentiometers
  analogReadResolution(POT_BIT_RES);
  analogReadAveraging(POT_NUM_READS);

  usbMIDI.setHandleSystemExclusive(processIncomingSysex);

  // MIDI.begin();
  midiWriteTimer.begin(writeMidi, midiInterval);
  midiReadTimer.begin(readMidi, midiInterval);
}

void loop() {

  if(shouldSendForcedControlUpdate && (millis() > sendForcedControlAt)) {
    // it's now time to send a forced control update, so...

    // disable this
    shouldSendForcedControlUpdate = false;

    // this will force a write the next time the midiWrite callback fires.
    forceMidiWrite = true;
  }

  // Read each knob, and send MIDI CC only if the value changed.
  // NB: patched as current pot 7 is borked and spams noise
  for (uint8_t i = 0; i < NB_POTS; i++) {
    pot_val[i] = analogRead(pot[i]);
  }

  if (shouldDoMidiRead)
  {
    doMidiRead();
    noInterrupts();
    shouldDoMidiRead = false;
    interrupts();
  }

  if (shouldDoMidiWrite)
  {
    doMidiWrite();
    noInterrupts();
    shouldDoMidiWrite = false;
    interrupts();
  }

  // Periodically send MIDI CC for every knob so that the receiving end matches the knobs
  // even when changing pure data patches.
//  if (loop_count > LOOPS_PER_REFRESH) {
//    for (uint8_t i = 0; i < 7; i++) {
//      usbMIDI.sendControlChange(cc[i], analogRead(pot[i]) >> (POT_BIT_RES - 7), MIDI_CHANNEL);
//    }
//    loop_count = 0;
//  }
//  loop_count++;
}


/*
 * Tiny function called via interrupt
 * (it's important to catch inbound MIDI messages even if we do nothing with
 * them.)
 */
void readMidi()
{
  shouldDoMidiRead = true;
}

/*
 * Function called when shouldDoMidiRead flag is HIGH
 */

void doMidiRead()
{
  // MIDI.read();
  usbMIDI.read();
}

/*
 * Tiny function called via interrupt
 */
void writeMidi()
{
  shouldDoMidiWrite = true;
}

/*
 * The function that writes changes in slider positions out the midi ports
 * Called when shouldDoMidiWrite flag is HIGH
 */
void doMidiWrite()
{

  for (uint8_t i = 0; i < NB_POTS; i++) {
    if (((pot_val[i] < prev_pot_val[i] - nbrhd) ||
         (pot_val[i] > prev_pot_val[i] + nbrhd)) || forceMidiWrite) {
      usbMIDI.sendControlChange(cc[i], pot_val[i] >> (POT_BIT_RES - 7), MIDI_CHANNEL);
      prev_pot_val[i] = pot_val[i];
    }
  }

  forceMidiWrite = false;
}
