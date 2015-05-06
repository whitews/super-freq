// FHT constants
#define SAMPLE_RATE 76923   // in kHz, 16MHz w/ prescaler=16 & 13 clock cycles
#define SKIP_MULT 24
#define MAX_BIN 16383
#define LIN_OUT 1           // use the linear output function
#define FHT_N 256           // set number of frequency bins
#define MIN_BIN_SUM 300     // Used to turn off the lights for low volumes
#define MIN_PEAK_VALUE 150  // Used to turn off the lights for low volumes

#include <math.h>
#include <FHT.h>
#include "LPD8806.h"
#include "SPI.h"
#include "setup.h"
#include "palettes.h"
#include "patterns.h"
#include "interface_controllers.h"
#include "freq_controllers.h"
#include "light_strip_controllers.h"

void setup() {
    if (DEBUG) {
        Serial.begin(115200);
    }
    
    // determine the update delay based on LED count for pattern timing
    calculateUpdateDelay();
    
    // pull down for palette change interrupt pin
    pinMode(10, INPUT);
    digitalWrite(10, LOW);
    
    // pull down for pattern change interrupt pin
    pinMode(11, INPUT);
    digitalWrite(11, LOW);
    
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
    // division factor of 16 (B100)
    ADCSRA = B11100100;
    
    ADMUX = B01000000;  // use adc0
    
    // turn off the digital input for all analog pins
    DIDR0 = B00111111;
    
    // initial color palette
    for (int i = 0; i < palette_color_count; i++) {
        current_palette[i].red = pgm_read_byte(
            &(palettes[i + (palette_color_count * palette_choice)].red)
        );
        current_palette[i].green = pgm_read_byte(
            &(palettes[i + (palette_color_count * palette_choice)].green)
        );
        current_palette[i].blue = pgm_read_byte(
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
            Serial.print("Palette choice: ");
            Serial.println(palette_choice);
        }
        
        palette_switch_flag = false;
    }
    
    if (pattern_switch_flag) {
        input_A2 = read_analog_pin(A2);
        pattern_choice = determine_switch_selection(input_A2) - 1;
        
        if (DEBUG) {
            Serial.print("Analog value: ");
            Serial.println(input_A2);
            Serial.print("Pattern choice: ");
            Serial.println(pattern_choice);
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
                comet();
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
                comet();
            case 8:
                comet();
            case 9:
                comet();
            case 10:
                comet();
            case 11:
                comet();
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
        for (int i = 0; i < palette_color_count; i++) {
            current_palette[i].red = pgm_read_byte(&(palettes[i].red));
            current_palette[i].green = pgm_read_byte(&(palettes[i].green));
            current_palette[i].blue = pgm_read_byte(&(palettes[i].blue));
        }
    }
    
    cli();            // disable global interrupts
    calculateFreq();  // result is placed in fht_lin_out
    sei();            // re-enable interrupts

    peak_index = 0;
    max_value = 0;
    sum_fht = 0;
    
    // iterate over bins 6 through 36, 
    // Note: first bin is the total power in the sample
    for (int i=6; i <= 36; i++) {
        if (max_value < fht_lin_out[i]) {
            // peak index determines frequency
            peak_index = i;
            // max value used as an additional check to turn the lights off
            max_value = fht_lin_out[i];
        }
        
        // control brightness by summing all bins (undampened)
        sum_fht += fht_lin_out[i];
    }

    // Use sum_fht to determine brightness, but substract our min threshold
    // first to allow reaching lower brightness levels
    if (sum_fht > MIN_BIN_SUM) {
        brightness = round(
            1.0 * max_brightness * (sum_fht - MIN_BIN_SUM) / MAX_BIN
        );
    } else {
        brightness = 0;
    }
    
    // safety net in case our calculated brightness was out of range
    if (brightness > 127) {  // too much
        brightness = 127;
    } else if (brightness < 0) {  // too little
        brightness = 0;
    }
    
    // Reset peak_index to zero if signal is weak
    if (max_value < MIN_PEAK_VALUE || sum_fht < MIN_BIN_SUM) {
        peak_index = 0;
    }
    
    setColor(peak_index, brightness);
    //delay(250);
    
    if (DEBUG) {
        frequency = peak_index * ((1.0 * SAMPLE_RATE / SKIP_MULT) / FHT_N);
        
        Serial.print("FHT sum: ");
        Serial.print(sum_fht);
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
