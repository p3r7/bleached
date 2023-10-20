

// ------------------------------------------------------------------------
// entrypoint

void processIncomingSysex(byte* sysexData, unsigned size) {
  D(Serial.println("Ooh, sysex"));
  D(printHexArray(sysexData, size));
  D(Serial.println());

  if(size < 3) {
    D(Serial.println("That's an empty sysex, bored now"));
    return;
  }

  if(!(sysexData[1] == 0x7d && sysexData[2] == 0x00 && sysexData[3] == 0x00)) {
    D(Serial.println("That's not a sysex message for us"));
    return;
  }

  uint8_t p = 0;
  uint8_t v = 0;

  switch (sysexData[4]) {
  case 0x1f: // 1nFo -> c0nFig
    D(Serial.println("Got an 1nFo request"));
    sendCurrentState();

    // also, in 1s time, please send the current state
    // of the controls, so the editor looks nice.
    shouldSendForcedControlUpdate = true;
    sendForcedControlAt = millis() + 1000;
    break;
  case 0x0e: // m0dE
    D(Serial.println("Got a m0dE switch request"));

    v = sysexData[5];
    if (v != CC
        && v != CC14
        && v != NRPN) {
      D(Serial.println("Unexpected mode, aborting!"));
      return;
    }
    D(Serial.println("Switching mode " + String(v)));
    cc_mode = v;

    sendCurrentState();
    shouldSendForcedControlUpdate = true;
    sendForcedControlAt = millis() + 1000;

    break;

  default:
    D(Serial.println("Unexpected sysex request!"));
  }
}


// ------------------------------------------------------------------------
// c0nFig

const uint8_t SYSEX_RESP_LEN = 12 + ( NB_POTS * 7) + 1;

void sendCurrentState() {
  //   0F - "c0nFig" - outputs its config:

  byte sysexData[SYSEX_RESP_LEN];

  sysexData[0] = 0x7d; // manufacturer (set to "prototyping, test, private use and experimentation")
  sysexData[1] = 0x00;
  sysexData[2] = 0x00;

  sysexData[3] = 0x0F; // ConFig;

  sysexData[4] = DEVICE_ID;

  sysexData[5] = MAJOR_VERSION; // major version
  sysexData[6] = MINOR_VERSION; // minor version
  sysexData[7] = POINT_VERSION; // point version

  sysexData[8] = MCU;
  sysexData[9] = POT_BIT_RES;

  sysexData[10] = NB_POTS;

  sysexData[11] = cc_mode;

  int offset = 12;

  // pinout
  for(int i = 0; i < NB_POTS; i++) {
    sysexData[offset] = pot[i];
    offset++;
  }
  // ch
  for(int i = 0; i < NB_POTS; i++) {
    sysexData[offset] = ch[i];
    offset++;
  }
  // cc
  for(int i = 0; i < NB_POTS; i++) {
    sysexData[offset] = cc[i];
    offset++;
  }
  // 14-bit cc
  for(int i = 0; i < NB_POTS; i++) {
    sysexData[offset] = cc14[i][0];
    offset++;
    sysexData[offset] = cc14[i][1];
    offset++;
  }
  // nrpn
  for(int i = 0; i < NB_POTS; i++) {
    sysexData[offset] = nrpn[i] >> 7;
    offset++;
    sysexData[offset] = nrpn[i] & 0x7F;
    offset++;
  }

  sysexData[offset] = 0xF7;

  D(Serial.println("Sending c0nFig:"));
  D(printHexArray(sysexData, SYSEX_RESP_LEN));

  usbMIDI.sendSysEx(SYSEX_RESP_LEN, sysexData, false);
  forceMidiWrite = true;
}
