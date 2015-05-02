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
Color color_palette[24];

// each palette is 24 colors, all are stored in the same array
const struct Color palettes[168] PROGMEM = {
    // 1st palette: Rainbow
    {   5,   0,  90 },  // microviolet
    {   0,   0, 100 },  // blue
    {   0,   5,  90 },  // blue w/ green tint
    {   0,  12,  60 },  // greenish-blue
    {   0,  20,  43 },  // bluish-green
    {   0,  30,  30 },  // cyan
    {   0,  43,  20 },  // light cyan
    {   0,  60,  12 },  // sea green
    {   0,  90,   5 },  // light green
    {   0, 100,   0 },  // green
    {   5,  90,   0 },  // lime green
    {  12,  60,   0 },  // lime
    {  20,  43,   0 },  // yellow-lime
    {  30,  30,   0 },  // yellow
    {  43,  20,   0 },  // orange-yellow
    {  60,  12,   0 },  // orange
    {  90,   5,   0 },  // orange-red
    { 100,   0,   0 },  // red
    {  90,   0,   5 },  // almost red
    {  60,   0,  12 },  // red onion
    {  43,   0,  20 },  // purplish-red
    {  30,   0,  30 },  // purple
    {  20,   0,  43 },  // ultraviolet
    {  12,   0,  60 },  // violet

    // 2nd palette: warm colors
    {  47,  25,   0 },
    {  51,  24,   0 },
    {  55,  23,   0 },
    {  59,  21,   0 },
    {  63,  19,   0 },
    {  67,  18,   0 },
    {  71,  17,   0 },
    {  75,  15,   0 },
    {  78,  13,   0 },
    {  80,  11,   0 },
    {  81,   9,   0 },
    {  82,   7,   0 },
    {  83,   5,   0 },
    {  84,   4,   0 },
    {  85,   3,   0 },
    {  86,   2,   0 },
    {  88,   0,   0 },
    {  82,   0,   1 },
    {  80,   0,   2 },
    {  78,   0,   4 },
    {  76,   0,   7 },
    {  74,   0,   9 },
    {  72,   0,  11 },
    {  72,   0,  13 },
    
    // 3rd palette: cool colors
    {   9,  61,   0 },
    {   6,  67,   0 },
    {   3,  72,   0 },
    {   0,  77,   0 },
    {   0,  82,   0 },
    {   0,  77,   4 },
    {   0,  68,   8 },
    {   0,  61,  12 },
    {   0,  55,  15 },
    {   0,  50,  18 },
    {   0,  46,  21 },
    {   0,  42,  24 },
    {   0,  38,  27 },
    {   0,  34,  30 },
    {   0,  30,  34 },
    {   0,  26,  38 },
    {   0,  22,  42 },
    {   0,  18,  46 },
    {   0,  15,  50 },
    {   0,  12,  54 },
    {   0,   9,  59 },
    {   0,   6,  66 },
    {   0,   3,  71 },
    {   0,   0,  76 },
     
     // 4th palette: Blue-orange cream-cycle 
    {   0,   0, 100 },
    {  10,  10,  60 },
    {  15,  15,  30 },
    {  30,   6,   0 },
    {  60,  12,   0 },
    {  30,   6,   0 },
    {  15,  15,  30 },
    {  10,  10,  60 },
    {   0,   0, 100 },
    {  10,  10,  60 },
    {  15,  15,  30 },
    {  30,   6,   0 },
    {  60,  12,   0 },
    {  30,   6,   0 },
    {  15,  15,  30 },
    {  10,  10,  60 },
    {   0,   0, 100 },
    {  10,  10,  60 },
    {  15,  15,  30 },
    {  30,   6,   0 },
    {  60,  12,   0 },
    {  30,   6,   0 },
    {  15,  15,  30 },
    {  10,  10,  60 },
    
    // 5th palette: brights, less yellows
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
    
    // 6th palette: Rat Fink 
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
   
    // 7th palette: Simply Red.
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

