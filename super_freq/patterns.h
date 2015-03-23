boolean shimmy_even = true;
boolean bitrot_rotten = false;
int bitrot_led;
boolean bitgarden_grew = false;
int bitgarden_led;
byte swell_brightness = 0;
boolean swell_up = true;

void engageViolence() {
    if (DEBUG) {
        Serial.println("Violence mode engaged!");
    }
    
    // first ramp up the brightness
    for (int i=0; i < 128; i++) {
        strip_color = strip.Color(i, i, i);
        for (int i=0; i < strip.numPixels(); i++) {
            strip.setPixelColor(i, strip_color);
        }
        strip.show();
        delay(10);
    }
    
    while (white_out_button_state == HIGH) {
        // do nothing, we hold the violence!
        white_out_button_state = digitalRead(white_out_button_pin);
    }
}

// start pattern functions
void bullet() {
    // a single LED running fast across the strip
    int j = 0;
    while (pattern_button_state == HIGH) {
        for (int i=0; i < strip.numPixels(); i++) {
            if (i==j) {
                strip.setPixelColor(i, strip_color);
            } else {
                strip.setPixelColor(i, strip.Color(0, 0, 0));
            }
        }
        strip.show();
        delay(10);
        j++;
        if (j >= strip.numPixels()) {
            j = 0;
        }
        pattern_button_state = digitalRead(pattern_button_pin);
    }
}

void snail() {
    // a single LED running slow across the strip
    int j = 0;
    while (pattern_button_state == HIGH) {
        for (int i=0; i < strip.numPixels(); i++) {
            if (i==j) {
                strip.setPixelColor(i, strip_color);
            } else {
                strip.setPixelColor(i, strip.Color(0, 0, 0));
            }
        }
        strip.show();
        delay(250);
        j++;
        if (j >= strip.numPixels()) {
            j = 0;
        }
        pattern_button_state = digitalRead(pattern_button_pin);
    }
}

void shimmy() {
    // evens then odds
    while (pattern_button_state == HIGH) {
        for (int i=0; i < strip.numPixels(); i++) {
            if (shimmy_even) {
                if (i%2) {
                    strip.setPixelColor(i, strip_color);
                } else {
                    strip.setPixelColor(i, strip.Color(0, 0, 0));
                }
            } else {
                if (i%2) {
                    strip.setPixelColor(i, strip.Color(0, 0, 0));
                } else {
                    strip.setPixelColor(i, strip_color);
                }
            }
        }
        shimmy_even = !shimmy_even;
        strip.show();
        delay(100);
        pattern_button_state = digitalRead(pattern_button_pin);
    }
}

void fountain() {
    // start in the middle and grow both sides
    
    while (pattern_button_state == HIGH) {

        // first, turn all the lights off
        for (int i=0; i < strip.numPixels(); i++) {
            strip.setPixelColor(i, strip.Color(0, 0, 0));
        }
        strip.show();
        
        for (int i=0; i < (strip.numPixels() / 2); i++) {
            
            // pattern differs for odd vs even LED lengths
            // if even we start with the middle 2 LEDs, 
            // for odd just the 1 middle LED
            if (strip.numPixels() % 2) {
                // it's an odd number light up the one middle LED
                strip.setPixelColor(strip.numPixels() / 2 - i, strip_color);
                strip.setPixelColor(strip.numPixels() / 2 + i, strip_color);
            } else {
                // it's an even number we have to start from the middle 2 LEDs
                strip.setPixelColor(strip.numPixels() / 2 - i - 1, strip_color);
                strip.setPixelColor(strip.numPixels() / 2 + i, strip_color);
            }
            strip.show();
            delay(30);
        }

        pattern_button_state = digitalRead(pattern_button_pin);
    }
}

void bitrot() {
    // start with all lights on w/ current color then
    // randomly turn off one light at a time until all
    // are gone
        
    while (pattern_button_state == HIGH) {
        
        // reset the LEDs to the current color in case the button
        // is held past one bitrot cycle
        for (int i=0; i < strip.numPixels(); i++) {
            strip.setPixelColor(i, strip_color);
        }       
        
        while (!bitrot_rotten) {       
            // chose a random LED
            bitrot_led = rand()%strip.numPixels();
            strip.setPixelColor(bitrot_led, strip.Color(0, 0, 0));
            strip.show();
            
            delay(10);
            
            // check if all lights are off
            for (int i=0; i < strip.numPixels(); i++) {
                if (strip.getPixelColor(i) > 0) {
                    // found a lit LED, not all off yet
                    bitrot_rotten = false;
                    break;
                } else {
                    bitrot_rotten = true;
                }
            }
        }
        
        bitrot_rotten = false;        
        pattern_button_state = digitalRead(pattern_button_pin);
    }
}

void bitgarden() {
    // start with all lights off w/ then randomly
    // turn on one light at a time w/ current color
    // until all are lit
    
    // Check if the current strip_color is "off" to
    // avoid an infinite loop...we can turn "on" all
    // the LEDs to the current color if there is no
    // current color!
    //
    // However, it's a bit weird! (pun intended)
    // The strip_color value has each color component
    // 7-bit value stored as 8 bits with the high bit
    // set to 1, BUT if you read a particular LED
    // using getPixelColor that high bit is set to 0
    //
    // So, if we check our strip_color to determine if
    // the current color is "off" we would have to do
    // some bit shifting and bit masking. Instead,
    // we just read the first LED.
    if (strip.getPixelColor(0) <= 0) {
        return;
    }
    
    while (pattern_button_state == HIGH) {
        
        // turn off all LEDs 
        for (int i=0; i < strip.numPixels(); i++) {
            strip.setPixelColor(i, strip.Color(0, 0, 0));
        }
        
        // keep looping while bitgarden_grew is false
        while (!bitgarden_grew) {       
            // chose a random LED
            bitgarden_led = rand()%strip.numPixels();
            strip.setPixelColor(bitgarden_led, strip_color);
            strip.show();
            
            delay(10);
            
            // check if all lights are off
            for (int i=0; i < strip.numPixels(); i++) {
                if (strip.getPixelColor(i) <= 0) {
                    // found a blank LED, not all on yet
                    bitgarden_grew = false;
                    break;
                } else {
                    bitgarden_grew = true;
                }
            }
        }
        
        bitgarden_grew = false;        
        pattern_button_state = digitalRead(pattern_button_pin);
    }
}

void swell() {
    // use current color and pulse the brightness
    swell_brightness = brightness;
        
    while (pattern_button_state == HIGH) {      

        for (int i=0; i < strip.numPixels(); i++) {
            
            strip.setPixelColor(i, 
                strip.Color(
                    round((red   / 100.0) * swell_brightness),
                    round((green / 100.0) * swell_brightness),
                    round((blue  / 100.0) * swell_brightness)
                )
            );
        }
                
        strip.show();        
        
        if (swell_brightness >= 127) {
            swell_up = false;
        } else if (swell_brightness <=15) {
            swell_up = true;
        }
        
        if (swell_up) {
            swell_brightness++;
        } else {
            swell_brightness--;
        }
        
        pattern_button_state = digitalRead(pattern_button_pin);
    }
}
