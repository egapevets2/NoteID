
#define ARM_MATH_CM4
#include <arm_math.h>
#include <Adafruit_NeoPixel.h>

//#define DEBUG_PRINT_ENABLE




#define SAMPLE_RATE_HZ  8000             // samples per second.
#define FFT_SIZE  1024              // 
#define SAMPS_PER_RUN 256           // every time you this many samples, run the algorithm on the last FFT_SIZE samples
#define POWER_LED_PIN  13          // Output pin for power LED (pin 13 to use Teensy 3.0's onboard LED).
#define AUDIO_INPUT_PIN  14        // Input ADC pin for audio data.
#define ANALOG_READ_RESOLUTION  10 // Bits of resolution for the ADC.
#define ANALOG_READ_AVERAGING  16  // Number of samples to average with each ADC reading.


#define DELTA_T_ISR_TOLERANCE 30 // micro seconds


#define LED_TEST_DELAYVAL 20

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
#define LED_STRAND_LENGTH 300
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


#define NEW_SAMPLE_BUFFER_AVAILABLE 1
#define NEW_SAMPLE_BUFFER_NOT_READY_YET 0

#define SILENT  1
#define GOING_UP  2
#define GOING_DOWN  3
#define RISE_FALL_COUNT_REQUIREMENT  5

#define  MAX_NUM_PEAKS  15
