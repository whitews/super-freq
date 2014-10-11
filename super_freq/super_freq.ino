#define LIN_OUT 1 // use the linear output function
#define FFT_N 128 // set number of FFT points
#define DEBUG 1   // set to 1 to turn on Serial printing

/*
 * Used to turn off the lights for low volumes 
 * 
 * Note: some sources (iPhone) have amps that 
 * remains on for a few seconds even after the 
 * music is stopped or paused, so these are a 
 * little higher than they might ought to be 
 * for mixers.
 */
#define MIN_FFT_SUM 400
#define MIN_PEAK_VALUE 30

// Some FFT constants
#define SAMPLE_RATE 9600
#define SKIP_MULT 8
#define MAX_FFT_BIN 32767

#include <math.h>
#include <FFT.h>
#include "LPD8806.h"
#include "SPI.h"

// FFT stuff
int peak_index;
int max_value;
int sum_fft;
float frequency;  // only used for debugging

// LEDeez
int nLEDs = 32;  // Number of RGB LEDs in strand
byte max_brightness = 127;  // LED intensity max is 127
byte brightness;  // where LED brightness is stored (based on dB level)

// Pins
byte dataPin  = 2;  // any pin
byte clockPin = 3;  // any pin
byte button2Pin = 7;  // any pin
byte button3Pin = 8;  // any pin

int button2State = 0;
int button3State = 0;

boolean set_LED_count_mode = false;
boolean increment_LED_count = false;

// First parameter is the number of LEDs in the strand.  The LED strips
// are 32 LEDs per meter but you can extend or cut the strip.  Next two
// parameters are SPI data and clock pins:
LPD8806 strip = LPD8806(nLEDs, dataPin, clockPin);
uint32_t strip_color = strip.Color(0, 0, 0);

// Color stuff
byte red = 0;
byte green = 0;
byte blue = 0;

struct Color {
    byte red;
    byte green;
    byte blue;
};

byte palette_color_count = 24;

// each palette is 24 colors, all are stored in the same array
const struct Color palettes[144] PROGMEM = {
    // 1st palette: full spectrum of colors
    {  25,   0, 100 },  // indigo
    {   0,   0, 100 },  // blue
    {   0,  25, 100 },  // med. blue
    {   0,  50, 100 },  // sky blue
    {   0,  75, 100 },  // pale blue
    {   0, 100, 100 },  // cyan
    {   0, 100,  75 },  // pale green
    {   0, 100,  50 },  // grass green
    {   0, 100,  25 },  // bright green
    {   0, 100,   0 },  // green
    {  25, 100,   0 },  // lime
    {  50, 100,   0 },  // bright lime
    {  75, 100,   0 },  // neon lime
    { 100, 100,   0 },  // yellow
    { 100,  75,   0 },  // orange
    { 100,  50,   0 },  // med. orange
    { 100,  25,   0 },  // dark orange
    { 100,   0,   0 },  // red
    { 100,   0,  25 },  // pink
    { 100,   0,  50 },  // bright pink
    { 100,   0,  75 },  // hot pink
    { 100,   0, 100 },  // purple
    { 100,  50, 100 },  // hot purple
    { 100, 100, 100 },  // white hot

    // 2nd palette: warm colors
    { 100, 100,   0 },  // yellow
    { 100, 100,   0 },  // yellow
    { 100, 100,   0 },  // yellow
    { 100, 100,   0 },  // yellow
    { 100,  50,   0 },  // orange
    { 100,  50,   0 },  // orange
    { 100,  50,   0 },  // orange
    { 100,  50,   0 },  // orange
    { 100,   0,   0 },  // red
    { 100,   0,   0 },  // red
    { 100,   0,   0 },  // red
    { 100,   0,   0 },  // red
    { 100,   0, 100 },  // purple
    { 100,   0, 100 },  // purple
    { 100,   0, 100 },  // purple
    { 100,   0, 100 },  // purple
    { 100,  50, 100 },  // hot purple
    { 100,  50, 100 },  // hot purple
    { 100,  50, 100 },  // hot purple
    { 100,  50, 100 },  // hot purple
    { 100, 100, 100 },  // white hot
    { 100, 100, 100 },  // white hot
    { 100, 100, 100 },  // white hot
    { 100, 100, 100 },  // white hot
     
     // 3rd palette: Blue Orange
    {   0,   0, 100 },  // blue
    {   0,   0, 100 },  // blue
    {   0,   0, 100 },  // blue
    {   0,   0, 100 },  // blue
    { 100,  50,   0 },  // orange
    { 100,  50,   0 },  // orange
    { 100,  50,   0 },  // orange
    { 100,  50,   0 },  // orange
    {   0,   0, 100 },  // blue
    {   0,   0, 100 },  // blue
    { 100,  50,   0 },  // orange
    { 100,  50,   0 },  // orange
    {   0,   0, 100 },  // blue
    {   0,   0, 100 },  // blue
    { 100,  50,   0 },  // orange
    { 100,  50,   0 },  // orange
    {   0,   0, 100 },  // blue
    { 100,  50,   0 },  // orange
    {   0,   0, 100 },  // blue
    { 100,  50,   0 },  // orange
    {   0,   0, 100 },  // blue
    { 100,  50,   0 },  // orange
    {   0,   0, 100 },  // blue
    { 100,  50,   0 },  // orange
    
    // 4th palette: brights, less yellows
    { 100,  10, 100 },  // hot purple
    { 100,  10, 100 },  // hot purple
    { 100,  10, 100 },  // hot purple
    { 100,  10, 100 },  // hot purple
    { 100,  10, 100 },  // hot purple
    { 100,  10, 100 },  // hot purple
    { 100,  10, 100 },  // hot purple
    { 100,  10, 100 },  // hot purple
    { 100,  10, 100 },  // hot purple
    { 100,  10, 100 },  // hot purple
    { 100,  10, 100 },  // hot purple
    { 100,  10, 100 },  // hot purple
    { 100,  10, 100 },  // hot purple
    { 100,  10, 100 },  // hot purple
    { 100,  10, 100 },  // hot purple
    { 100,  10, 100 },  // hot purple
    { 100,  10, 100 },  // hot purple
    { 100,  10, 100 },  // hot purple
    { 100,  10, 100 },  // hot purple
    { 100,  10, 100 },  // hot purple
    { 100,  10, 100 },  // hot purple
    { 100,  10, 100 },  // hot purple
    { 100,  10, 100 },  // hot purple
    { 100,  10, 100 },  // hot purple
    
    // 5th palette: Rat Fink 
    {   0, 100,   0 },  // green
    {   0, 100,   0 },  // green
    {   0, 100,   0 },  // green
    {   0, 100,   0 },  // green
    { 100,   0,  25 },  // pink
    { 100,   0,  25 },  // pink
    { 100,   0,  25 },  // pink
    { 100,   0,  25 },  // pink
    {   0, 100,   0 },  // green
    { 100,   0,  25 },  // pink
    {   0, 100,   0 },  // green
    {   0, 100,   0 },  // green
    { 100,   0,  25 },  // pink
    { 100,   0,  25 },  // pink
    { 100,   0,  25 },  // pink
    {   0, 100,   0 },  // green
    { 100,   0,  25 },  // pink
    {   0, 100,   0 },  // green
    { 100,   0,  25 },  // pink
    {   0, 100,   0 },  // green
    { 100,   0,  25 },  // pink
    { 100,   0,  25 },  // pink
    {   0, 100,   0 },  // green
    { 100,   0,  25 },  // pink
   
    // 6th palette: Simply Red.
    { 100,   0,   0 },  // red
    { 100,   0,   0 },  // red
    { 100,   0,   0 },  // red
    { 100,   0,   0 },  // red
    { 100,   0,   0 },  // red
    { 100,   0,   0 },  // red
    { 100,   0,   0 },  // red
    { 100,   0,   0 },  // red
    { 100,   0,   0 },  // red
    { 100,   0,   0 },  // red
    { 100,   0,   0 },  // red
    { 100,   0,   0 },  // red
    { 100,   0,   0 },  // red
    { 100,   0,   0 },  // red
    { 100,   0,   0 },  // red
    { 100,   0,   0 },  // red
    { 100,   0,   0 },  // red
    { 100,   0,   0 },  // red
    { 100,   0,   0 },  // red
    { 100,   0,   0 },  // red
    { 100,   0,   0 },  // red
    { 100,   0,   0 },  // red
    { 100,   0,   0 },  // red
    { 100,   0,   0 }   // red
    
};

