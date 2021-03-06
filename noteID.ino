// NoteList function x
// inputs a buffer of samples, takes fft, outputs a list of peaks
// subsequent processin can identify notes and harmonics, chords, whatever.


#include "header.h"
#include "Chord.h"


// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            6


// Color order, for more information see https://github.com/adafruit/Adafruit_NeoPixel/blob/master/Adafruit_NeoPixel.h
//uint8_t colorOrder = 0x52; //or just use NEO_GBR
uint8_t colorOrder = NEO_GRB + NEO_KHZ800;
// Define new pointer for NeoPixel
Adafruit_NeoPixel *pixels;



char done_print_loop = 1;
uint32_t i;
int kkk = 0;

// Interrupt Service Routine Variables.
uint16_t sampleCounter;
volatile uint16_t iCirc,iRead;
uint16_t iTransfer;
volatile uint32_t Old_T_isr;
volatile char ISR_state=0;
volatile uint32_t LastGood_T_isr;


volatile uint32_t isrSkipList[32];
volatile char iISRskip=0;
// the circular buffer is a bit bigger than the full frame
// for FFT, so you can read backwards from the present sample,
// and not have the ISR overwrite the older samples
volatile uint16_t ADC_circ_buffer[N_CIRC_BUF];


float samples[FFT_SIZE * 2];
volatile char ADC_semaphore;




float magnitudes[FFT_SIZE];
float magnitudesINTRP[N_INTERPOLATED_SAMPS];
float hanningWindow[FFT_SIZE * 2];
float FIRpState[(numTaps_intrpFIR / L_upsampleFactor) + FFT_SIZE - 1];
float vMax;
float LowE = 83.0;
char RealtimeToggle = 0;
bin_t bins[NUM_NOTE_BINS];
peak_t peaks[MAX_NUM_PEAKS];

IntervalTimer samplingTimer;



//-------------------------------------------------------------------------------------
//                                                                  function prototypes
void pkDetect(float v[], uint32_t n, uint32_t Npeaks, peak_t pkLst[]);
void getSpectrum() ;

void NoteBinBuider(int LowE, float BinWidth, bin_t bin[]);
void NoteID(peak_t pk[], bin_t bin[]);
void SimInput(void);
void samplingBegin();
uint8_t GetNewSampleBuffer();
void LED_test(void);
void debugPrintInSetup(void);
void LED_animation(void);
void RealtimeToggleIndicator(void);
void debugPrintSampsAndTimeStamps(void);
void IntSamp2Float(void);

//-------------------------------------------------------------------------------------
//                                                                      ARDUINO SETUP()
// runs one time, at power-up.
void setup() {
  Serial1.begin(9600);

  Serial1.println("Guitar Spectral Peak Finder Test");
  Serial1.println("__samplesTS");
Serial1.print("Tsample microseconds=");
Serial1.println(SAMPLE_PERIOD_US);
  // Turn on the power indicator LED.
  pinMode(POWER_LED_PIN, OUTPUT);
  digitalWrite(POWER_LED_PIN, HIGH);

  digitalWrite(POWER_LED_PIN, LOW);



  // Here is a good place to read numPixel & colorOrder from EEPROM or what ever.
  // create a new NeoPixel instance with new values
  pixels = new Adafruit_NeoPixel(LED_STRAND_LENGTH, PIN, colorOrder);
  pixels->begin(); // This initializes the NeoPixel library.



  NoteBinBuider(LowE, BIN_WIDTH, bins);
  ADC_semaphore = 0;



  SimInput();



  //Serial1.println("start processing the buffer");
  unsigned long StartTime = micros();
  //getSpectrum() ;

  //pkDetect(magnitudesINTRP, N_INTERPOLATED_SAMPS, MAX_NUM_PEAKS, peaks);

  //NoteID(peaks, bins);

  // unsigned long EndTime = micros();
  //Serial1.print("Time to convert buffer to note bins=");
  //Serial1.print((EndTime - StartTime) / 1000);
  //Serial1.println(" milliseconds");



  // Set up ADC and audio input.
  pinMode(AUDIO_INPUT_PIN, INPUT);
  analogReadResolution(ANALOG_READ_RESOLUTION);
  analogReadAveraging(ANALOG_READ_AVERAGING);
  // Begin sampling audio
  //samplingBegin();


  //LED_test();


  //debugPrintInSetup();






  ADC_semaphore = 1;


}
//-------------------------------------------------------------------------------------
//                                                                      ARDUINO LOOP()
// a while(1==1) loop.  you must allow it to
// run.  if you kill it, by putting in your
// own while(1==1), you block the serial port
//

// fake out a 30 ms process time, and
// after awhile, stop and print out a buffer
// you should see the adc'd sine wave, but with
// some flat spots where you fixed the interrupt stuff.



void loop() {
 // if (ADC_semaphore == 0)
  {
    IntSamp2Float();

    //SimInput();

    //getSpectrum() ;

   // pkDetect(magnitudesINTRP, N_INTERPOLATED_SAMPS, MAX_NUM_PEAKS, peaks);
    //NoteID(peaks, bins);
    // debugPrintInSetup();

   // RealtimeToggleIndicator();
   delay(10);

   LED_animation();



    if (kkk++ == 100)
    {

{
      samplingTimer.end();
  Serial1.println("--------------------------");
  for(int i=0;i<32;i++)Serial1.println(isrSkipList[i]);
}
      
      // debugPrintSampsAndTimeStamps();
      while (1 == 1) {} //kill.
    }



    ADC_semaphore = 1;
  }




}
