// FFT constants
#define SAMPLE_RATE 9600
#define SKIP_MULT 8
#define MAX_FFT_BIN 32767
#define LIN_OUT 1          // use the linear output function
#define FFT_N 128          // set number of FFT points
#define MIN_FFT_SUM 900    // Used to turn off the lights for low volumes
#define MIN_PEAK_VALUE 400  // Used to turn off the lights for low volumes

#include <math.h>
#include <FFT.h>
#include "LPD8806.h"
#include "SPI.h"
#include "setup.h"
#include "palettes.h"
#include "patterns.h"
#include "interface_controllers.h"
#include "fft_controllers.h"
#include "light_strip_controllers.h"

void setup() {
    if (DEBUG) {
        Serial.begin(9600);
    }
    
    // pull down for palette change interrupt pin
    digitalWrite(2, LOW);
    
    // pull down for pattern change interrupt pin
    digitalWrite(2, LOW);
    
    // setup interrupt 0 (digital pin 2) to detect changed palette 
    // rotary switch position
    attachInterrupt(0, palette_switch_changed, RISING);
    
    // setup interrupt 1 (digital pin 3) to detect changed pattern 
    // rotary switch position
    attachInterrupt(1, pattern_switch_changed, RISING);
    
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

void loop() {
    // check for LED count mode
    if (set_LED_count_mode == true) {
        if (DEBUG) {
            Serial.println("Entered LED set mode");
        }
        setLEDcount();
        return;
    }
    
    if (palette_switch_flag) {
        input_A1 = read_analog_pin(A1);
        new_palette_choice = determine_switch_selection(input_A1);
        changeColorPalette();
        
        if (DEBUG) {
            Serial.print("Analog value: ");
            Serial.println(input_A1);
        }
        
        palette_switch_flag = false;
    }
    
    if (pattern_switch_flag) {
        input_A2 = read_analog_pin(A2);
        pattern_choice = determine_switch_selection(input_A2) - 1;
        
        if (DEBUG) {
            Serial.print("Analog value: ");
            Serial.println(input_A2);
        }
        
        pattern_switch_flag = false;
    }
    
    // read pattern engage input pin here
    pattern_button_state = digitalRead(pattern_button_pin);
    
    if (pattern_button_state == HIGH) {
        // initialize pattern mode
        if (DEBUG) {
            Serial.print("Analog value: ");
            Serial.println(input_A2);
            Serial.print("Initialize pattern: ");
            Serial.println(pattern_choice);
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
    
    // iterate over all bins except the first bin, 
    // as the first one's the total power in the sample
    for (int i=1; i < (FFT_N / 2); i++) {
        if (max_value < fft_lin_out[i]) {
            // peak index determines frequency
            peak_index = i;
            // max value used as an additional check to turn the lights off
            max_value = fft_lin_out[i];
        }
        
        // control brightness by summing all bins
        sum_fft += fft_lin_out[i];
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
    delay(20);
            
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
