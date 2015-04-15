// switch interrupt routine for the palette rotary switch
// keep as lean as possible, just set the switch_changed flag
void palette_switch_changed()
{
    palette_switch_flag = true;
}

// switch interrupt routine for the pattern rotary switch
// keep as lean as possible, just set the switch_changed flag
void pattern_switch_changed()
{
    pattern_switch_flag = true;
}

int read_analog_pin(int pin) {
    // change ADC pin
    if (pin == A1) {
        ADMUX = B01000001;
    } else if (pin == A2) {
        ADMUX = B01000010;
    }
    
    // delay is necessary when switching ADC pins,
    // else the other pins will still have non-zero values
    // it's roughly the 13 ADC clock cycles (104us),
    // but it looks like we can cheat a bit for some performance
    // improvement
    delay(60);
    
    byte low_byte = ADCL;
    int value = (ADCH << 8) | low_byte;
    
    // return ADC to A0 for audio
    ADMUX = B01000000;
    
    // we switched pins, so another delay is on order
    delay(60);
    
    return value;
}

int determine_switch_selection(int analog_value) {
    // threshold for digital interrupt is roughly 2.7V of 5V, 
    // so start at 552...our hardware voltage divider uses a 
    // 14.7kOhm prior to the 1k(x11) resistor array
    if (analog_value >= 552 && analog_value <= 599) {
        return 1;
    } 
    else if (analog_value > 599 && analog_value <= 639) {
        return 2;
    } 
    else if (analog_value > 639 && analog_value <= 678) {
        return 3;
    } 
    else if (analog_value > 678 && analog_value <= 718) {
        return 4;
    } 
    else if (analog_value > 718 && analog_value <= 757) {
        return 5;
    } 
    else if (analog_value > 757 && analog_value <= 797) {
        return 6;
    } 
    else if (analog_value > 797 && analog_value <= 838) {
        return 7;
    } 
    else if (analog_value > 828 && analog_value <= 878) {
        return 8;
    } 
    else if (analog_value > 878 && analog_value <= 920) {
        return 9;
    } 
    else if (analog_value > 920 && analog_value <= 960) {
        return 10;
    } 
    else if (analog_value > 960 && analog_value <= 1001) {
        return 11;
    } 
    else if (analog_value > 1001 && analog_value <= 1023) {
        return 12;
    }
    
    // if no analog value matched a selection, return 0
    return 0;
}
