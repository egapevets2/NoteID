// NoteList function x
// inputs a buffer of samples, takes fft, outputs a list of peaks
// subsequent processin can identify notes and harmonics, chords, whatever.
#include "header.h"

#define ARM_MATH_CM4
#include <arm_math.h>
#include <WProgram.h>  // at the top of the file



#define USE_RECORDED_CHORD


float FIRtaps[numTaps_intrpFIR] = {
  -0.0000, -0.0000, -0.0001, -0.0003, -0.0006, -0.0011, -0.0017, -0.0025,
  -0.0032, -0.0038, -0.0041, -0.0039, -0.0030, -0.0012, 0.0015, 0.0054,
  0.0103, 0.0162, 0.0230, 0.0305, 0.0383, 0.0461, 0.0536, 0.0604,
  0.0661, 0.0704, 0.0731, 0.0740, 0.0731, 0.0704, 0.0661, 0.0604,
  0.0536, 0.0461, 0.0383, 0.0305, 0.0230, 0.0162, 0.0103, 0.0054,
  0.0015, -0.0012, -0.0030, -0.0039, -0.0041, -0.0038, -0.0032, -0.0025,
  -0.0017, -0.0011, -0.0006, -0.0003, -0.0001, -0.0000, -0.0000
};
extern char done_print_loop;
extern uint32_t i;

arm_fir_interpolate_instance_f32  SinterpObject;
// 4 bytes per float.
// 4*FFT_SIZE*(2+1+11+2+10)=

extern float samples[FFT_SIZE * 2];
extern float magnitudes[FFT_SIZE];
extern float magnitudesINTRP[N_INTERPOLATED_SAMPS];
extern float hanningWindow[FFT_SIZE * 2];
extern float FIRpState[(numTaps_intrpFIR / L_upsampleFactor) + FFT_SIZE - 1];

extern float VpkList[MAX_NUM_PEAKS];
extern uint32_t ipkList[MAX_NUM_PEAKS];
void pkDetect(float v[], uint32_t n, uint32_t Npeaks, float VpkList[], uint32_t ipkList[]);

//-------------------------------------------------------------------------------------
//                                                                        INTERPOLATOR
//                                                                        INIT
void InitializeTheInterpolatorObject(void)
{
  /*
    Parameters
    [in, out]  S points to an instance of the floating - point FIR interpolator structure
    [in]  L upsample factor
    [in]  numTaps number of filter coefficients in the filter
    [in]  pCoeffs points to the filter coefficient buffer
    [in]  pState  points to the state buffer
    [in]  blockSize number of input samples to process per call

    pCoeffs points to the array of filter coefficients stored in time reversed order :
    {b[numTaps - 1], b[numTaps - 2], b[numTaps - 2], ..., b[1], b[0]}
    The length of the filter numTaps must be a multiple of the interpolation factor L.
    pState points to the array of state variables.pState is of length(numTaps / L) + blockSize - 1
    words where blockSize is the number of input samples processed by each call to arm_fir_interpolate_f32().
  */

  arm_fir_interpolate_init_f32(
    &SinterpObject,
    L_upsampleFactor,
    numTaps_intrpFIR,
    FIRtaps,
    FIRpState,
    N_SAMPS_TO_INTERP
  );

}
//-------------------------------------------------------------------------------------
//                                                                        CREATE WINDOW
// applying it to the complex input, which has
// FFT_SIZE complex inputs
// real0, imag0, real1, imag1, ... imag[(FFT_SIZE*2)-1]
void hanning(void)
{
  uint32_t i;
  for (i = 0; i < (FFT_SIZE * 2); i++)
    hanningWindow[i] = 0.5 * (1 - arm_cos_f32(2 * PI * (i) / (FFT_SIZE * 2)));
}

//-------------------------------------------------------------------------------------
//                                                                      ARDUINO SETUP()
// runs one time, at power-up.
void getSpectrum() {
  hanning();
  InitializeTheInterpolatorObject();
#ifdef DEBUG_PRINT_ENABLE
  Serial1.println("__samples");
  for (i = 0; i < (FFT_SIZE * 2); i += 2)
    Serial1.println(samples[i]);
  Serial1.println("END");
#endif
  /*
    _____________________________________
     ORIGINAL SIGNAL, FFT_SIZE points.
             _____                      _        _____                      _
            /     \      __    __      / \      /     \      __    __      / \
           /       \    /  \  /  \    /   \____/       \    /  \  /  \    /   \__
      --------------\--/----\/    \__/------------------\__/----\/----\__/--
  */
  arm_mult_f32(hanningWindow, samples, samples, 2 * FFT_SIZE);

#ifdef DEBUG_PRINT_ENABLE

  Serial1.println("__WindowedSamples");
  for (i = 0; i < (FFT_SIZE * 2); i += 2)
    Serial1.println(samples[i]);
  Serial1.println("END");
#endif


  /*
     _______________
      AFTER WINDOW - tapers the edges down to zero, so it becomes a periodic signal,
             no harsh jumps at beginning or end.
                                        _        _____
                               __      / \      /     \      __
                        ____  /  \    /   \____/       \    /  \  ____
      -----__________--/----\/    \__/------------------\__/----\/----\__/\_____--

  */
  arm_cfft_radix4_instance_f32 fft_inst;
  arm_cfft_radix4_init_f32(&fft_inst, FFT_SIZE, 0, 1);
  arm_cfft_radix4_f32(&fft_inst, samples);
  /*
    __________
    AFTER FFT


    note:  FFT has complex in and out.  they interleave the FFT points like this
    real0,imag0,real1,imag1,real2,imag2, ... real(FFT_SIZE-1),imag(FFT_SIZE-1)
     x[0],  x[1], x[2],              ...                     x[2*FFT_SIZE - 1]

                _/\      __
                |  \    /  \
    Real  ______/   \__/    \_____________________________


                 ___      ____
    Imaginary___/   \    /    \_____________________________
                     \   |
                      \__|

  */

  arm_cmplx_mag_f32(samples, magnitudes, N_SAMPS_TO_INTERP);

  /*
      ________________
      AFTER cmplx_mag
      cmplx mag takes in 2*FFT_size numbers, which is FFT_SIZE complex values,
      and it outputs the FFT_SIZE magnitudes of those.

                    /\             |     we over-sampled so
                    | |   __       |---> don't care beyond here
      magnitude ____/ \__/  \______|___________________________
                                   |
               N_SAMPS_TO_INTERP   |---> guitar spectrum not out here
  */

  arm_fir_interpolate_f32(&SinterpObject, magnitudes, magnitudesINTRP, N_SAMPS_TO_INTERP);

  /*
      _____________________
      AFTER interpolation
         are the coarse points from the complex magnitude above, then this
       fills in smoothly between the *'s



    first step:  Put zeros between the samples you got from the
    cmplx_mag

              *               *   *
              |   *           |   |
              |   |           |   |
       ooo*ooo|ooo|ooo*ooo*ooo|ooo|ooo


       Second Step:
       do a low-pass filter on that.
       it is an FIR filter (Finite Impulse Response)
       similar to a running average, like
       "take the average of the last 16 samples"
               _
              / \_            ____
                  |          *
             /    *        _/      \
            /      \      /         \
    __*____*        \*___*
  */




}
