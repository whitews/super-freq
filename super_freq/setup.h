#define DEBUG 0  // set to 1 to turn on Serial printing

// Pins
byte button2Pin = 7;  // any pin
byte button3Pin = 8;  // any pin

int button2State = 0;
int button3State = 0;

// LEDeez
int nLEDs = 32;  // Number of RGB LEDs in strand
byte max_brightness = 127;  // LED intensity max is 127
byte brightness;  // where LED brightness is stored (based on dB level)

// First parameter is the number of LEDs in the strand.
// We're using the dedicated data and clock pins, 11 & 13 respectively
LPD8806 strip = LPD8806(nLEDs);
uint32_t strip_color = strip.Color(0, 0, 0);

// Vars for special mode to set LED count 
boolean set_LED_count_mode = false;
boolean increment_LED_count = false;

// User selection vars
byte palette_choice = 0;
byte pattern_choice = 5;

void setLEDcount() {
    button3State = digitalRead(button3Pin);
    nLEDs = 1;
    strip.updateLength(nLEDs);
    strip_color = strip.Color(127, 127, 127);
    for (int i=0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip_color);
    }
    strip.show();
    
    // button 3 may still be pressed since the code gets here very fast
    // if button 1 is pressed, wait will the user releases it
    while (button3State == HIGH) {
        // do nothing, we need to wait until user releases the button
        button3State = digitalRead(button3Pin);
    }
    
    // button 2 is used to exit the LED count set mode
    while (button2State == LOW) {
        button2State = digitalRead(button2Pin);
        button3State = digitalRead(button3Pin);
        increment_LED_count = false;
        
        if (button3State == HIGH) {
            increment_LED_count = true;
        }
        
        while (button3State == HIGH) {
            // do nothing, we need to wait until user releases the button
            button3State = digitalRead(button3Pin);
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
    
    // button 2 was pressed, save LED count to EEPROM
    set_LED_count_mode = false;
}
