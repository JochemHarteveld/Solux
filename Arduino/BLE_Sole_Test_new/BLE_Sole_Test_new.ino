#include <ArduinoBLE.h>
#include <FastLED.h>
#include <arduinoFFT.h>

#define LED_PIN     2    // Change this to the GPIO pin connected to the data pin of the LED strip
#define NUM_LEDS_PER_STRIP 30
#define NUM_STRIPS 5
#define NUM_LEDS (NUM_LEDS_PER_STRIP * NUM_STRIPS)
#define LED_TYPE    SK6812
#define COLOR_ORDER GRB

#define MIC_PIN 35
#define LDR_PIN     34   // Analog pin connected to LDR
#define NUM_SAMPLES 10   // Number of samples for the moving average

CRGB leds[NUM_LEDS];

BLEService bleService("19b10010-e8f2-537e-4f6c-d104768a1214"); // BLE service UUID
BLECharacteristic bleCharacteristic("19b10011-e8f2-537e-4f6c-d104768a1214", BLERead | BLEWrite, 512); // BLE characteristic UUID with larger size

int ldrValues[NUM_SAMPLES]; // Array to store LDR readings
int sampleIndex = 0;        // Current sample index
bool autoBrightness = false; // Auto brightness flag
int brightnessPercentage = 0; // Brightness percentage
int position = 1; // LED position flag, 0 for on, 1 for off
int currentPattern = 0; // Current pattern
int currentPalette = 0; // Current color palette
unsigned long lastUpdate = 0; // Last update time
const int updateInterval = 50; // Update interval in milliseconds


const uint16_t samples = 512;       // Must be a power of 2
const double samplingFrequency = 10000; // Hz, must be less than 10000 due to ADC limitations
unsigned int samplingPeriodUs;
unsigned long microseconds;

double vReal[samples];
double vImag[samples];

ArduinoFFT FFT = ArduinoFFT(vReal, vImag, samples, samplingFrequency);

void setup() {
  Serial.begin(9600);
  
  // Initialize FastLED
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(50); // Set initial brightness

  samplingPeriodUs = round(1000000 * (1.0 / samplingFrequency));

  // Initialize BLE
  if (!BLE.begin()) {
    Serial.println("Failed to initialize BLE!");
    while (1);
  }

  BLE.setLocalName("ESP32_BLE_Test");
  BLE.setAdvertisedService(bleService);

  bleService.addCharacteristic(bleCharacteristic);
  BLE.addService(bleService);

  bleCharacteristic.setValue("Ready");
  BLE.advertise();

  Serial.println("BLE initialized and advertising...");

  // Initialize LDR readings
  for (int i = 0; i < NUM_SAMPLES; i++) {
    ldrValues[i] = analogRead(LDR_PIN);
  }
}

int getAverageLDRValue() {
  ldrValues[sampleIndex] = analogRead(LDR_PIN);
  sampleIndex = (sampleIndex + 1) % NUM_SAMPLES;
  
  long sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += ldrValues[i];
  }
  
  return sum / NUM_SAMPLES;
}

void setLEDColorPalette(int palette) {
  currentPalette = palette;
  switch (palette) {
    case 0: // Blue
      fill_solid(leds, NUM_LEDS, CRGB::Blue);
      break;
    case 1: // Rainbow
      fill_rainbow(leds, NUM_LEDS, 0, 255 / NUM_LEDS);
      break;
    case 2: // Ocean
      fill_gradient_RGB(leds, NUM_LEDS, CRGB::Blue, CRGB::Aqua);
      break;
    case 3: // Lava
      fill_gradient_RGB(leds, NUM_LEDS, CRGB::Red, CRGB::Orange);
      break;
    case 4: // Forest
      fill_gradient_RGB(leds, NUM_LEDS, CRGB::Green, CRGB::ForestGreen);
      break;
    case 5: // Love
      fill_gradient_RGB(leds, NUM_LEDS, CRGB::Red, CRGB::Pink);
      break;
    default:
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      break;
  }
  FastLED.show();
}

void updateBrightness() {
  int currentBrightness;
  if (autoBrightness) {
    int ldrValue = getAverageLDRValue();
    currentBrightness = map(ldrValue, 0, 4095, 0, 255);
  } else {
    currentBrightness = map(brightnessPercentage, 0, 100, 0, 255);
  }
  FastLED.setBrightness(currentBrightness);
  FastLED.show();
}

void patternBlinking() {
  static bool brighter = false;
  static uint8_t originalBrightness = 0; // Store the original brightness
  
  if (millis() - lastUpdate > 500) { // Toggle every 500 milliseconds
    lastUpdate = millis();
    brighter = !brighter;
    if (brighter) {
      // Store the current brightness
      originalBrightness = FastLED.getBrightness();
      // Increase brightness
      FastLED.setBrightness(originalBrightness); // Increase by 75%
      FastLED.show();
      delay(80); // Adjust delay as needed
    } else {
      // Decrease brightness
      FastLED.setBrightness(originalBrightness * 0.5); // Decrease by 25%
      FastLED.show();
      // Restore original brightness after a short delay
      delay(80); // Adjust delay as needed
      FastLED.setBrightness(originalBrightness);
      FastLED.show();
      delay(80);
    }
  }
}

