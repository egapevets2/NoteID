

#include "header.h"

extern float samples[];
extern float magnitudes[];
extern float magnitudesINTRP[];
extern float hanningWindow[];
extern float FIRpState[];
extern float vMax;
extern float LowE;
extern char RealtimeToggle;

extern bin_t bins[];
extern peak_t peaks[];
extern Adafruit_NeoPixel *pixels;


void LED_animation(void)
{
  uint8_t red, green, blue;












  for (int i = 0; i < NUM_NOTE_BINS; i++) {



    if ((rand() & 0x3f) == 0)bins[i].playing = 1;
    else bins[i].playing = 0;
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    if (bins[i].playing == 1)
    {
      red = 150; green = 0; blue = 0;
    }
    else
    {
      red = 0;
      green = 0;
      blue = 5;
    }
    pixels->setPixelColor(i, pixels->Color(red, green, blue));
  }
  // for 49 pixels, I find this takes like 580, or 1580 microseconds.
  //
    unsigned long  t0a = micros();

    unsigned long  t0 = micros();
  pixels->show(); // This sends the updated pixel color to the hardware.
    unsigned long  t1 = micros();

    unsigned long  t1a = micros();

  Serial1.print("Animation Elapsed Time = ");
  Serial1.println(t1 - t0);

  Serial1.print("Animation Elapsed Time a = ");
  Serial1.println(t1a - t0a);
}


void RealtimeToggleIndicator(void)
{
  if (RealtimeToggle == 0)
  {
    digitalWrite(POWER_LED_PIN, HIGH);
    RealtimeToggle = 1;
  }
  else
  {
    digitalWrite(POWER_LED_PIN, LOW);
    RealtimeToggle = 0;
  }

}
