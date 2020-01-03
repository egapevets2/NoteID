#include "header.h"

extern float samples[FFT_SIZE * 2];
extern uint32_t sampleCounter;
extern IntervalTimer samplingTimer;
extern float Chord[];

////////////////////////////////////////////////////////////////////////////////
// SAMPLING FUNCTIONS
////////////////////////////////////////////////////////////////////////////////


void samplingCallback() {
  // Read from the ADC and store the sample data
  samples[sampleCounter] = (float32_t)analogRead(AUDIO_INPUT_PIN);
  // Complex FFT functions require a coefficient for the imaginary part of the input.
  // Since we only have real data, set this coefficient to zero.
  samples[sampleCounter+1] = 0.0;
  // Update sample buffer position and stop after the buffer is filled
  sampleCounter += 2;
  if (sampleCounter >= FFT_SIZE*2) {
    samplingTimer.end();
  }
}

void samplingBegin() {
  // Reset sample buffer position and start callback at necessary rate.
  sampleCounter = 0;
  samplingTimer.begin(samplingCallback, 1000000/SAMPLE_RATE_HZ);
}

boolean samplingIsDone() {
  return sampleCounter >= FFT_SIZE*2;
}



uint8_t GetNewSampleBuffer(void)
{
  if (samplingIsDone()) {
    samplingBegin();
    return(NEW_SAMPLE_BUFFER_AVAILABLE);
  }
  else
  {
    return(NEW_SAMPLE_BUFFER_NOT_READY_YET);
  }
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
