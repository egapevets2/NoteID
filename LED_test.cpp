

#include "header.h"


extern Adafruit_NeoPixel *pixels;


void LED_test(void)
{



  // For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.

  for (int i = 0; i < NUM_NOTE_BINS; i++) {

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels->setPixelColor(i, pixels->Color(0, 150, 0)); // Moderately bright green color.

    pixels->show(); // This sends the updated pixel color to the hardware.

    delay(LED_TEST_DELAYVAL); // Delay for a period of time (in milliseconds).

  }

  for (int i = 0; i < NUM_NOTE_BINS; i++) {

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels->setPixelColor(i, pixels->Color(150, 0, 0)); // Moderately bright green color.

    pixels->show(); // This sends the updated pixel color to the hardware.

    delay(LED_TEST_DELAYVAL); // Delay for a period of time (in milliseconds).

  }

  for (int i = 0; i < NUM_NOTE_BINS; i++) {

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels->setPixelColor(i, pixels->Color(0, 0, 150)); // Moderately bright green color.

    pixels->show(); // This sends the updated pixel color to the hardware.

    delay(LED_TEST_DELAYVAL); // Delay for a period of time (in milliseconds).

  }





  digitalWrite(POWER_LED_PIN, HIGH);
  delay(250);
  digitalWrite(POWER_LED_PIN, LOW);
  delay(250);
  digitalWrite(POWER_LED_PIN, HIGH);
  delay(1000);
  digitalWrite(POWER_LED_PIN, LOW);
  delay(1000);
  Serial1.println("================================== D O N E");
   
}
