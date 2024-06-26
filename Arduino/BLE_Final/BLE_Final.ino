#include <ArduinoBLE.h>
#include <FastLED.h>
#include <arduinoFFT.h>

#define LED_PIN     2    // GPIO pin connected to the data pin of the LED strip
#define NUM_LEDS_PER_STRIP 30
#define NUM_STRIPS 5
#define NUM_LEDS (NUM_LEDS_PER_STRIP * NUM_STRIPS)
#define LED_TYPE    SK6812
#define COLOR_ORDER GRB

#define MIC_PIN 35
#define LDR_PIN 34  // Analog pin connected to LDR
#define NUM_SAMPLES 10 // Number of samples for the moving average

CRGB leds[NUM_LEDS];
BLEService bleService("19b10010-e8f2-537e-4f6c-d104768a1214");
BLECharacteristic bleCharacteristic("19b10011-e8f2-537e-4f6c-d104768a1214", BLERead | BLEWrite, 512);

bool bitmap[NUM_STRIPS][NUM_LEDS];

int ldrValues[NUM_SAMPLES];
int sampleIndex = 0;
bool autoBrightness = false;
int brightnessPercentage = 0;
int position = 1;
int currentPattern = 0;
int currentPalette = 0;
unsigned long lastUpdate = 0;
const int updateInterval = 50;

const uint16_t samples = 512;
const double samplingFrequency = 10000;
unsigned int samplingPeriodUs;
unsigned long microseconds;

double vReal[samples];
double vImag[samples];

ArduinoFFT FFT = ArduinoFFT(vReal, vImag, samples, samplingFrequency);

void setup() {
  Serial.begin(9600);

  // Initialize FastLED
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(50);

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
  clearBitmap();
}

void clearBitmap() {
  for (int i = 0; i < NUM_STRIPS; i++) {
    for (int j = 0; j < NUM_LEDS; j++) {
      bitmap[i][j] = false;
    }
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
    case 0:
      fill_solid(leds, NUM_LEDS, CRGB::Blue);
      break;
    case 1:
      fill_rainbow(leds, NUM_LEDS, 0, 255 / NUM_LEDS);
      break;
    case 2:
      fill_gradient_RGB(leds, NUM_LEDS, CRGB::Blue, CRGB::Aqua);
      break;
    case 3:
      fill_gradient_RGB(leds, NUM_LEDS, CRGB::Red, CRGB::Orange);
      break;
    case 4:
      fill_gradient_RGB(leds, NUM_LEDS, CRGB::Green, CRGB::ForestGreen);
      break;
    case 5:
      fill_gradient_RGB(leds, NUM_LEDS, CRGB::Red, CRGB::Pink);
      break;
    default:
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      break;
  }
  FastLED.show();
}

int updateBrightness() {
  int currentBrightness;
  if (autoBrightness) {
    int ldrValue = getAverageLDRValue();
    currentBrightness = map(ldrValue, 0, 4095, 0, 255);
  } else {
    currentBrightness = map(brightnessPercentage, 0, 100, 0, 255);
  }
  FastLED.setBrightness(currentBrightness);
  FastLED.show();
  return currentBrightness;
}

void patternBlinking() {
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
    updateLEDsRandom(normalizedVolume, peak);  // Using normalized volume

    // Step 6: Show the LEDs
    FastLED.show();
}

void updateLEDsRandom(double volume, double pitch) {
    // Calculate the number of LEDs to turn on based on the normalized volume
    int numLEDsToTurnOn = map(volume * 1023 / 3, 0, 1023, 0, NUM_LEDS);

    // Clear the LED array
    memset(leds, 0, NUM_LEDS * sizeof(CRGB));

    // Turn on random LEDs
    for (int i = 0; i < numLEDsToTurnOn; i++) {
        int ledIndex = random(NUM_LEDS);
        // Ensure the LEDs use the existing color palette
        setLEDColorPalette(currentPalette);
        leds[ledIndex] = leds[0]; // Use the first LED's color as a reference
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
                // Set LED color using the setLEDColorPalette function
                setLEDColorPalette(currentPalette);
            }
        }
    }
    FastLED.show();
}

