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

void changeColorPalette() {
    // re-populate colors for color_palette from new palette choice
    if (palette_choice != new_palette_choice - 1) {
        palette_choice = new_palette_choice - 1;
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
    }
}
