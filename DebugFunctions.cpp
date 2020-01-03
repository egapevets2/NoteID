#include "header.h"
extern float samples[];
extern float magnitudes[];
extern float magnitudesINTRP[];
extern float hanningWindow[];
extern float FIRpState[];
extern float vMax;
extern float LowE;

extern bin_t bins[];
extern peak_t peaks[];

void debugPrintInSetup(void)
{
  int i;
  //#ifdef DEBUG_PRINT_ENABLE
  Serial1.println("__SAMPLE_RATE_HZ");
  Serial1.println(SAMPLE_RATE_HZ);
  Serial1.println("END");
  Serial1.println("__FFT_SIZE");
  Serial1.println(FFT_SIZE);
  Serial1.println("END");
  Serial1.println("__L_upsampleFactor");
  Serial1.println(L_upsampleFactor);
  Serial1.println("END");
  Serial1.println("__numTaps_intrpFIR");
  Serial1.println(numTaps_intrpFIR);
  Serial1.println("END");
  Serial1.println("__MAX_NUM_PEAKS");
  Serial1.println(MAX_NUM_PEAKS);
  Serial1.println("END");
  Serial1.println("__N_SAMPS_TO_INTERP");
  Serial1.println(N_SAMPS_TO_INTERP);
  Serial1.println("END");
  Serial1.println("__N_INTERPOLATED_SAMPS");
  Serial1.println(N_INTERPOLATED_SAMPS);
  Serial1.println("END");
  Serial1.println("__I_INTERP_0HZ");
  Serial1.println(I_INTERP_0HZ);
  Serial1.println("END");
  Serial1.println("__QUIET_THRESH");
  Serial1.println(QUIET_THRESH);
  Serial1.println("END");
  Serial1.println("__NUM_NOTE_BINS");
  Serial1.println(NUM_NOTE_BINS);
  Serial1.println("END");
  Serial1.println("__Left");
  for (i = 0; i < NUM_NOTE_BINS; i++)    Serial1.println(bins[i].Left);
  Serial1.println("END");
  Serial1.println("__Right");
  for (i = 0; i < NUM_NOTE_BINS; i++)    Serial1.println(bins[i].Right);
  Serial1.println("END");
  Serial1.println("__Center");
  for (i = 0; i < NUM_NOTE_BINS; i++)    Serial1.println(bins[i].Center);
  Serial1.println("END");
  Serial1.println("__Actual");
  for (i = 0; i < NUM_NOTE_BINS; i++)    Serial1.println(bins[i].Actual);
  Serial1.println("END");
  Serial1.println("__playing");
  for (i = 0; i < NUM_NOTE_BINS; i++)    Serial1.println(bins[i].playing);
  Serial1.println("END");
  Serial1.println("__dB_level");
  for (i = 0; i < NUM_NOTE_BINS; i++)    Serial1.println(bins[i].vPeak);
  Serial1.println("END");
  Serial1.println("__NoteName");
  for (i = 0; i < NUM_NOTE_BINS; i++)    Serial1.println(bins[i].NoteName);
  Serial1.println("END");
  Serial1.println("__String");
  for (i = 0; i < NUM_NOTE_BINS; i++)    Serial1.println(bins[i].StringNum);
  Serial1.println("END");


  Serial1.println("__vMax");
  Serial1.println(vMax);
  Serial1.println("END");


  Serial1.println("__magnitudes");
  for (i = 0; i < N_SAMPS_TO_INTERP; i++)    Serial1.println(magnitudes[i]);
  Serial1.println("END");

  Serial1.println("__magnitudesINTRP");
  for (i = 0; i < N_INTERPOLATED_SAMPS; i++)    Serial1.println(magnitudesINTRP[i]);
  Serial1.println("END");


  Serial1.println("__VpkList");
  for (i = 0; i < MAX_NUM_PEAKS; i++)    Serial1.println(peaks[i].vPeak);
  Serial1.println("END");


  Serial1.println("__ipkList");
  for (i = 0; i < MAX_NUM_PEAKS; i++)    Serial1.println(peaks[i].i);
  Serial1.println("END");


  Serial1.println("================================== D O N E");
  //#endif
}
