// NoteList function x
// inputs a buffer of samples, takes fft, outputs a list of peaks
// subsequent processin can identify notes and harmonics, chords, whatever.


#include "header.h"
#include "Chord.h"








// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            6


// Color order, for more information see https://github.com/adafruit/Adafruit_NeoPixel/blob/master/Adafruit_NeoPixel.h
uint8_t colorOrder = 0x52; //or just use NEO_GBR

// Define new pointer for NeoPixel
Adafruit_NeoPixel *pixels;


int delayval = 100; // delay for half a second














char done_print_loop = 1;
uint32_t i;



float samples[FFT_SIZE * 2];
float magnitudes[FFT_SIZE];
float magnitudesINTRP[N_INTERPOLATED_SAMPS];
float hanningWindow[FFT_SIZE * 2];
float FIRpState[(numTaps_intrpFIR / L_upsampleFactor) + FFT_SIZE - 1];
float vMax;
float LowE = 83.0;

void pkDetect(float v[], uint32_t n, uint32_t Npeaks, peak_t pkLst[]);
void getSpectrum() ;

void NoteBinBuider(int LowE, float BinWidth, bin_t bin[]);
void NoteID(peak_t pk[], bin_t bin[]);



bin_t bins[NUM_NOTE_BINS];
peak_t peaks[MAX_NUM_PEAKS];

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
//-------------------------------------------------------------------------------------
//                                                                      ARDUINO SETUP()
// runs one time, at power-up.
void setup() {
  Serial1.begin(115200);

  Serial1.println("Guitar Spectral Peak Finder Test");

  // Turn on the power indicator LED.
  pinMode(POWER_LED_PIN, OUTPUT);
  digitalWrite(POWER_LED_PIN, HIGH);

  digitalWrite(POWER_LED_PIN, LOW);



  // Here is a good place to read numPixel & colorOrder from EEPROM or what ever.
  // create a new NeoPixel instance with new values
  pixels = new Adafruit_NeoPixel(LED_STRAND_LENGTH, PIN, colorOrder);
  pixels->begin(); // This initializes the NeoPixel library.



  NoteBinBuider(LowE, BIN_WIDTH, bins);



  SimInput();



  Serial1.println("start processing the buffer");
  unsigned long StartTime = micros();
  getSpectrum() ;

  pkDetect(magnitudesINTRP, N_INTERPOLATED_SAMPS, MAX_NUM_PEAKS, peaks);

  NoteID(peaks, bins);

  unsigned long EndTime = micros();




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

  Serial1.print("Time to convert buffer to note bins=");
  Serial1.print((EndTime - StartTime) / 1000);
  Serial1.println(" milliseconds");
  Serial1.println("================================== D O N E");
  //#endif



}
//-------------------------------------------------------------------------------------
//                                                                      ARDUINO LOOP()
// a while(1==1) loop.  you must allow it to
// run.  if you kill it, by putting in your
// own while(1==1), you block the serial port
//
void loop() {



  // For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.

  for (int i = 0; i < NUM_NOTE_BINS; i++) {

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels->setPixelColor(i, pixels->Color(0, 150, 0)); // Moderately bright green color.

    pixels->show(); // This sends the updated pixel color to the hardware.

    delay(delayval); // Delay for a period of time (in milliseconds).

  }

  for (int i = 0; i < NUM_NOTE_BINS; i++) {

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels->setPixelColor(i, pixels->Color(150, 0, 0)); // Moderately bright green color.

    pixels->show(); // This sends the updated pixel color to the hardware.

    delay(delayval); // Delay for a period of time (in milliseconds).

  }

  for (int i = 0; i < NUM_NOTE_BINS; i++) {

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels->setPixelColor(i, pixels->Color(0, 0, 150)); // Moderately bright green color.

    pixels->show(); // This sends the updated pixel color to the hardware.

    delay(delayval); // Delay for a period of time (in milliseconds).

  }





  digitalWrite(POWER_LED_PIN, HIGH);
  delay(250);
  digitalWrite(POWER_LED_PIN, LOW);
  delay(250);
  digitalWrite(POWER_LED_PIN, HIGH);
  delay(1000);
  digitalWrite(POWER_LED_PIN, LOW);
  delay(1000);
  Serial1.print(i++);
  Serial1.println("================================== D O N E");


}
