// Color stuff
byte red = 0;
byte green = 0;
byte blue = 0;

struct Color {
    byte red;
    byte green;
    byte blue;
};

byte palette_color_count = 30;
Color current_palette[30];

// each palette is 32 colors, all are stored in the same array
const struct Color palettes[120] PROGMEM = {
    // 1st palette: Rainbow
    {   7,   0,  87 },
    {   0,   0, 100 },  // blue
    {   0,   7,  87 },
    {   0,  13,  73 },
    {   0,  20,  60 },
    {   0,  26,  46 },
    {   0,  33,  33 },  // cyan
    {   0,  46,  26 },
    {   0,  60,  20 },
    {   0,  73,  13 },
    {   0,  87,   7 },
    {   0, 100,   0 },  // green
    {   7,  87,   0 },
    {  13,  73,   0 },
    {  20,  60,   0 },
    {  26,  46,   0 },
    {  33,  33,   0 },  // yellow
    {  46,  26,   0 },
    {  60,  20,   0 },
    {  73,  13,   0 },
    {  87,   7,   0 },
    { 100,   0,   0 },  // red
    {  87,   0,   7 },
    {  73,   0,  13 },
    {  60,   0,  20 },
    {  46,   0,  26 },
    {  33,   0,  33 },  // purple
    {  26,   0,  46 },
    {  20,   0,  60 },
    {  13,   0,  73 },  // violet

    // 2nd palette: cool colors
    {   7,   0,  87 },
    {   3,   0,  93 },
    {   0,   0, 100 },  // blue
    {   0,   3,  93 },
    {   0,   7,  87 },
    {   0,  10,  80 },
    {   0,  13,  73 },
    {   0,  17,  67 },
    {   0,  20,  60 },
    {   0,  23,  53 },
    {   0,  26,  46 },
    {   0,  30,  40 },
    {   0,  33,  33 },  // cyan
    {   0,  40,  30 },
    {   0,  46,  26 },
    {   0,  53,  23 },
    {   0,  60,  20 },
    {   0,  67,  17 },
    {   0,  73,  13 },
    {   0,  80,  10 },
    {   0,  87,   7 },
    {   0,  93,   3 },
    {   0, 100,   0 },  // green
    {   3,  93,   0 },
    {   7,  87,   0 },
    {  10,  80,   0 },
    {  13,  73,   0 },
    {  17,  67,   0 },
    {  20,  73,   0 },
    
    // 3rd palette: warm colors
    {  20,  60,   0 },
    {  23,  53,   0 },
    {  26,  46,   0 },
    {  30,  40,   0 },
    {  33,  33,   0 },  // yellow
    {  40,  30,   0 },
    {  46,  26,   0 },
    {  53,  23,   0 },
    {  60,  20,   0 },
    {  67,  17,   0 },
    {  73,  13,   0 },
    {  80,  10,   0 },
    {  87,   7,   0 },
    {  93,   3,   0 },
    { 100,   0,   0 },  // red
    {  93,   0,   3 },
    {  87,   0,   7 },
    {  80,   0,  10 },
    {  73,   0,  13 },
    {  67,   0,  17 },
    {  60,   0,  20 },
    {  53,   0,  23 },
    {  46,   0,  26 },
    {  40,   0,  30 },
    {  33,   0,  33 },  // purple
    {  30,   0,  40 },
    {  26,   0,  46 },
    {  23,   0,  53 },
    {  20,   0,  60 },
    {  17,   0,  67 },
     
     // 4th palette: Blue-orange cream-cycle 
    {   0,   0, 100 },
    {  10,  10,  75 },
    {  20,  20,  50 },
    {  30,  30,  30 },
    {  40,  20,   0 },
    {  50,  10,   0 },
    {  60,  20,   0 },
    {  50,  10,   0 },
    {  40,  20,   0 },
    {  30,  30,  30 },
    {  20,  20,  50 },
    {  10,  10,  75 },
    {   0,   0, 100 },
    {  10,  10,  75 },
    {  20,  20,  50 },
    {  30,  30,  30 },
    {  40,  20,   0 },
    {  50,  10,   0 },
    {  60,  20,   0 },
    {  50,  10,   0 },
    {  40,  20,   0 },
    {  30,  30,  30 },
    {  20,  20,  50 },
    {  10,  10,  75 },
    {   0,   0, 100 },
    {  10,  10,  75 },
    {  20,  20,  50 },
    {  30,  30,  30 },
    {  40,  20,   0 },
    {  50,  10,   0 }
};

