#include <FastLED.h>

#define LED_PIN     2    // Change this to the GPIO pin connected to the data pin of the LED strip
#define NUM_LEDS    30   // Change this to the number of LEDs in your strip
#define LED_TYPE    SK6812
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
}

void loop() {
  Serial.println("Moving gradient along the LED strip");

  // Define a color palette
  CRGBPalette16 palette = RainbowColors_p;

  // Move the gradient along the LED strip
  for (int j = 0; j < 256; j++) {
    for (int i = 0; i < NUM_LEDS; i++) {
      int colorIndex = (i * 256 / NUM_LEDS + j) % 256;
      leds[i] = ColorFromPalette(palette, colorIndex);
    }
    FastLED.show();
    delay(20); // Adjust delay for speed of movement
  }
}
