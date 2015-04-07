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
    // it's roughly the 13 ADC clock cycles (104us) w/some padding
    delay(150);
    
    byte low_byte = ADCL;
    int value = (ADCH << 8) | low_byte;
    
    // return ADC to A0 for audio
    ADMUX = B01000000;
    
    // we switched pins, so another delay is on order
    delay(150);
    
    return value;
}

int determine_switch_selection(int analog_value) {
    if (analog_value >= 600 && analog_value <= 643) {
        return 1;
    } 
    else if (analog_value > 643 && analog_value <= 679) {
        return 2;
    } 
    else if (analog_value > 679 && analog_value <= 715) {
        return 3;
    } 
    else if (analog_value > 715 && analog_value <= 752) {
        return 4;
    } 
    else if (analog_value > 752 && analog_value <= 788) {
        return 5;
    } 
    else if (analog_value > 788 && analog_value <= 824) {
        return 6;
    } 
    else if (analog_value > 824 && analog_value <= 860) {
        return 7;
    } 
    else if (analog_value > 860 && analog_value <= 896) {
        return 8;
    } 
    else if (analog_value > 896 && analog_value <= 932) {
        return 9;
    } 
    else if (analog_value > 932 && analog_value <= 967) {
        return 10;
    } 
    else if (analog_value > 967 && analog_value <= 1005) {
        return 11;
    } 
    else if (analog_value > 1005 && analog_value <= 1023) {
        return 12;
    }
    
    // if no analog value matched a selection, return 0
    return 0;
}
