#define LIN_OUT 1 // use the log output function
#define FFT_N 128 // set number of FFT points

#include <math.h>
#include <FFT.h>
#include "LPD8806.h"
#include "SPI.h"

int peak_index;
int max_value;
int sum_fft;
float frequency;
float SAMPLE_RATE = 9600.0;
int SKIP_MULT = 16;
int MAX_FFT_BIN = 16383;
int brightness_mult;

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
    Serial.begin(9600);
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
         * sample at slower rate, minimum sample rate is 9.6 kHz
         * so reduce sample rate by skipping ADC read cycles by 
         * SKIP_MULT
         */
        for (int j = 0; j < SKIP_MULT; j++) {
            while(!(ADCSRA & 0x10)); // wait for adc to be ready
            ADCSRA = 0xf5;           // restart adc
        }

        byte m = ADCL;             // fetch adc data
        byte j = ADCH;
        int k = (j << 8) | m;      // form into an int
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
    int i;

    for (i=0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, c);
        strip.show();
    }
}

void loop() {
    //cli();  
    calculateFFT();  // result is placed in fft_log_out
    //sei();

    peak_index = 0;
    max_value = 0;
    sum_fft = 0;

    for (int i=0; i < (FFT_N / 2); i++) {
        if (max_value < fft_lin_out[i]) {
            // peak index determines frequency
            peak_index = i;
            max_value = fft_lin_out[i];
        }
        if (i > 0) {
            // sum of all bins determines brightness
            sum_fft += fft_lin_out[i];
        }
    }

    frequency = (peak_index * (SAMPLE_RATE / SKIP_MULT)) / (FFT_N / 4);
    brightness_mult = floor(95 * max_value / MAX_FFT_BIN);
    
    if (max_value < 80 || sum_fft < 400 || peak_index == 0) {
        // signal too weak or we got the 1st peak (0Hz), no lights
        setColor(strip.Color(0, 0, 0));
    } else if (frequency > 0 && frequency < base_freak * 2) {
        // red
        setColor(strip.Color(brightness_mult,   0,   0));
    }
    else if (frequency >= base_freak * 2 && frequency < (base_freak * 3)) {
        // yellow
        setColor(strip.Color( brightness_mult, brightness_mult,   0));
    }
    else if (frequency >= base_freak * 3 && frequency < (base_freak * 4)) {
        // green
        setColor(strip.Color(  0, brightness_mult,   0));
    }
    else if (frequency >= base_freak * 4 && frequency < base_freak * 5) {
        // cyan
        setColor(strip.Color(  0, brightness_mult, brightness_mult));
    }
    else if (frequency >= base_freak * 5 && frequency < base_freak * 6) {
        // blue
        setColor(strip.Color(  0,   0, brightness_mult));
    }
    else if (frequency >= base_freak * 6 && frequency < SAMPLE_RATE/4) {
        // violence!!!
        setColor(strip.Color(brightness_mult,   0, brightness_mult));
    }
    
    Serial.print("FFT sum: ");
    Serial.print(sum_fft);
    Serial.print("\tPeak index: ");
    Serial.print(peak_index);
    Serial.print("\tMax value: ");
    Serial.print(max_value);
    Serial.print("\tFrequency: ");
    Serial.println(frequency);
//    delay(30);
}


