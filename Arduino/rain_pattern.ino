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
  
  // Initialize bitmap (all LEDs off)
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
        leds[strip * NUM_LEDS + led] = CRGB::Blue;  // On LEDs are blue
      } else {
        leds[strip * NUM_LEDS + led] = CRGB::Black;  // Off LEDs are black (off)
      }
    }
  }
  FastLED.show();
}

// TLDR 1/8 for a raindrop to appear per strip from top to bottom
void createRainPattern() {
  for (int strip = 0; strip < NUM_STRIPS; strip++) {
    if (strip % 2 == 0) {  // For rows 0, 2, and 4
      // Shift existing raindrops up
      for (int led = 0; led < NUM_LEDS - 1; led++) {
        bitmap[strip][led] = bitmap[strip][led + 1];
      }
      // Randomly add new raindrops at the bottom
      bitmap[strip][NUM_LEDS - 1] = random(8) == 0;  // 1 in 8 chance of a new raindrop
    } else {  // For rows 1 and 3
      // Shift existing raindrops down
      for (int led = NUM_LEDS - 1; led > 0; led--) {
        bitmap[strip][led] = bitmap[strip][led - 1];
      }
      // Randomly add new raindrops at the top
      bitmap[strip][0] = random(8) == 0;  // 1 in 8 chance of a new raindrop
    }
  }
}

void loop() {
  createRainPattern();
  updateLEDs();
  delay(100);  // Adjust to change animation speed
}
