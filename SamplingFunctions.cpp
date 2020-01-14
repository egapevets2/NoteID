#include "header.h"



extern IntervalTimer samplingTimer;
extern float Chord[];

extern uint32_t sampleCounter;
extern uint32_t iCirc;
extern uint32_t iNxtRun;
extern uint32_t iTransfer;
extern int ADC_circ_buffer[];



extern int ISRtimeStamp[FFT_SIZE];
extern int samplesTS[FFT_SIZE];

extern float samples[];
extern char ADC_semaphore;

extern uint32_t Old_T_isr;
extern char ISR_state;

int MissedSamples(int deltaMicroseconds)
{



  return (1);
}



////////////////////////////////////////////////////////////////////////////////
// SAMPLING FUNCTIONS
////////////////////////////////////////////////////////////////////////////////
// NFFT                  - 1024
// iCirc                 - increments every interrupt, wraps NFFT
// ADC_circ_buffer[FFT_SIZE] - adc puts a sample in here every interrupt
// ADC_semaphore         - gets set once you get a new SAMPS_PER_RUN batch
// samples[2*FFT_SIZE]        - this is the working copy of the samples, that the alg runs on.
//
// loop() does the algorithm, and writes the led string, and then sets the semaphore
// so this ISR gives the latest 1024 samples.  The ideal is that loop() cycles at faster
// than 24 FPS (frames per second), so that the leds are not flashy, but are smoothly animated.
//
// normally each T_isr is exactly 125us from the previous.  But, if something in loop()
// disables interrupts, you will skip this ISR:  for example neopixels turn off interrupts.
// So this includes a sync state machine so there are no jumps in the samples.
// i can just repeat samples to fake it out, since the interrupts are off for only a few ms.
//


#define IN_SYNC 0
#define WAIT_FOR_SYNC 1
#define RESYNC 2

void samplingCallback() {

  uint32_t T_isr = micros();

  int delta_T_isr = T_isr - Old_T_isr;


  switch (ISR_state)
  {
    case IN_SYNC:
      if (abs(delta_T_isr) > DELTA_T_ISR_TOLERANCE)
      {
        ISR_state = WAIT_FOR_SYNC;

      }
      else
      {
        //  ISRtimeStamp[iCirc]=micros();//was for debug
        ADC_circ_buffer[iCirc] = analogRead(AUDIO_INPUT_PIN);
        if (++iCirc >= FFT_SIZE) iCirc = 0;
      }
      break;


    // ditch one sample when you come back to sampling.
    case WAIT_FOR_SYNC:
      ISR_state = RESYNC;
      break;

    case RESYNC:
      if (abs(delta_T_isr) > DELTA_T_ISR_TOLERANCE)
      {
        ISR_state = WAIT_FOR_SYNC;

      }
      else
      {
        int filler = analogRead(AUDIO_INPUT_PIN);
        int N = MissedSamples(delta_T_isr);
        for (int i = 0; i < N; i++)
        {
          ADC_circ_buffer[iCirc] = filler;
          if (++iCirc >= FFT_SIZE) iCirc = 0;
        }
        ISR_state = IN_SYNC;

      }
      break;

      
    default:
      ISR_state = IN_SYNC;
      break;
  }



  Old_T_isr = T_isr;




  if ((ADC_semaphore == 1) && (ISR_state == IN_SYNC))
  {
    // iCirc indexes the OLDEST sample.
    // starting with the oldest, transfer them all into the working buffer,
    // and put iCirc back when you are done, so the next adc sample
    // goes overwrites the oldest.
    sampleCounter = 0;
    for (iTransfer = 0; iTransfer < FFT_SIZE; iTransfer++)
    {
      samples[sampleCounter] = (float32_t)ADC_circ_buffer[iCirc];
      // samplesTS[iTransfer] = ISRtimeStamp[iCirc];//was for debug

      if (++iCirc == FFT_SIZE) iCirc = 0;
      // Complex FFT functions require a coefficient for the imaginary part of the input.
      // Since we only have real data, set this coefficient to zero.
      samples[sampleCounter + 1] = 0.0;
      sampleCounter += 2;
    }

    ADC_semaphore = 0; // samples[] is ready to run

  }
}

void samplingBegin() {
  // Reset sample buffer position and start callback at necessary rate.
  sampleCounter = 0;
  iNxtRun = 0;
  iCirc = 0;
  samplingTimer.begin(samplingCallback, 1000000 / SAMPLE_RATE_HZ);
}


//-------------------------------------------------------------------------------------
//                                                                SIMULATE GUITAR INPUT
void SimInput(void)
{
  uint32_t sampleCounter, j;
  j = 0;
  for (sampleCounter = 0; sampleCounter < (FFT_SIZE * 2); sampleCounter += 2)
  {
    // POWER CHORD HAS ROOT AND 7TH.
    // open 6th string is 83hz
    // seventh is 83*2^(7/12)=124HZ
    // 83 hz at 4ksps is 50 samples per cycle
    // t=iSamples/SamplesPerSecond=Seconds
    // THE 0.5 is because sample counter goes up by 2, so it is sample number times 2
    float f0 = 83;
    float n1, n2;

    n1 = 0.0;
    n2 = 7.0;

    samples[sampleCounter] = sin(6.28 * f0 * pow(2.0, n1 / 12.0) * ((float)sampleCounter) * 0.5 / 4000.0) +
                             sin(6.28 * f0 * pow(2.0, n2 / 12.0) * ((float)sampleCounter) * 0.5 / 4000.0);

#ifdef USE_RECORDED_CHORD
    samples[sampleCounter] = Chord[j++];
#endif

    // Complex FFT functions require a coefficient for the imaginary part of the input.
    // Since we only have real data, set this coefficient to zero.
    samples[sampleCounter + 1] = 0.0;
  }

}
