#include <Adafruit_NeoPixel.h>

#define PIN 8
#define PIN_MODESWITCH 10

Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, PIN, NEO_GRB + NEO_KHZ800);

typedef enum {
  QUIET = 0,
  SIGNAL_LEFT,
  SIGNAL_RIGHT,
  RAINBOW,
  LAST
} FlashMode;

FlashMode mode = QUIET;

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  pinMode(PIN_MODESWITCH, INPUT);
}

void loop() 
{
  switch(mode)
  {
    case SIGNAL_LEFT:
      colorWipe(strip.Color(0, 255, 0), 500);
      delay(1000);
    break;

    case SIGNAL_RIGHT:
      colorWipe(strip.Color(0, 0, 255), 500);
      delay(1000);
    break;
    
    case RAINBOW:
      //Some example procedures showing how to display to the pixels:
      //colorWipe(strip.Color(255, 0, 0), 500); // Red
      //colorWipe(strip.Color(0, 255, 0), 500); // Green
      //colorWipe(strip.Color(0, 0, 255), 500); // Blue
      rainbowCycleFlashy(20);
    break;
  }

  if(digitalRead(PIN_MODESWITCH))
  {
    int m = mode;
    
    m++;
    mode = m;
    
    if(mode == LAST)
    {
      mode = QUIET;
    }
    delay(1000);
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*1; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, attention-grabbing flashing
void rainbowCycleFlashy(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*1; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
    
    strip.setPixelColor(0, 0x00);
    strip.show();
    delay(wait*8);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}
