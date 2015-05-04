void calculateFreq() {
    for (int i = 0 ; i < FHT_N; i += 1) { // save samples
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
         * SAMPLE_RATE / SKIP_MULT
         */
        for (int j = 0; j < SKIP_MULT; j++) {
            while(!(ADCSRA & B00010000));  // wait for adc to be ready
            ADCSRA = B11110100;            // restart adc
        }

        byte low_byte = ADCL;              // fetch ADC data (low byte)
        int k = (ADCH << 8) | low_byte;    // combine low/high bytes to form into an int
        k -= 0x0200;               // form into a signed int
        k <<= 6;                   // form into a 16b signed int
        fht_input[i] = k;          // put real data into even bins
    }
    fht_window();                  // window the data for better frequency response
    fht_reorder();                 // reorder the data before doing the fft
    fht_run();                     // process the data in the fft
    fht_mag_lin();                 // take the output of the fft
}