void patternRain() {
  // Define droplet color and brightness
  CRGB dropletColor = CRGB::Blue;  // You can change this color as desired
  int dropletBrightness = 100;     // Adjust brightness as needed

  // Create a temporary array to store the state of LEDs
  bool tempBitmap[NUM_STRIPS][NUM_LEDS_PER_STRIP];

  // Initialize temporary bitmap with current state
  for (int strip = 0; strip < NUM_STRIPS; strip++) {
    for (int led = 0; led < NUM_LEDS_PER_STRIP; led++) {
      tempBitmap[strip][led] = bitmap[strip][led];
    }
  }

  // Move existing droplets down by one position
  for (int strip = 0; strip < NUM_STRIPS; strip++) {
    for (int led = 0; led < NUM_LEDS_PER_STRIP; led++) {
      if (bitmap[strip][led]) {
        // Move the droplet down one position
        if (led < NUM_LEDS_PER_STRIP - 1) {
          tempBitmap[strip][led + 1] = true;
        }
        tempBitmap[strip][led] = false; // Clear the current position
      }
    }
  }

  // Add new droplets at the top
  for (int strip = 0; strip < NUM_STRIPS; strip++) {
    if (random(10) < 2) { // Adjust the probability as needed (e.g., 2 out of 10)
      int startingLED = random(NUM_LEDS_PER_STRIP - 2); // Ensure enough space for a droplet
      tempBitmap[strip][startingLED] = true;
      tempBitmap[strip][startingLED + 1] = true;
      tempBitmap[strip][startingLED + 2] = true;
    }
  }

  // Update the main bitmap with the temporary bitmap
  for (int strip = 0; strip < NUM_STRIPS; strip++) {
    for (int led = 0; led < NUM_LEDS_PER_STRIP; led++) {
      bitmap[strip][led] = tempBitmap[strip][led];
    }
  }

  // Update LEDs based on the bitmap
  for (int strip = 0; strip < NUM_STRIPS; strip++) {
    for (int led = 0; led < NUM_LEDS_PER_STRIP; led++) {
      int ledIndex = strip * NUM_LEDS_PER_STRIP + led;
      if (bitmap[strip][led]) {
        leds[ledIndex] = dropletColor;
        leds[ledIndex].fadeToBlackBy(255 - dropletBrightness); // Adjust brightness
      } else {
        leds[ledIndex] = CRGB::Black; // Clear non-droplet LEDs
      }
    }
  }

  // Show the updated LEDs
  FastLED.show();
}

void patternFire() {
  int baseBrightness = updateBrightness(); // Get the current brightness
  for (int i = 0; i < NUM_LEDS; i++) {
    int brightnessAdjust = baseBrightness + ((i % 2 == 0) ? 5 : -5);
    leds[i] = CHSV(0, 255, brightnessAdjust);
  }
  FastLED.show();
}

void patternGrowing() {
  // Implement growing pattern
}

void patternHeartPulse() {
  int maxOffset = 6; // position above centre
  int minOffset = -6; // position below centre
  int offset = 0; // current position
  int step = 1;

  while (true) {
    createHeartPattern(offset);
    updateLEDsheart();
    
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

void updateLEDsheart() {
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

void handleBLEData() {
  if (bleCharacteristic.written()) {
    int valueLength = bleCharacteristic.valueLength();
    char *valueChars = new char[valueLength + 1];
    const uint8_t *valueBytes = bleCharacteristic.value();
    memcpy(valueChars, valueBytes, valueLength);
    valueChars[valueLength] = '\0';
    Serial.println(valueChars);

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

    if (position == 1) {
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      FastLED.show();
    } else {
      setLEDColorPalette(colorPalette);
    }

    if (autoBrightness == 0) {
      updateBrightness();
    }

    delete[] valueChars;
  }
}

void loop() {
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
    }

    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}
