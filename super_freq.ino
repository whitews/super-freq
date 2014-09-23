#define LIN_OUT 1 // use the linear output function
#define FFT_N 128 // set number of FFT points
#define DEBUG 1   // set to 1 to turn on Serial printing

#include <math.h>
#include <FFT.h>
#include "LPD8806.h"
#include "SPI.h"

int peak_index;
int max_value;
int sum_fft;
float frequency;
float SAMPLE_RATE = 9600.0;
int SKIP_MULT = 8;
int MAX_FFT_BIN = 16383;

/*
 * used to cut the lights, 
 * some sources (iPhone) have amps that create a bit of noise that 
 * remains on for a few seconds even after music is stopped or paused.
 * Perhaps this doesn't occur on mixers, and it may be possible to
 * eliminate with an OpAmp and/or low pass filter circuit
 */
int MIN_FFT_SUM = 400;

// LEDeez
int brightness;  // controls the LED brightness based on dB level
// LEDs can be set up to 127, but it seems too bright so we tone it down a bit
int MAX_BRIGHTNESS = 127;

float base_freak = SAMPLE_RATE / FFT_N / 2;

// Number of RGB LEDs in strand:
int nLEDs = 32;

// Chose 2 pins for output; can be any valid output pins:
int dataPin  = 2;
int clockPin = 3;

// First parameter is the number of LEDs in the strand.  The LED strips
// are 32 LEDs per meter but you can extend or cut the strip.  Next two
// parameters are SPI data and clock pins:
LPD8806 strip = LPD8806(nLEDs, dataPin, clockPin);
uint32_t strip_color = strip.Color(0, 0, 0);

struct Color {
    float red;
    float green;
    float blue;
};

struct Color full_spectrum[24] = {
    { 0.25, 0.00, 1.00 },  // indigo
    { 0.00, 0.00, 1.00 },  // blue
    { 0.00, 0.25, 1.00 },  // med. blue
    { 0.00, 0.50, 1.00 },  // sky blue
    { 0.00, 0.75, 1.00 },  // pale blue
    { 0.00, 1.00, 1.00 },  // cyan
    { 0.00, 1.00, 0.75 },  // pale green
    { 0.00, 1.00, 0.50 },  // grass green
    { 0.00, 1.00, 0.25 },  // bright green
    { 0.00, 1.00, 0.00 },  // green
    { 0.25, 1.00, 0.00 },  // lime
    { 0.50, 1.00, 0.00 },  // bright lime
    { 0.75, 1.00, 0.00 },  // neon lime
    { 1.00, 1.00, 0.00 },  // yellow
    { 1.00, 0.75, 0.00 },  // orange
    { 1.00, 0.50, 0.00 },  // med. orange
    { 1.00, 0.25, 0.00 },  // dark orange
    { 1.00, 0.00, 0.00 },  // red
    { 1.00, 0.00, 0.25 },  // pink
    { 1.00, 0.00, 0.50 },  // bright pink
    { 1.00, 0.00, 0.75 },  // hot pink
    { 1.00, 0.00, 1.00 },  // purple
    { 0.75, 0.00, 1.00 },  // hot purple
    { 0.50, 0.00, 1.00 }   // deep purple
};

Color color_palette[24];

void setup() {
    for (int i = 0; i < 24; i++) {
        color_palette[i] = full_spectrum[i];
    }

    if (DEBUG) {
        Serial.begin(9600);
    }
    TIMSK0 = 0;           // turn off timer0 for lower jitter
    ADCSRA = 0xe5;        // set the adc to free running mode 
    ADMUX = 0x40;         // use adc0
    DIDR0 = 0x01;         // turn off the digital input for adc0

    // Start up the LED strip
    strip.begin();

    // Update the strip, to start they are all 'off'
    strip.show();
}