void patternSynced() {
    // Step 1: Read the microphone
  for (int i = 0; i < samples; i++) {
    microseconds = micros();    
    vReal[i] = analogRead(MIC_PIN); // A conversion takes about 1uS on an ESP32
    vImag[i] = 0;
    while (micros() < (microseconds + samplingPeriodUs)) {
      // do nothing to wait
    }
  }

  // Step 2: Compute the FFT
  FFT.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.compute(FFT_FORWARD);
  FFT.complexToMagnitude();

  // Step 3: Find the peak frequency
  double peak = FFT.majorPeak();
  Serial.print("Peak Frequency: ");
  Serial.println(peak);

  // Step 4: Compute volume
  double volume = 0;
  for (int i = 2; i < (samples / 2); i++) {
    volume += vReal[i];
  }
  volume = volume / (samples / 2);
  Serial.print("Volume: ");
  Serial.println(volume);

  // Normalize the volume to the range of 0 to 1023
  double normalizedVolume = volume / 1023.0;
  Serial.print("Normalized Volume: ");
  Serial.println(normalizedVolume);

  // Step 5: Update the LEDs based on volume and pitch
  updateLEDs(normalizedVolume, peak);  // Using normalized volume

  // Step 6: Show the LEDs
  FastLED.show();
}

void updateLEDs(double volume, double pitch) {
  // Define the 2D array to represent the LED matrix
  bool ledMatrix[NUM_STRIPS][NUM_LEDS_PER_STRIP] = {false};

  // Map volume to the height of the bar
  int barHeight = map(volume * 1023 / 3, 0, 1023, 0, NUM_LEDS_PER_STRIP / 2);

  // Map pitch to the strip index (range from 0 to NUM_STRIPS - 1)
  int stripIndex = map(pitch, 0, samplingFrequency / 2, 0, NUM_STRIPS / 2);

  // Clear the LED array
  memset(leds, 0, NUM_LEDS * sizeof(CRGB));

  // Update the ledMatrix based on volume and pitch without overlapping
  for (int strip = 0; strip < NUM_STRIPS; strip++) {
    for (int i = 0; i < barHeight; i++) {
      if (strip % 2 == 0) {
        ledMatrix[strip][i] = true;
      } else {
        ledMatrix[strip][NUM_LEDS_PER_STRIP - 1 - i] = true;
      }
    }
  }

  // Convert the 2D ledMatrix to the FastLED CRGB array
  for (int strip = 0; strip < NUM_STRIPS; strip++) {
    for (int i = 0; i < NUM_LEDS_PER_STRIP; i++) {
      if (ledMatrix[strip][i]) {
        int ledIndex = strip * NUM_LEDS_PER_STRIP + i;
        // Example: set color directly based on currentPalette
        switch (currentPalette) {
          case 0:
            leds[ledIndex] = CRGB::Blue;
            break;
          case 1:
            leds[ledIndex] = CHSV(i * (255 / NUM_LEDS), 255, 255); // Example for rainbow effect
            break;
          // Add cases for other palettes as needed
          default:
            leds[ledIndex] = CRGB::Black; // Default case
            break;
        }
      }
    }
  }

  FastLED.show();
}

void patternRain() {
  static int dripPositions[NUM_LEDS] = {0}; // Array to store positions of the active drips
  static int dripCount = 0; // Number of active drips
  if (millis() - lastUpdate > 100) {
    lastUpdate = millis();
    
    // Move all drips down by 1 LED
    for (int i = 0; i < dripCount; i++) {
      dripPositions[i]++;
      if (dripPositions[i] >= NUM_LEDS) {
        dripPositions[i] = 0;
      }
    }
    
    // Randomly add a new drip if the conditions are met
    if (dripCount == 0 || dripPositions[dripCount - 1] >= 3 + random(3)) {
      dripPositions[dripCount++] = 0; // Add a new drip at the top
      if (dripCount >= NUM_LEDS) {
        dripCount = 0; // Reset if we have too many drips
      }
    }
    
    // Clear the LEDs
    fill_solid(leds, NUM_LEDS, CRGB::Black);

    // Light up the drips
    for (int i = 0; i < dripCount; i++) {
      leds[dripPositions[i]] = leds[dripPositions[i]].lerp8(CRGB::White, 192); // Make the drip brighter
      if (dripPositions[i] + 1 < NUM_LEDS) {
        leds[dripPositions[i] + 1] = leds[dripPositions[i] + 1].lerp8(CRGB::White, 192); // Make the next LED brighter
      }
    }

    FastLED.show();
  }
}

