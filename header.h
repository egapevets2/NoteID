// NoteList function x
// inputs a buffer of samples, takes fft, outputs a list of peaks
// subsequent processin can identify notes and harmonics, chords, whatever.

#define ARM_MATH_CM4
#include <arm_math.h>

#define SAMPLE_RATE_HZ  8000             // samples per second.
#define FFT_SIZE  1024              // Size of the FFT.  Realistically can only be at most 256
#define POWER_LED_PIN  13          // Output pin for power LED (pin 13 to use Teensy 3.0's onboard LED).
#define L_upsampleFactor 11
#define numTaps_intrpFIR 5*L_upsampleFactor
#define  MAX_N_PEAKS  15
#define N_SAMPS_TO_INTERP (FFT_SIZE/8)
#define   N_INTERPOLATED_SAMPS (N_SAMPS_TO_INTERP * L_upsampleFactor)



#define USE_RECORDED_CHORD