void calculateFFT() {
    for (int i = 0 ; i < FFT_N * 2; i += 2) { // save samples
        /* 
         * A hack to sample at a slower rate. 
         * The minimum sample rate is 9.6 kHz, so reduce sample rate 
         * by skipping ADC read cycles, essentially spreading out our
         * sample over time. Reducing the sample rate increases our
         * frequency resolution, so we can differentiate frequencies 
         * close to one another, but at the expense of not detecting higher 
         * frequencies.
         * 
         * The effective sample rate would then be:
         * SAMPLE_RATE / SKIP_MULT
         */
        for (int j = 0; j < SKIP_MULT; j++) {
            while(!(ADCSRA & 0x10)); // wait for adc to be ready
            ADCSRA = 0xf5;           // restart adc
        }

        byte m = ADCL;             // fetch ADC data (low byte)
        byte j = ADCH;             // fetch ADC data (high byte)
        int k = (j << 8) | m;      // combine low/high bytes to form into an int
        k -= 0x0200;               // form into a signed int
        k <<= 6;                   // form into a 16b signed int
        fft_input[i] = k;          // put real data into even bins
        fft_input[i+1] = 0;        // set odd bins to 0
    }
    fft_window();                  // window the data for better frequency response
    fft_reorder();                 // reorder the data before doing the fft
    fft_run();                     // process the data in the fft
    fft_mag_lin();                 // take the output of the fft
}

// Set color based on frequency and brightness
void setColor(int peak_index, int brightness) {
    
    if (peak_index == 0) {
        strip_color = strip.Color(0, 0, 0);
    } else if (peak_index > 24) {
        strip_color = strip.Color(
            round(color_palette[23].red   * brightness),
            round(color_palette[23].green * brightness),
            round(color_palette[23].blue  * brightness)
        );
    } else {
        strip_color = strip.Color(
            round(color_palette[peak_index - 1].red   * brightness),
            round(color_palette[peak_index - 1].green * brightness),
            round(color_palette[peak_index - 1].blue  * brightness)
        );
    }
    
    for (int i=0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip_color);
    }
    strip.show();
}

void loop() {
    cli();           // disable global interrupts
    calculateFFT();  // result is placed in fft_lin_out
    sei();           // re-enable interrupts

    peak_index = 0;
    max_value = 0;
    sum_fft = 0;

    for (int i=0; i < (FFT_N / 2); i++) {
        if (max_value < fft_lin_out[i]) {
            // peak index determines frequency
            peak_index = i;
            // max value was used to determine brightness, now using sum_fft
            max_value = fft_lin_out[i];
        }
        if (i > 0) {
            /* 
             * alternate way to control brightness
             * by summng all bins except the first bin, i.e. 
             * the total power in the sample. However, I don't
             * have a handle on what the maximum sum would be.
             */
            sum_fft += fft_lin_out[i];
        }
    }

    // Note, using approximate max for scaling brightness as:
    // 1.5 * max bin size minus our MIN_FFT_SUM lower cutoff
    brightness = round(1.0 * MAX_BRIGHTNESS * sum_fft / (MAX_FFT_BIN * 1.5 - MIN_FFT_SUM));
    if (brightness > 127) {
        brightness = 127;
    }
    
    // Set frequency to zero if signal is weak or if the 1st peak (0Hz) is dominant
    if (max_value < 60 || sum_fft < MIN_FFT_SUM || peak_index == 0) {
        // signal too weak or we got the 1st peak (0Hz), no lights
        peak_index = 0;        
    }
    
    setColor(peak_index, brightness);
            
    if (DEBUG) {
        frequency = (peak_index * (SAMPLE_RATE / SKIP_MULT)) / (FFT_N / 4);
        
        Serial.print("FFT sum: ");
        Serial.print(sum_fft);
        Serial.print("\tPeak index: ");
        Serial.print(peak_index);
        Serial.print("\tMax value: ");
        Serial.print(max_value);
        Serial.print("\tFrequency: ");
        Serial.print(frequency);
        Serial.print("\tBrightness: ");
        Serial.println(brightness);
        
        Serial.print("Red: ");
        Serial.print(color_palette[peak_index].red);
        Serial.print("\tGreen: ");
        Serial.print(color_palette[peak_index].green);
        Serial.print("\tBlue: ");
        Serial.println(color_palette[peak_index].blue);
    }
}