void patternFire() {
  if (millis() - lastUpdate > 50) {
    lastUpdate = millis();
    for (int i = 0; i < NUM_LEDS; i++) {
      // Choose a random value to adjust brightness, either up or down
      int brightnessAdjust = random8(128, 255);
      leds[i] = leds[i].lerp8(CRGB(CHSV(random8(0, 25), 255, brightnessAdjust)), 64);
    }
    FastLED.show();
  }
}

void patternGrowing() {
  static int growingIndex = 0;
  static bool growing = true;
  if (millis() - lastUpdate > updateInterval) {
    lastUpdate = millis();

    if (growing) {
      leds[growingIndex] = leds[growingIndex].lerp8(CRGB::White, 64);
      if (growingIndex == NUM_LEDS - 1) {
        growing = false;
      } else {
        growingIndex++;
      }
    } else {
      leds[growingIndex] = leds[growingIndex].lerp8(CRGB::Black, 64);
      if (growingIndex == 0) {
        growing = true;
      } else {
        growingIndex--;
      }
    }
    FastLED.show();
  }
}

void patternHeartPulse() {
  static int pulseState = 0;
  static int brightness = 0;
  if (millis() - lastUpdate > updateInterval) {
    lastUpdate = millis();
    if (pulseState == 0) {
      brightness += 15;
      if (brightness >= 255) {
        brightness = 255;
        pulseState = 1;
      }
    } else {
      brightness -= 15;
      if (brightness <= 0) {
        brightness = 0;
        pulseState = 0;
      }
    }
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = leds[i].lerp8(CRGB::Red, brightness);
    }
    FastLED.setBrightness(brightness);
    FastLED.show();
  }
}

void handleBLEData() {
  if (bleCharacteristic.written()) {
    int valueLength = bleCharacteristic.valueLength();
    char *valueChars = new char[valueLength + 1]; // Allocate buffer dynamically
    const uint8_t *valueBytes = bleCharacteristic.value();
    memcpy(valueChars, valueBytes, valueLength);
    valueChars[valueLength] = '\0'; // Null terminate the string
    Serial.println(valueChars);
    
    // Parse the received data
    char *token = strtok(valueChars, ",");
    position = atoi(token);
    token = strtok(NULL, ",");
    autoBrightness = atoi(token);
    token = strtok(NULL, ",");
    brightnessPercentage = atoi(token);
    token = strtok(NULL, ",");
    currentPattern = atoi(token);
    token = strtok(NULL, ",");
    int colorPalette = atoi(token);

    // Print the decoded options
    Serial.print("Position: ");
    Serial.println(position == 0 ? "On" : "Off");
    Serial.print("Auto Brightness: ");
    Serial.println(autoBrightness == 0 ? "Off" : "On");
    Serial.print("Brightness Percentage: ");
    Serial.println(brightnessPercentage);
    Serial.print("Pattern: ");
    switch (currentPattern) {
      case 0:
        Serial.println("Blinking");
        break;
      case 1:
        Serial.println("Synced");
        break;
      case 2:
        Serial.println("Rain");
        break;
      case 3:
        Serial.println("Fire");
        break;
      case 4:
        Serial.println("Growing");
        break;
      case 5:
        Serial.println("Heart Pulse");
        break;
      default:
        Serial.println("Unknown");
    }
    Serial.print("Color Palette: ");
    switch (colorPalette) {
      case 0:
        Serial.println("Blue");
        break;
      case 1:
        Serial.println("Rainbow");
        break;
      case 2:
        Serial.println("Ocean");
        break;
      case 3:
        Serial.println("Lava");
        break;
      case 4:
        Serial.println("Forest");
        break;
      case 5:
        Serial.println("Love");
        break;
      default:
        Serial.println("Unknown");
    }

    // Set LED colors based on the color palette
    if (position == 1) {
      // Position off, turn off LEDs
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      FastLED.show();
    } else {
      // Position on, set LED colors based on the color palette
      setLEDColorPalette(colorPalette);
    }

    if (autoBrightness == 0) { // Only update brightness if autoBrightness is off and position is on
      updateBrightness();
    }

    delete[] valueChars; // Free the allocated buffer
  }
}

void loop() {
  // Check for BLE connections
  BLEDevice central = BLE.central();
  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    while (central.connected()) {
      handleBLEData();
      if (position == 0) {
        switch (currentPattern) {
          case 0:
            patternBlinking();
            break;
          case 1:
            patternSynced();
            break;
          case 2:
            patternRain();
            break;
          case 3:
            patternFire();
            break;
          case 4:
            patternGrowing();
            break;
          case 5:
            patternHeartPulse();
            break;
          default:
            fill_solid(leds, NUM_LEDS, CRGB::Black);
            FastLED.show();
            break;
        }
      }
      if (autoBrightness == 1) { // Only update brightness if autoBrightness is on
        updateBrightness();
      }
    }

    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }

  // Continuously update brightness
  if (position == 0 && autoBrightness == 1) {
    updateBrightness();
  }
}
