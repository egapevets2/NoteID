#include "header.h"





// undercase indicates sharp...
// starts at E because that is the "root note"  - the lowest note,
// and every note thereafter goes up a half a step.
// there are 5 notes per string,
// but 3rd is a special case. that's why I explicitly
// make this table instead of making a formula.
char ScaleOfNotes[NUM_NOTE_BINS] = {
  'E', 'F', 'f', 'G', 'g',    //6
  'A', 'a', 'B', 'C', 'c',    //5
  'D', 'd', 'E', 'F', 'f',    //4
  'G', 'g', 'A', 'a',         //3
  'B', 'C', 'c', 'D', 'd',    //2
  'E', 'F', 'f', 'G', 'g',    //1
  'A', 'a', 'B', 'C', 'c',    //1
  'D', 'd', 'E', 'F', 'f',    //beyond 12th
  'G', 'g', 'A', 'a',         //1
  'B', 'C', 'c', 'D', 'd',    //1
  'E'
};
char ScaleOfStrings[NUM_NOTE_BINS] = {
  6, 6, 6, 6, 6,
  5, 5, 5, 5, 5,
  4, 4, 4, 4, 4,
  3, 3, 3, 3,
  2, 2, 2, 2, 2,
  1, 1, 1, 1, 1,
  1, 1, 1, 1, 1,
  1, 1, 1, 1, 1,
  1, 1, 1, 1,
  1, 1, 1, 1, 1,
  1
};


float i2Freq(int i)
{
  return (((float)i) * (SAMPLE_RATE_HZ / ((float)FFT_SIZE)));
}

float INTERPi2Freq(int i)
{
  return (((float)(i - I_INTERP_0HZ)) * (SAMPLE_RATE_HZ / ((float)(FFT_SIZE * L_upsampleFactor))));
}
int Freq2INTERPi(float f)
{
  return (f/(SAMPLE_RATE_HZ / ((float)(FFT_SIZE * L_upsampleFactor)))+I_INTERP_0HZ);
}

void NoteBinBuider(int LowE, float BinWidth, bin_t bin[])
{
  int i;
  // these are the fractional notes on either side of the ideal note.
  float LeftHalfStepIndex;
  float RightHalfStepIndex;
  float CenterHalfStepIndex;
  float RootNote = (float)LowE;
  for (i = 0; i < NUM_NOTE_BINS; i++)
  {
    LeftHalfStepIndex = ((float)i) - (BinWidth / 2.0);
    RightHalfStepIndex = ((float)i) + (BinWidth / 2.0);
    CenterHalfStepIndex = ((float)i);

    bin[i].Left = RootNote * pow(2.0, (LeftHalfStepIndex / 12.0));
    bin[i].Right = RootNote * pow(2.0, (RightHalfStepIndex / 12.0));
    bin[i].Center = RootNote * pow(2.0, (CenterHalfStepIndex / 12.0));

    bin[i].NoteName = ScaleOfNotes[i];
    bin[i].StringNum = ScaleOfStrings[i];
  }
}

// Inputs list of spectral peak locations,
// and bin Left and Right Sides.
// For each bin[i], it outputs NoteList[i] = 1 if the note is in there.
// and also puts in the level in dB
//
// Both the input NoteList[]
void NoteID(peak_t pk[], bin_t bin[])
{
  int iPeak = 0;
  for (int iBin = 0; iBin < NUM_NOTE_BINS; iBin++)
  {
    // If there is a spectral peak within the bin, then that bin is has a
    // note playing.

    float freq = INTERPi2Freq(pk[iPeak].i);
    
    if ((freq >= bin[iBin].Left) &&
        (freq <= bin[iBin].Right))
    {
      bin[iBin].playing = 1;
      bin[iBin].vPeak = pk[iPeak].vPeak;
      bin[iBin].Actual = freq;
      iPeak++;
    }
    else
    {
      bin[iBin].playing = 0;
    }
  }

}
