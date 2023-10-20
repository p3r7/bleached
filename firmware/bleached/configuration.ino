

// ------------------------------------------------------------------------
// default vals

const uint8_t defaultChannels[] = { 1, 1, 1, 1,
                                    1, 1, 1 };

const uint8_t defaultModes[] = { CC, CC, CC, CC,
                                 CC, CC, CC };

const uint8_t defaultCCs[] = { CC01, CC02, CC03, CC04,
                               CC05, CC06, CC07 };

const uint8_t default14BitCCs[][2] = { ECC01, ECC02, ECC03, ECC04,
                                       ECC05, ECC06, ECC07 };

const uint16_t defaultNRPNs[] = { NRPN1, NRPN2, NRPN3, NRPN4,
                                  NRPN5, NRPN6, NRPN7 };


// ------------------------------------------------------------------------
// eeprom

void checkDefaultSettings() {
  int firstByte = EEPROM.read(0x00);
  if(firstByte > 0x01) {
    D(Serial.println("First Byte is > 0x01, probably needs initialising"));
    initializeFactorySettings();
  } else {
    D(Serial.print("First Byte is set to: "));
    D(printHex(firstByte));
    D(Serial.println());
    byte buffer[80];
    readEEPROMArray(0, buffer, 80);
    D(Serial.println("Config found:"));
    D(printHexArray(buffer, 80));
  }
}

 void initializeFactorySettings() {
  // set default config flags (LED ON, LED DATA, ROTATE, etc)
  // fadermin/max are based on "works for me" for twra2. Your mileage may vary.
  EEPROM.write(0,1);  // LED ON
  EEPROM.write(1,1);  // LED DATA

  int offset = 2;

  for(int i = offset; i < 16; i ++) {
    EEPROM.write(i,0);  // blank remaining config slots.
    offset++;
  }

  // ch
  for(int i = 0; i < NB_POTS; i++) {
    EEPROM.write(offset, defaultChannels[i]);
    offset++;
  }

  // mode
  for(int i = 0; i < NB_POTS; i++) {
    EEPROM.write(offset, defaultModes[i]);
    offset++;
  }

  // CC
  for(int i = 0; i < NB_POTS; i++) {
    EEPROM.write(offset, defaultCCs[i]);
    offset++;
  }

  // 14-bit CC
  for(int i = 0; i < NB_POTS; i++) {
    EEPROM.write(offset, default14BitCCs[i][0]);
    offset++;
    EEPROM.write(offset, default14BitCCs[i][1]);
    offset++;
  }

  // NRPN
  for(int i = 0; i < NB_POTS; i++) {
    EEPROM.write(offset, defaultNRPNs[i] >> 7);
    offset++;
    EEPROM.write(offset, defaultNRPNs[i] & 0x7F);
    offset++;
  }

  // serial dump that config.
  byte buffer[80];
  readEEPROMArray(0,buffer,80);
  D(Serial.println("Config Instantiated."));
  D(printHexArray(buffer, 80));
}

// void loadSettingsFromEEPROM() {
//   // load usb channels
//   for(int i = 0; i < channelCount; i++) {
//     int baseAddress = 16;
//     int readAddress = baseAddress + i;
//     usbChannels[i] = EEPROM.read(readAddress);
//   }

//   D(Serial.println("USB Channels loaded:"));
//   D(printIntArray(usbChannels,channelCount));

//   // load TRS channels
//   for(int i = 0; i < channelCount; i++) {
//     int baseAddress = 32;
//     int readAddress = baseAddress + i;
//     trsChannels[i] = EEPROM.read(readAddress);
//   }

//   D(Serial.println("TRS Channels loaded:"));
//   D(printIntArray(trsChannels,channelCount));

//   // load USB ccs
//   for(int i = 0; i < channelCount; i++) {
//     int baseAddress = 48;
//     int readAddress = baseAddress + i;
//     usbCCs[i] = EEPROM.read(readAddress);
//   }

//   D(Serial.println("USB CCs loaded:"));
//   D(printIntArray(usbCCs,channelCount));


//   // load TRS ccs
//   for(int i = 0; i < channelCount; i++) {
//     int baseAddress = 64;
//     int readAddress = baseAddress + i;
//     trsCCs[i] = EEPROM.read(readAddress);
//   }

//   D(Serial.println("TRS CCs loaded:"));
//   D(printIntArray(trsCCs,channelCount));

//   // load other config
//   ledOn = EEPROM.read(0);
//   ledFlash = EEPROM.read(1);
//   flip = EEPROM.read(2);
//   midiThru = EEPROM.read(8);

//   // i2cMaster only read at startup

//   int faderminLSB = EEPROM.read(4);
//   int faderminMSB = EEPROM.read(5);

//   D(Serial.print ("Setting fadermin to "));
//   D(Serial.println((faderminMSB << 7) + faderminLSB));
//   faderMin = (faderminMSB << 7) + faderminLSB;

//   int fadermaxLSB = EEPROM.read(6);
//   int fadermaxMSB = EEPROM.read(7);

//   D(Serial.print ("Setting fadermax to "));
//   D(Serial.println((fadermaxMSB << 7) + fadermaxLSB));
//   faderMax = (fadermaxMSB << 7) + fadermaxLSB;
// }
