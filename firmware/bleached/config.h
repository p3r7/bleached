

/*
 * firmware metadata
 */

int MAJOR_VERSION = 0x02;
int MINOR_VERSION = 0x00;
int POINT_VERSION = 0x00;


/*
 * device metadata
 */

#if defined(__MKL26Z64__) || defined(__MK20DX128__) || defined(_LC_DEBUG)
const int DEVICE_ID = 0x03; // 16nLC, do not change, needed by editor
#else
const int DEVICE_ID = 0x02; // 16n, do not change, needed by editor
#endif


// Map MIDI CC channels to knobs numbered left to right.
#define CC01  102
#define CC02  103
#define CC03  104
#define CC04  105
#define CC05  106
#define CC06  107
#define CC07  108

// Map the TeensyLC pins to each potentiometer numbered left to right.
#define POT01 0
#define POT02 1
#define POT03 2
#define POT04 3
#define POT05 4
#define POT06 5
#define POT07 6

// Use this MIDI channel.
#define MIDI_CHANNEL 1

// Send MIDI CC messages for all 7 knobs after the main loop runs this many times.
// This prevents having to twiddle the knobs to update the receiving end.
// 10,000 loops is roughly 10 seconds.
//#define LOOPS_PER_REFRESH 10000

// potentiometer read parameters
#define POT_BIT_RES         10 // 10 works, 7-16 is valid
#define POT_NUM_READS       32 // 32 works

#define NB_POTS 7

// an array of potentiometer pins
uint8_t pot[7] = {POT01,
                  POT02, POT03, POT04, POT05, POT06, POT07
};

// an array of CC numbers
uint8_t cc[7] = { CC01,
                  CC02, CC03, CC04, CC05, CC06, CC07
};
