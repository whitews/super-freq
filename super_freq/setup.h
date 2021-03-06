#define DEBUG 0  // set to 1 to turn on Serial printing

// Pins
byte pattern_button_pin = 1;  // any digital pin except 2, 3, 11, or 13
byte white_out_button_pin = 2;  // any digital pin except 2, 3, 11, or 13

int pattern_button_state = 0;
int white_out_button_state = 0;

// Interrupt flags, default to true to trigger initial read
volatile boolean palette_switch_flag = true;
volatile boolean pattern_switch_flag = true;

// LEDeez
int nLEDs = 32;  // Number of RGB LEDs in strand
byte max_brightness = 127;  // LED intensity max is 127
byte brightness;  // where LED brightness is stored (based on dB level)
int update_delay;  // in milliseconds, determined by the LED count

// First parameter is the number of LEDs in the strand.
// We're using the dedicated data and clock pins, 11 & 13 respectively
LPD8806 strip = LPD8806(nLEDs);
uint32_t strip_color = strip.Color(0, 0, 0);

// Vars for special mode to set LED count 
boolean set_LED_count_mode = false;
boolean increment_LED_count = false;

// FHT vars
int peak_index;
int max_value;
int sum_fht;
float frequency;  // only used for debugging

// User selection vars
int input_A1;
int input_A2;
byte selection_A1;
byte new_palette_choice;
byte palette_choice = 0;
byte pattern_choice = 5;

void calculateUpdateDelay() {
    if (nLEDs > 0 && nLEDs <= 32) {
        update_delay = 20;
    } else if (nLEDs > 32 && nLEDs <= 64) {
        update_delay = 17;
    } else if (nLEDs > 64 && nLEDs <= 128) {
        update_delay = 14;
    } else if (nLEDs > 128 && nLEDs <= 256) {
        update_delay = 10;
    } else if (nLEDs > 256 && nLEDs <= 512) {
        update_delay = 4;
    } else {
        update_delay = 0;  // above 512, update is slow enough as it is
    }
}

void setLEDcount() {
    white_out_button_state = digitalRead(white_out_button_pin);
    nLEDs = 1;
    strip.updateLength(nLEDs);
    strip_color = strip.Color(127, 127, 127);
    for (int i=0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip_color);
    }
    strip.show();
    
    // button 3 may still be pressed since the code gets here very fast
    // if button 1 is pressed, wait will the user releases it
    while (white_out_button_state == HIGH) {
        // do nothing, we need to wait until user releases the button
        white_out_button_state = digitalRead(white_out_button_pin);
    }
    
    // button 2 is used to exit the LED count set mode
    while (pattern_button_state == LOW) {
        pattern_button_state = digitalRead(pattern_button_pin);
        white_out_button_state = digitalRead(white_out_button_pin);
        increment_LED_count = false;
        
        if (white_out_button_state == HIGH) {
            increment_LED_count = true;
        }
        
        while (white_out_button_state == HIGH) {
            // do nothing, we need to wait until user releases the button
            white_out_button_state = digitalRead(white_out_button_pin);
        }
        
        if (increment_LED_count) {
            nLEDs++;
            strip.updateLength(nLEDs);
            strip_color = strip.Color(127, 127, 127);
            for (int i=0; i < strip.numPixels(); i++) {
                strip.setPixelColor(i, strip_color);
            }
            strip.show();
        }
    }
    
    calculateUpdateDelay();
    
    // button 2 was pressed, save LED count to EEPROM
    set_LED_count_mode = false;
}
