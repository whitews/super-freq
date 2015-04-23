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
const struct Color palettes[144] PROGMEM = {
    // 2nd palette: full spectrum of colors 2
    {  14,   0,  61 },  // microviolet
    {   0,   0,  83 },  // blue
    {   0,  15,  63 },  // blue w/ green tint
    {   0,  27,  47 },  // greenish-blue
    {   0,  37,  27 },  // bluish-green
    {   0,  47,  15 },  // cyan
    {   0,  55,   8 },  // light cyan
    {   0,  63,   5 },  // sea green
    {   0,  73,   3 },  // light green
    {   0,  81,   0 },  // green
    {  14,  57,   0 },  // lime green
    {  27,  45,   0 },  // lime
    {  37,  37,   0 },  // yellow-lime
    {  47,  27,   0 },  // yellow
    {  63,  19,   0 },  // orange-yellow
    {  79,  15,   0 },  // orange
    {  81,   7,   0 },  // orange-red
    {  83,   0,   0 },  // red
    {  80,   0,   2 },  // almost red
    {  73,   0,  11 },  // red onion
    {  63,   0,  21 },  // purplish-red
    {  47,   0,  27 },  // purple
    {  37,   0,  37 },  // ultraviolet
    {  27,   0,  47 },  // violet

    // 2nd palette: warm colors
    {  47,  25,   0},
    {  51,  24,   0},
    {  55,  23,   0},
    {  59,  21,   0},
    {  63,  19,   0},
    {  67,  18,   0},
    {  71,  17,   0},
    {  75,  15,   0},
    {  78,  13,   0},
    {  80,  11,   0},
    {  81,   9,   0},
    {  82,   7,   0},
    {  83,   5,   0},
    {  84,   4,   0},
    {  85,   3,   0},
    {  86,   2,   0},
    {  88,   0,   0},
    {  82,   0,   1},
    {  80,   0,   2},
    {  78,   0,   4},
    {  76,   0,   7},
    {  74,   0,   9},
    {  72,   0,  11},
    {  72,   0,  13},
     
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

