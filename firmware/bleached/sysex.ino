

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


  if (sysexData[4] == 0x1f) {
    // 1F = "1nFo" - please send me your current config
    D(Serial.println("Got an 1nFo request"));
    sendCurrentState();

    // also, in 1s time, please send the current state
    // of the controls, so the editor looks nice.
    shouldSendForcedControlUpdate = true;
    sendForcedControlAt = millis() + 1000;
  }

}


const uint8_t SYSEX_RESP_LEN = 10+NB_POTS+NB_POTS+1;

void sendCurrentState() {
  //   0F - "c0nFig" - outputs its config:

  byte sysexData[SYSEX_RESP_LEN];

  sysexData[0] = 0x7d; // manufacturer (set to "prototyping, test, private use and experimentation")
  sysexData[1] = 0x00;
  sysexData[2] = 0x00;

  sysexData[3] = 0x0F; // ConFig;

  sysexData[4] = DEVICE_ID; // Device 01, ie, dev board
  sysexData[5] = MAJOR_VERSION; // major version
  sysexData[6] = MINOR_VERSION; // minor version
  sysexData[7] = POINT_VERSION; // point version

  sysexData[8] = NB_POTS;

  sysexData[9] = MIDI_CHANNEL;
  for(int i = 0; i < NB_POTS; i++) {
    sysexData[10+i] = cc[i];
  }
  for(int i = 0; i < NB_POTS; i++) {
    sysexData[10+NB_POTS+i] = pot[i];
  }

  sysexData[10+NB_POTS+NB_POTS] = 0x7f;


  // 	16 bytes of config flags, notably:
  // 	LED PERMANENT
  // 	LED DATA XFER
  // 	ROTATE (flip+reverse)
  //  i2c MASTER/FOLLOEWR
  //  fadermin LSB
  //  fadermin MSB
  //  fadermax LSB
  //  fadermax MSB
  //  Soft MIDI thru

  // 	16x USBccs
  // 	16x TRSccs
  // 	16x USBchannel
  // 	16x TRS channel

  // So that's 3 for the mfg + 1 for the message + 80 bytes
  // can be done with a simple "read eighty bytes and send them."

  D(Serial.println("Sending this data"));
  D(printHexArray(sysexData, SYSEX_RESP_LEN));

  usbMIDI.sendSysEx(SYSEX_RESP_LEN, sysexData, false);
  forceMidiWrite = true;
}
