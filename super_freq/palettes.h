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
    {  80,   0,   2 },  // pink
    {  73,   0,  11 },  // bright pink
    {  63,   0,  21 },  // hot pink
    {  47,   0,  27 },  // purple
    {  37,   0,  37 },  // hot purple
    {  27,   0,  47 },  // white hot

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

