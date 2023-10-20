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

#include <EEPROM.h> // unused
#include <ResponsiveAnalogRead.h>

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
// consts

enum mode{CC, CC14, RPN, NRPN};


// ------------------------------------------------------------------------
// live conf

uint8_t ch[] = { 1, 1, 1, 1,
                 1, 1, 1 };

uint8_t cc_mode = CC;

uint8_t cc[] = { CC01, CC02, CC03, CC04,
                 CC05, CC06, CC07 };

uint8_t cc14[][2] = { ECC01, ECC02, ECC03, ECC04,
                      ECC05, ECC06, ECC07 };

uint16_t nrpn[] = { NRPN1, NRPN2, NRPN3, NRPN4,
                    NRPN5, NRPN6, NRPN7 };


// ------------------------------------------------------------------------
// state - knobs

uint16_t pot_val[] = { 0xffff, 0xffff, 0xffff, 0xffff,
                       0xffff, 0xffff, 0xffff };

uint16_t prev_pot_val[] = { 0xffff, 0xffff, 0xffff, 0xffff,
                            0xffff, 0xffff, 0xffff };


// ------------------------------------------------------------------------
// state - midi state

bool shouldSendForcedControlUpdate = false;
uint8_t sendForcedControlAt;

IntervalTimer midiWriteTimer;
IntervalTimer midiReadTimer;
int midiInterval = 1000; // 1ms
bool shouldDoMidiRead = false;
bool shouldDoMidiWrite = false;
bool forceMidiWrite = false;


// ------------------------------------------------------------------------
// init

ResponsiveAnalogRead *analog_reads[NB_POTS];

void setup() {
  // Serial.begin(38400); // serial monitoring for debugging

  analogReadResolution(POT_BIT_RES);
  analogReadAveraging(POT_NUM_READS);

  // for (uint8_t i = 0; i < NB_POTS; i++) {
  //   analog_reads[i] = new ResonsiveAnalogRead(pot[i], true);
  //   analog_reads[i]->setAnalogResolution(RESOLUTION);
  // }

  usbMIDI.setHandleSystemExclusive(processIncomingSysex);
  midiWriteTimer.begin(writeMidi, midiInterval);
  midiReadTimer.begin(readMidi, midiInterval);
}


// ------------------------------------------------------------------------
// main loop

void loop() {

  if(shouldSendForcedControlUpdate && (millis() > sendForcedControlAt)) {
    // it's now time to send a forced control update, so...

    // disable this
    shouldSendForcedControlUpdate = false;

    // this will force a write the next time the midiWrite callback fires.
    forceMidiWrite = true;
  }

  // Read each knob, and send MIDI CC only if the value changed.
  for (uint8_t i = 0; i < NB_POTS; i++) {
    pot_val[i] = analogRead(pot[i]);
  }

  if (shouldDoMidiRead) {
    doMidiRead();
    noInterrupts();
    shouldDoMidiRead = false;
    interrupts();
  }

  if (shouldDoMidiWrite) {
    doMidiWrite();
    noInterrupts();
    shouldDoMidiWrite = false;
    interrupts();
  }
}


// ------------------------------------------------------------------------
// midi i/o

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
  uint16_t pot_val_14 = 0;

  for (uint8_t i = 0; i < NB_POTS; i++) {
    if (((pot_val[i] < prev_pot_val[i] - nbrhd) ||
         (pot_val[i] > prev_pot_val[i] + nbrhd)) || forceMidiWrite) {

      pot_val_14 = pot_val[i] << (14 - POT_BIT_RES);

      switch (cc_mode) {
      case CC:
        D(Serial.println("Sending CC!"));

        // usbMIDI.sendControlChange(cc[i], pot_val[i] >> (POT_BIT_RES - 7), ch[i]);
        usbMIDI.sendControlChange(cc[i], pot_val_14 >> 7, ch[i]);
        break;
      case CC14:
        D(Serial.println("Sending CC14! "));
        D(Serial.println(String(pot_val[i])
                         + " -> " + String(pot_val_14)
                         + " -> " + String(pot_val_14 >> 7) + " / " + String(pot_val_14 & 0x7F) ));
        usbMIDI.sendControlChange(cc14[i][0], pot_val_14 >> 7, ch[i]);
        usbMIDI.sendControlChange(cc14[i][1], pot_val_14 & 0x7F, ch[i]);
        break;
      case RPN:
        D(Serial.println("Sending RPN!"));
        // - address
        usbMIDI.sendControlChange(101, nrpn[i] >> 7, ch[i]);
        usbMIDI.sendControlChange(100, nrpn[i] & 0x7F, ch[i]);
        // - value
        usbMIDI.sendControlChange(6,  pot_val_14 >> 7, ch[i]);
        usbMIDI.sendControlChange(38, pot_val_14 & 0x7F, ch[i]);
        break;
      case NRPN:
        D(Serial.println("Sending NRPN!"));
        // - address
        usbMIDI.sendControlChange(99, nrpn[i] >> 7, ch[i]);
        usbMIDI.sendControlChange(98, nrpn[i] & 0x7F, ch[i]);
        // - value
        usbMIDI.sendControlChange(6,  pot_val_14 >> 7, ch[i]);
        usbMIDI.sendControlChange(38, pot_val_14 & 0x7F, ch[i]);
        break;
      default:
        D(Serial.println("Unexpected mode!"));
      }
      prev_pot_val[i] = pot_val[i];
    }
  }

  forceMidiWrite = false;
}