// palette vars
Color color_palette[24];
byte palette_choice = 0;

// pattern vars
byte pattern_choice = 3;
boolean shimmy_even = true;

void setup() {
    if (DEBUG) {
        Serial.begin(9600);
    }
    
    pinMode(button2Pin, INPUT);
    pinMode(button3Pin, INPUT);
    
    // check for button press to enter LED count set mode
    button3State = digitalRead(button3Pin);
    
    if (button3State == HIGH) {
        set_LED_count_mode = true;
    }
    
    //TIMSK0 = 0;           // turn off timer0 for lower jitter
    ADCSRA = 0xe5;        // set the adc to free running mode 
    ADMUX = 0x40;         // use adc0
    DIDR0 = 0x01;         // turn off the digital input for adc0
    
    // initial color palette
    for (int i = 0; i < 24; i++) {
        color_palette[i].red = pgm_read_byte(&(palettes[i + (palette_color_count * palette_choice)].red));
        color_palette[i].green = pgm_read_byte(&(palettes[i + (palette_color_count * palette_choice)].green));
        color_palette[i].blue = pgm_read_byte(&(palettes[i + (palette_color_count * palette_choice)].blue));
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
        round(red   * brightness / 127),
        round(green * brightness / 127),
        round(blue  * brightness / 127)
    );
    
    if (DEBUG) {
        Serial.print(color_palette[peak_index - 1].red);
        Serial.print("\t");
        Serial.print(color_palette[peak_index - 1].green);
        Serial.print("\t");
        Serial.println(color_palette[peak_index - 1].blue);
    }
    
    for (int i=0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip_color);
    }
    strip.show();
}

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
            Serial.println("Button pressed!");
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
    
    while (button3State == HIGH) {
        // do nothing, we hold the violence!
        button3State = digitalRead(button3Pin);
    }
}

// start pattern functions
void bullet() {
    // a single LED running fast across the strip
    int j = 0;
    while (button2State == HIGH) {
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
        button2State = digitalRead(button2Pin);
    }
}

void snail() {
    // a single LED running slow across the strip
    int j = 0;
    while (button2State == HIGH) {
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
        button2State = digitalRead(button2Pin);
    }
}

void shimmy() {
    // evens then odds
    while (button2State == HIGH) {
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
        button2State = digitalRead(button2Pin);
    }
}

void fountain() {
    // start in the middle and grow both sides
    
    while (button2State == HIGH) {

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
            delay(60 + (2 * i)); // get progressively slower each iteration
        }

        button2State = digitalRead(button2Pin);
    }
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
    button2State = digitalRead(button2Pin);
    
    if (button2State == HIGH) {
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
                bullet();
            case 5:
                bullet();
            case 6:
                bullet();
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
    button3State = digitalRead(button3Pin);
    
    if (button3State == HIGH) {
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
