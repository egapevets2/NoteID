

#include "header.h"





#define Silent  1
#define GoingUp  2
#define GoingDown  3
#define RiseFallCountRequirement  5
#include <SoftwareSerial.h>

void pkDetect(float v[], uint32_t n, uint32_t Npeaks, float VpkList[], uint32_t ipkList[])
{

  int Counter;
  int pkState;
  float k2Min = 0.01;  // notes must be this fraction of vMax
  uint32_t i = 1;
  uint32_t pkNum = 0;
  float thrMin, vMax;
  uint32_t iPeak;
  float vPeak;
  // Maximum of the whole spectrum - everything else is relative to this.
  arm_max_f32(v,
              n,
              &vMax,
              &iPeak
             );
  thrMin = k2Min * vMax;

  for (pkNum = 0; pkNum < Npeaks; pkNum++)
  {
    VpkList[pkNum] = 1;
    ipkList[pkNum] = 1;

  }
  pkNum = 0;

  pkState = Silent;
  while ((i < n) && (pkNum < Npeaks))
  {
    i = i + 1;
    switch (pkState)
    {
      case Silent:
        if (v[i] > thrMin)
        {
          pkState = GoingUp;
          Counter = 0;
        }
        break;

      case GoingUp:
        if (v[i] < thrMin)
        {
          pkState = Silent;
        }
        else
        {
          if (v[i] > v[i - 1])
          {
            Counter = Counter + 1;// still going up...
          }
          else // came to the peak or a flat top.
          {
            if (Counter > RiseFallCountRequirement)
            {
              pkState = GoingDown;
              iPeak = i;
              vPeak = v[iPeak];
              Counter = 0;
            }
            else // did not make it smoothly down.
            {
              pkState = Silent;
            }
          }
        }
        break;

      case GoingDown:
        if (v[i] < thrMin)
        {
          pkState = Silent;
        }
        else if (v[i] < v[i - 1])
        {
          Counter = Counter + 1;// still going up...
        }
        else // started back up too soon.
        {
          pkState = Silent;
        }

        // If you have been in this state,
        // consistent downwards long enough, then you have
        // qualified the peak, save it and go to next one.
        if (Counter > RiseFallCountRequirement)
        {
          pkState = Silent;

          VpkList[pkNum] = vPeak;
          ipkList[pkNum] = iPeak;
          pkNum++;
        }
        break;
    }
  }
}
