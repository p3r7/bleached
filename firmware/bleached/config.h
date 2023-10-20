
#pragma once


// ------------------------------------------------------------------------
// DEBUG FLAG

#define DEBUG 1


// ------------------------------------------------------------------------
// fw version

int MAJOR_VERSION = 0x02;
int MINOR_VERSION = 0x01;
int POINT_VERSION = 0x00;


// ------------------------------------------------------------------------
// sysex

const int DEVICE_ID = 0x07; // bleached

#if defined(__MKL26Z64__) || defined(__MK20DX128__) || defined(_LC_DEBUG)
#define MCU 0x01 // LC
#else
#define MCU 0x02 // 3.2
#endif


// ------------------------------------------------------------------------
// adc

// potentiometer read parameters
#define RESOLUTION          8192

// #define POT_BIT_RES         10 // 10 works, 7-16 is valid
#if defined(__MKL26Z64__) || defined(__MK20DX128__) || defined(_LC_DEBUG)
#define POT_BIT_RES         12
#else
#define POT_BIT_RES         13
#endif

#define POT_NUM_READS       32 // 32 works

// Prevent jitter when reading the potentiometers.
// Higher value is less chance of jitter but also less precision.
const uint8_t nbrhd = 5;


// ------------------------------------------------------------------------
// pot <-> pinout

#define POT01 0
#define POT02 1
#define POT03 2
#define POT04 3
#define POT05 4
#define POT06 5
#define POT07 6


// ------------------------------------------------------------------------
// conf

// Map MIDI CC channels to knobs numbered left to right.
#define CC01  102
#define CC02  103
#define CC03  104
#define CC04  105
#define CC05  106
#define CC06  107
#define CC07  108

// Likewise for extended range (14 bit) CC
#define ECC01  {70, 102}
#define ECC02  {71, 103}
#define ECC03  {72, 104}
#define ECC04  {73, 105}
#define ECC05  {74, 106}
#define ECC06  {75, 107}
#define ECC07  {76, 108}

// & NRPN
#define NRPN1  1
#define NRPN2  2
#define NRPN3  3
#define NRPN4  4
#define NRPN5  5
#define NRPN6  6
#define NRPN7  7

// Use this MIDI channel.
#define MIDI_CHANNEL 1

// Send MIDI CC messages for all 7 knobs after the main loop runs this many times.
// This prevents having to twiddle the knobs to update the receiving end.
// 10,000 loops is roughly 10 seconds.
//#define LOOPS_PER_REFRESH 10000

#define NB_POTS 7
// #define NB_POTS 6

// an array of potentiometer pins
uint8_t pot[7] = {POT01, POT02, POT03, POT04,
                  POT05, POT06, POT07
};
