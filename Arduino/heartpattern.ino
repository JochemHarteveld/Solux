#include <FastLED.h>

#define LED_PIN     2
#define NUM_STRIPS  5
#define NUM_LEDS    30
#define LED_TYPE    SK6812
#define COLOR_ORDER GRB

CRGB leds[NUM_STRIPS * NUM_LEDS];
bool bitmap[NUM_STRIPS][NUM_LEDS];

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_STRIPS * NUM_LEDS).setCorrection(TypicalLEDStrip);
  
  // Initialise bitmap by turning all LEDs off
  clearBitmap();
}

void clearBitmap() {
  for (int i = 0; i < NUM_STRIPS; i++) {
    for (int j = 0; j < NUM_LEDS; j++) {
      bitmap[i][j] = false;
    }
  }
}

void updateLEDs() {
  for (int strip = 0; strip < NUM_STRIPS; strip++) {
    for (int led = 0; led < NUM_LEDS; led++) {
      if (bitmap[strip][led]) {
        leds[strip * NUM_LEDS + led] = CRGB::Red;  // On LEDs are red (heart color)
      } else {
        leds[strip * NUM_LEDS + led] = CRGB::Black;  // Off LEDs are black (off)
      }
    }
  }
  FastLED.show();
}

void createHeartPattern(int offset) {
  clearBitmap();
  
  // Define the heart pattern (flipped 90 degrees)
  const bool heartPattern[5][11] = {
    {0, 0, 0, 0, 1, 1, 1, 1, 1, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 1, 1, 1, 1, 1, 0}
  };
  
  // Starting position to find centre
  int startX = (NUM_LEDS - 5) / 2;
  
  // Map the heart pattern to the LED strips
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 11; j++) {
      if (i % 2 == 0) {
        // Even strips: downwards
        bitmap[i][startX + j + offset - 1] = heartPattern[i][j];
      } else {
        // Odd strips: upwards
        bitmap[i][startX + 5 - j - offset] = heartPattern[i][j];
      }
    }
  }
}

void heartStep() {
    int maxOffset = 6; // position above centre
  int minOffset = -6; // position below centre
  int offset = 0; // current position
  int step = 1;

  while (true) {
    createHeartPattern(offset);
    updateLEDs();
    
    // "gradual" speed increase, speeds up below centre
    if (offset > 0) {
        delay(100);
    } else {
        delay(75);
    }

    offset += step;

    // Change direction when reaching the maximum or minimum offset
    if (offset >= maxOffset || offset <= minOffset) {
      step = -step;
    }
  }
}

void loop() {
  heartStep();
}
