#include <FastLED.h>

#define LED_PIN     2    // Change this to the GPIO pin connected to the data pin of the LED strip
#define NUM_LEDS    300  // Change this to the number of LEDs in your strip
#define LED_TYPE    SK6812
#define COLOR_ORDER GRB

#define LDR_PIN     34   // Analog pin connected to LDR
#define NUM_SAMPLES 10   // Number of samples for the moving average

CRGB leds[NUM_LEDS];
int brightnessSamples[NUM_SAMPLES];  // Array to store LDR samples
int sampleIndex = 0;                 // Index for the current sample
int currentBrightness = 0;           // Variable to store the current brightness level
const int maxBrightness = 255;
const int minBrightness = maxBrightness / 3; // Minimum brightness is set to 1/3 of the maximum

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  pinMode(LDR_PIN, INPUT); // Set the LDR pin as input
  
  // Initialize the brightnessSamples array
  for (int i = 0; i < NUM_SAMPLES; i++) {
    brightnessSamples[i] = 0;
  }
}

void loop() {
  // Read the value from the LDR
  int ldrValue = analogRead(LDR_PIN);
  
  // Print the LDR value to the Serial Monitor
  Serial.print("LDR Value: ");
  Serial.println(ldrValue);

  // Add the new LDR value to the samples array
  brightnessSamples[sampleIndex] = ldrValue;
  sampleIndex = (sampleIndex + 1) % NUM_SAMPLES; // Update the index in a circular manner

  // Calculate the average LDR value
  long sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += brightnessSamples[i];
  }
  int averageLDRValue = sum / NUM_SAMPLES;
  
  // Map the average LDR value to a brightness level (minBrightness to maxBrightness)
  int targetBrightness = map(averageLDRValue, 0, 4095, maxBrightness, minBrightness);
  
  // Apply the brightness to the LEDs
  FastLED.setBrightness(targetBrightness);

  // Define a color palette
  CRGBPalette16 palette = RainbowColors_p;

  // Move the gradient along the LED strip
  for (int j = 0; j < 256; j++) {
    for (int i = 0; i < NUM_LEDS; i++) {
      int colorIndex = (i * 256 / NUM_LEDS + j) % 256;
      leds[i] = ColorFromPalette(palette, colorIndex);
    }
    FastLED.show();
    delay(10); // Adjust delay for speed of movement
  }
}
