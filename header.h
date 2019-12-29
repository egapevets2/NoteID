
#define ARM_MATH_CM4
#include <arm_math.h>

#define SAMPLE_RATE_HZ  8000             // samples per second.
#define FFT_SIZE  1024              // Size of the FFT.  Realistically can only be at most 256
#define POWER_LED_PIN  13          // Output pin for power LED (pin 13 to use Teensy 3.0's onboard LED).

/*
  an interpolated sample is the weighted average of this sample,
  and 2 samples on either side.  the weighted average meaning
  it is a filter with taps FIRtaps[]
  you get 11 interpolated samples between each input sample.
*/
#define L_upsampleFactor 11
#define N_SAMPS_TO_LP_INTERP 5
#define numTaps_intrpFIR N_SAMPS_TO_LP_INTERP*L_upsampleFactor
/*
  interpolating puts a delay of this many\
  samples.  so, this sample index is where the
  interpolated zero hz is....this is assuming both
  L_upsampleFactor and  N_SAMPS_TO_LP_INTERP are odd
*/
#define I_INTERP_0HZ ((numTaps_intrpFIR-1)/2)

#define N_SAMPS_TO_INTERP (FFT_SIZE/8)
#define   N_INTERPOLATED_SAMPS (N_SAMPS_TO_INTERP * L_upsampleFactor)



#define   QUIET_THRESH  0.01  // notes must be this fraction of vMax

#define   NUM_NOTE_BINS 49

#define USE_RECORDED_CHORD


#define BIN_WIDTH 1.5




/*    BIN DEFINITION
  Each fret is a half-step.
  there is a bin for half-step

  center
  .
  left     .  right
  |      .    |
  | ---o-.----|----  dB_level
  |   /.\.    |
  |  / . |    |
  | /  . .\   |
  |/   . . \  |
  |    .    \ |
  /|    .     \
  actual
*/
typedef struct BIN_T
{
  float Left;
  float Right;
  float Center;
  float Actual;
  int playing;
  float vPeak;
  char NoteName;
  int StringNum;
} bin_t;


typedef struct PEAK_T
{
  int i;
  float vPeak;
} peak_t;



#define SILENT  1
#define GOING_UP  2
#define GOING_DOWN  3
#define RISE_FALL_COUNT_REQUIREMENT  5

#define  MAX_NUM_PEAKS  15
