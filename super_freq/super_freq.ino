// FFT constants
#define SAMPLE_RATE 9600
#define SKIP_MULT 8
#define MAX_FFT_BIN 32767
#define LIN_OUT 1          // use the linear output function
#define FFT_N 128          // set number of FFT points
#define MIN_FFT_SUM 400    // Used to turn off the lights for low volumes
#define MIN_PEAK_VALUE 30  // Used to turn off the lights for low volumes

#include <math.h>
#include <FFT.h>
#include "LPD8806.h"
#include "SPI.h"
#include "setup.h"
#include "palettes.h"
#include "patterns.h"

// FFT vars
int peak_index;
int max_value;
int sum_fft;
float frequency;  // only used for debugging
byte low_byte;

void setup() {
    if (DEBUG) {
        Serial.begin(9600);
    }
    
    pinMode(pattern_button_pin, INPUT);
    pinMode(white_out_button_pin, INPUT);
    
    // check for button press to enter LED count set mode
    white_out_button_state = digitalRead(white_out_button_pin);
    
    if (white_out_button_state == HIGH) {
        set_LED_count_mode = true;
    }
    
    // set ADC to free running mode w/ prescaler set to a 
    // division factor of 32 (B101)
    ADCSRA = B11100101;
    
    ADMUX = B01000000;  // use adc0
    
    // turn off the digital input for all analog pins
    DIDR0 = B00111111;
    
    // initial color palette
    for (int i = 0; i < 24; i++) {
        color_palette[i].red = pgm_read_byte(
            &(palettes[i + (palette_color_count * palette_choice)].red)
        );
        color_palette[i].green = pgm_read_byte(
            &(palettes[i + (palette_color_count * palette_choice)].green)
        );
        color_palette[i].blue = pgm_read_byte(
            &(palettes[i + (palette_color_count * palette_choice)].blue)
        );
    }

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
         * SAMPLE_RATE / (SKIP_MULT / 2)
         */
        for (int j = 0; j < SKIP_MULT; j++) {
            while(!(ADCSRA & B00010000)); // wait for adc to be ready
            ADCSRA = B11110101;           // restart adc
        }

        low_byte = ADCL;           // fetch ADC data (low byte)
        int k = (ADCH << 8) | low_byte;  // combine low/high bytes to form into an int
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
        // signal was weak, turn lights off
        red = 0;
        green = 0;
        blue = 0;
    } else if (peak_index > 24) {
        red = color_palette[23].red;
        green = color_palette[23].green;
        blue = color_palette[23].blue;  
    } else {
        red = color_palette[peak_index - 1].red;
        green = color_palette[peak_index - 1].green;
        blue = color_palette[peak_index - 1].blue;        
    }
    
    strip_color = strip.Color(
        round((red   / 100.0) * brightness),
        round((green / 100.0) * brightness),
        round((blue  / 100.0) * brightness)
    );
    
    if (DEBUG) {
        Serial.print((red / 100.0) * brightness);
        Serial.print("\t");
        Serial.print((green / 100.0) * brightness);
        Serial.print("\t");
        Serial.println((blue / 100.0) * brightness);
    }
    
    for (int i=0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip_color);
    }
    strip.show();
}

void loop() {
    // check for LED count mode
    if (set_LED_count_mode == true) {
        if (DEBUG) {
            Serial.println("Entered LED set mode");
        }
        setLEDcount();
        return;
    }
    
    // read pattern engage input pin here
    pattern_button_state = digitalRead(pattern_button_pin);
    
    if (pattern_button_state == HIGH) {
        // initialize pattern mode
        if (DEBUG) {
            Serial.println("Initialize pattern!");
        }
        switch (pattern_choice) {
            case 0:
                bullet();
            case 1:
                snail();
            case 2:
                shimmy();
            case 3:
                fountain();
            case 4:
                bitrot();
            case 5:
                bitgarden();
            case 6:
                swell();
            case 7:
                bullet();
            case 8:
                bullet();
            case 9:
                bullet();
            case 10:
                bullet();
            case 11:
                bullet();
            default:
                break;
        }
    }
    
    // read white out input pin here
    white_out_button_state = digitalRead(white_out_button_pin);
    
    if (white_out_button_state == HIGH) {
        // engage violence mode!
        engageViolence();
    }
    
    // check if palette changed
    // Note: this equality will be changed when color palette chooser 
    // is implemented
    if (palette_choice != palette_choice) {
        for (int i = 0; i < 24; i++) {
            color_palette[i].red = pgm_read_byte(&(palettes[i].red));
            color_palette[i].green = pgm_read_byte(&(palettes[i].green));
            color_palette[i].blue = pgm_read_byte(&(palettes[i].blue));
        }
    }
    
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
            // max value used as an additional check to turn the lights off
            max_value = fft_lin_out[i];
        }
        if (i > 0) {
            // control brightness by summing all bins except the first bin, 
            // i.e. the total power in the sample
            sum_fft += fft_lin_out[i];
        }
    }

    // Use sum_fft to determine brightness
    brightness = round(1.0 * max_brightness * sum_fft / MAX_FFT_BIN);
    
    // safety net in case our calculated brightness was out of range
    if (brightness > 127) {  // too much
        brightness = 127;
    } else if (brightness < 0) {  // too little
        brightness = 0;
    }
    
    // Reset peak_index to zero if signal is weak
    if (max_value < MIN_PEAK_VALUE || sum_fft < MIN_FFT_SUM) {
        peak_index = 0;        
    }
    
    setColor(peak_index, brightness);
    delay(65);
            
    if (DEBUG) {
        frequency = peak_index * ((1.0 * SAMPLE_RATE / (SKIP_MULT / 2)) / (FFT_N / 2));
        
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
