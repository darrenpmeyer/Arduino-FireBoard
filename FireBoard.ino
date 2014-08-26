#include <FastLED.h>

/* NeoPixel Shield data pin is always 6. Change for other boards */
#define CONTROL_PIN 6

/* Board shape and size configuration. Sheild is 8x5, 40 pixels */
#define HEIGHT 5
#define WIDTH 8
#define NUM_LEDS HEIGHT*WIDTH

CRGB leds[NUM_LEDS];
CRGBPalette16 gPal;

void setup() {
  FastLED.addLeds<NEOPIXEL, CONTROL_PIN>(leds, NUM_LEDS);
  
  /* Set a black-body radiation palette
     This comes from FastLED */
  gPal = HeatColors_p; 
  
  /* Clear display before starting */
  FastLED.clear();
  FastLED.show();
  FastLED.delay(1500); // Sanity before start
}

/* Refresh rate. Higher makes for flickerier
   Recommend small values for small displays */
#define FPS 17
#define FPS_DELAY 1000/FPS

void loop() {
  random16_add_entropy( random() ); // We chew a lot of entropy
  
  Fireplace();

  FastLED.show();
  FastLED.delay(FPS_DELAY); //
}

/* Rate of cooling. Play with to change fire from
   roaring (smaller values) to weak (larger values) */
#define COOLING 55  

/* How hot is "hot"? Increase for brighter fire */
#define HOT 180
#define MAXHOT HOT*HEIGHT

void Fireplace () {
  static unsigned int spark[WIDTH]; // base heat
  CRGB stack[WIDTH][HEIGHT];        // stacks that are cooler
 
  // 1. Generate sparks to re-heat
  for( int i = 0; i < WIDTH; i++) {
    if (spark[i] < HOT ) {
      int base = HOT * 2;
      spark[i] = random16( base, MAXHOT );
    }
  }
  
  // 2. Cool all the sparks
  for( int i = 0; i < WIDTH; i++) {
    spark[i] = qsub8( spark[i], random8(0, COOLING) );
  }
  
  // 3. Build the stack
  /*    This works on the idea that pixels are "cooler"
        as they get further from the spark at the bottom */
  for( int i = 0; i < WIDTH; i++) {
    unsigned int heat = constrain(spark[i], HOT/2, MAXHOT);
    for( int j = HEIGHT-1; j >= 0; j--) {
      /* Calculate the color on the palette from how hot this
         pixel is */
      byte index = constrain(heat, 0, HOT);
      stack[i][j] = ColorFromPalette( gPal, index );
      
      /* The next higher pixel will be "cooler", so calculate
         the drop */
      unsigned int drop = random8(0,HOT);
      if (drop > heat) heat = 0; // avoid wrap-arounds from going "negative"
      else heat -= drop;
 
      heat = constrain(heat, 0, MAXHOT);
    }
  }
  
  // 4. map stacks to led array
  for( int i = 0; i < WIDTH; i++) {
  for( int j = 0; j < HEIGHT; j++) {
     leds[(j*WIDTH) + i] = stack[i][j];
  }
  }
  
}
