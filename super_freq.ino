#define LIN_OUT 1 // use the log output function
#define FFT_N 128 // set number of FFT points
#define DEBUG 0   // set to 1 to turn on Serial printing

#include <math.h>
#include <FFT.h>
#include "LPD8806.h"
#include "SPI.h"

int peak_index;
int max_value;
int sum_fft;
float frequency;
float SAMPLE_RATE = 9600.0;
int SKIP_MULT = 4;
int MAX_FFT_BIN = 16383;

/*
 * used to cut the lights, 
 * some sources (iPhone) have amps that create a bit of noise that 
 * remains on for a few seconds even after music is stopped or paused.
 * Perhaps this doesn't occur on mixers, and it may be possible to
 * eliminate with an OpAmp and/or low pass filter circuit
 */
int MIN_FFT_SUM = 3200;

// LEDeez
int brightness;  // controls the LED brightness based on dB level
// LEDs can be set up to 127, but it seems too bright so we tone it down a bit
int MAX_BRIGHTNESS = 95;

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

void setup() {
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

// Fill the dots progressively along the strip.
void setColor(uint32_t c) {
    for (int i=0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, c);
        strip.show();
    }
}

void loop() {
    cli();           // disable global interrupts
    calculateFFT();  // result is placed in fft_log_out
    sei();           // re-enable interrupts

    peak_index = 0;
    max_value = 0;
    sum_fft = 0;

    for (int i=0; i < (FFT_N / 2); i++) {
        if (max_value < fft_lin_out[i]) {
            // peak index determines frequency
            peak_index = i;
            // max value determines brightness
            max_value = fft_lin_out[i];
        }
        if (i > 0) {
            /* 
             * maybe an alternate way to control brightness
             * would be the sum of all non-zero bins, i.e. 
             * the total power in the sample. However, I don't
             * have a handle on what the maximum sum would be.
             */
            sum_fft += fft_lin_out[i];
        }
    }

    frequency = (peak_index * (SAMPLE_RATE / SKIP_MULT)) / (FFT_N / 4);
    brightness = round(1.0 * MAX_BRIGHTNESS * sum_fft / MAX_FFT_BIN);
    
    if (max_value < 80 || sum_fft < MIN_FFT_SUM || peak_index == 0) {
        // signal too weak or we got the 1st peak (0Hz), no lights
        setColor(strip.Color(0, 0, 0));
    } else if (frequency > 0 && frequency < base_freak * 2) {
        // red
        setColor(strip.Color(brightness, 0, 0));
    }
    else if (frequency >= base_freak * 2 && frequency < (base_freak * 3)) {
        // yellow
        setColor(strip.Color(brightness, brightness, 0));
    }
    else if (frequency >= base_freak * 3 && frequency < (base_freak * 4)) {
        // green
        setColor(strip.Color(0, brightness, 0));
    }
    else if (frequency >= base_freak * 4 && frequency < base_freak * 5) {
        // cyan
        setColor(strip.Color(0, brightness, brightness));
    }
    else if (frequency >= base_freak * 5 && frequency < base_freak * 6) {
        // blue
        setColor(strip.Color(0, 0, brightness));
    }
    else if (frequency >= base_freak * 6 && frequency < base_freak * 8) {
        // violence!!!
        setColor(strip.Color(brightness, 0, brightness));
    // we'll cycle through the colors again so all high freaks ain't purple!!!
    } else if (frequency > 8 && frequency < base_freak * 10) {
        // red
        setColor(strip.Color(brightness, 0, 0));
    }
    else if (frequency >= base_freak * 10 && frequency < (base_freak * 12)) {
        // yellow
        setColor(strip.Color(brightness, brightness, 0));
    }
    else if (frequency >= base_freak * 12 && frequency < (base_freak * 14)) {
        // green
        setColor(strip.Color(0, brightness, 0));
    }
    else if (frequency >= base_freak * 14 && frequency < base_freak * 16) {
        // cyan
        setColor(strip.Color(0, brightness, brightness));
    }
    else if (frequency >= base_freak * 16 && frequency < base_freak * 18) {
        // blue
        setColor(strip.Color(0, 0, brightness));
    }
    else if (frequency >= base_freak * 18 && frequency < SAMPLE_RATE/SKIP_MULT) {
        // violence!!!
        setColor(strip.Color(brightness, 0, brightness));
    }
    
    if (DEBUG) {
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
    }
}


