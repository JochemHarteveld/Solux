#include <ArduinoBLE.h>
#include <FastLED.h>
#include <arduinoFFT.h>

#define LED_PIN     2
#define LED_TYPE    SK6812
#define COLOR_ORDER GRB

#define MIC_PIN 35
#define LDR_PIN     34   // Analog pin connected to LDR
#define NUM_SAMPLES 10   // Number of samples for the moving average

#define NUM_STRIPS 5 // Assuming the value
#define NUM_LEDS_PER_STRIP 30 // Assuming the value
#define NUM_LEDS (NUM_STRIPS * NUM_LEDS_PER_STRIP)

CRGB leds[NUM_LEDS]; // Define the leds array
bool bitmap[NUM_STRIPS][NUM_LEDS_PER_STRIP]; // Define the bitmap array

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

  clearBitmap();
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

double* getFFT() {
    static double bandAmplitudes[NUM_STRIPS];  // Static to ensure it exists after the function returns
    static double magnitudes[5];               // Static to ensure it exists after the function returns

    for (int i = 0; i < samples; i++) {
        microseconds = micros();
        vReal[i] = analogRead(MIC_PIN); // A conversion takes about 1uS on an ESP32
        vImag[i] = 0;
        while (micros() < (microseconds + samplingPeriodUs)) {
            // do nothing to wait
        }
    }

    FFT.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.compute(FFT_FORWARD);
    FFT.complexToMagnitude();

    // Get magnitudes for specific frequencies
    int frequencies[5] = {125, 300, 600, 1200, 3000};

    for (int i = 0; i < 5; i++) {
        int index = (frequencies[i] * samples) / samplingFrequency;
        magnitudes[i] = vReal[index];
    }

    int bandSizemag = samples / (2 * NUM_STRIPS);

    for (int band = 0; band < NUM_STRIPS; band++) {
        double sum = 0;
        for (int i = band * bandSizemag; i < (band + 1) * bandSizemag; i++) {
            sum += vReal[i];
        }
        magnitudes[band] = sum / bandSizemag;
    }

    // Log the magnitudes
    Serial.println("Magnitudes for specific frequencies:");
    for (int i = 0; i < 5; i++) {
        Serial.print("Frequency ");
        Serial.print(frequencies[i]);
        Serial.print("Hz: ");
        Serial.println(magnitudes[i]);
    }

    int bandSize = samples / (2 * NUM_STRIPS);

    for (int band = 0; band < NUM_STRIPS; band++) {
        double sum = 0;
        for (int i = band * bandSize; i < (band + 1) * bandSize; i++) {
            sum += vReal[i];
        }
        bandAmplitudes[band] = sum / bandSize;
    }

    for (int band = 0; band < NUM_STRIPS; band++) {
        Serial.print("Band ");
        Serial.print(band);
        Serial.print(": ");
        Serial.println(bandAmplitudes[band]);
    }

    return bandAmplitudes;
}

void patternBlinking() {
    double* bandAmplitudes = getFFT();

    // Calculate the total amplitude
    double totalAmplitude = 0;
    for (int band = 0; band < NUM_STRIPS; band++) {
        totalAmplitude += bandAmplitudes[band];
    }

    Serial.println(totalAmplitude);

    // Calculate the delay based on the total amplitude
    int delayTime = 2000 - (totalAmplitude/5); // Adjust this factor as needed
    if (delayTime < 100) {
        delayTime = 100; // Minimum delay time
    }
    Serial.println(delayTime);

    // Calculate the number of LEDs to light up (approximately 1/5 of NUM_LEDS)
    int numLEDsToLight = NUM_LEDS / 5;

    // Array to keep track of which LEDs are lit
    bool ledState[NUM_LEDS];
    memset(ledState, 0, NUM_LEDS * sizeof(bool)); // Initialize all LEDs as off

    // Randomly select LEDs to turn on
    for (int i = 0; i < numLEDsToLight; i++) {
        int randomIndex = random(NUM_LEDS); // Generate a random index
        ledState[randomIndex] = true; // Mark the LED at randomIndex as on
    }

    // Set LEDs based on random selection
    for (int i = 0; i < NUM_LEDS; i++) {
        if (ledState[i]) {
            // Set LED color based on currentPalette
            switch (currentPalette) {
                case 0: // Blue
                    leds[i] = CRGB::Blue;
                    break;
                case 1: // Rainbow
                    leds[i] = CHSV(random(256), 255, 255); // Random hue for rainbow effect
                    break;
                case 2: // Ocean
                    leds[i] = CRGB::Aqua;
                    break;
                case 3: // Lava
                    leds[i] = CRGB::OrangeRed;
                    break;
                case 4: // Forest
                    leds[i] = CRGB::ForestGreen;
                    break;
                case 5: // Love
                    leds[i] = CRGB::DeepPink;
                    break;
                default:
                    leds[i] = CRGB::White; // Default to white if unknown palette
                    break;
            }
        } else {
            leds[i] = CRGB::Black; // Turn off LEDs not selected
        }
    }

    // Show the LEDs
    FastLED.show();

    // Delay for 5 seconds
    delay(delayTime);

    // Turn off all LEDs
    fill_solid(leds, NUM_LEDS, CRGB::Black);

    // Show the LEDs
    FastLED.show();

    // Delay for 5 seconds again before exiting the function
    delay(delayTime);
}

void patternSynced() {
  double* bandAmplitudes = getFFT();  
  updateLEDs(bandAmplitudes);

  FastLED.show();
  delay(10);
}

void updateLEDs(double bandAmplitudes[]) {
    bool ledMatrix[NUM_STRIPS][NUM_LEDS_PER_STRIP] = {false};

    double maxAmplitude = 0; // Corrected declaration
    for (int i = 0; i < NUM_STRIPS; i++) {
        if (bandAmplitudes[i] > maxAmplitude) {
            maxAmplitude = bandAmplitudes[i];
        }
    }

    double filteredAmplitudes[NUM_STRIPS];
    for (int i = 0; i < NUM_STRIPS; i++) {
        if (i == 0) {
            filteredAmplitudes[i] = (bandAmplitudes[i] + bandAmplitudes[i + 1]) / 2.0;
        } else if (i == NUM_STRIPS - 1) {
            filteredAmplitudes[i] = (bandAmplitudes[i] + bandAmplitudes[i - 1]) / 2.0;
        } else {
            filteredAmplitudes[i] = (bandAmplitudes[i - 1] + bandAmplitudes[i] + bandAmplitudes[i + 1]) / 3.0;
        }
    }

    for (int strip = 0; strip < NUM_STRIPS; strip++) {
        int barHeight = map(filteredAmplitudes[strip], 0, maxAmplitude, 0, NUM_LEDS_PER_STRIP);

        int startIndex = (strip % 2 == 0) ? 0 : (NUM_LEDS_PER_STRIP - 1);
        int endIndex = (strip % 2 == 0) ? barHeight : (NUM_LEDS_PER_STRIP - 1 - barHeight);

        if (strip % 2 == 0) {
            for (int i = startIndex; i < endIndex; i++) {
                ledMatrix[strip][i] = true;
            }
        } else {
            for (int i = startIndex; i > endIndex; i--) {
                ledMatrix[strip][i] = true;
            }
        }
    }

    memset(leds, 0, NUM_STRIPS * NUM_LEDS_PER_STRIP * sizeof(CRGB));

    for (int strip = 0; strip < NUM_STRIPS; strip++) {
        for (int i = 0; i < NUM_LEDS_PER_STRIP; i++) {
            if (ledMatrix[strip][i]) {
                int ledIndex = strip * NUM_LEDS_PER_STRIP + i;
                switch (currentPalette) {
                    case 0: // Blue
                        leds[ledIndex] = CRGB::Blue;
                        break;
                    case 1: // Rainbow
                        leds[ledIndex] = CHSV(random(256), 255, 255); // Random hue for rainbow effect
                        break;
                    case 2: // Ocean
                        leds[ledIndex] = CRGB::Aqua;
                        break;
                    case 3: // Lava
                        leds[ledIndex] = CRGB::OrangeRed;
                        break;
                    case 4: // Forest
                        leds[ledIndex] = CRGB::ForestGreen;
                        break;
                    case 5: // Love
                        leds[ledIndex] = CRGB::DeepPink;
                        break;
                    default:
                        leds[ledIndex] = CRGB::White; // Default to white if unknown palette
                        break;
                }
            }
        }
    }

    FastLED.show();
}

void updateRainLEDs() {  
    for (int strip = 0; strip < NUM_STRIPS; strip++) {
        for (int led = 0; led < NUM_LEDS_PER_STRIP; led++) {
            int ledIndex = strip * NUM_LEDS_PER_STRIP + led;
            if (bitmap[strip][led]) {
                // On LEDs are determined by the current palette
                switch (currentPalette) {
                    case 0: // Blue
                        leds[ledIndex] = CRGB::Blue;
                        break;
                    case 1: // Rainbow
                        leds[ledIndex] = CHSV(random(256), 255, 255); // Random hue for rainbow effect
                        break;
                    case 2: // Ocean
                        leds[ledIndex] = CRGB::Aqua;
                        break;
                    case 3: // Lava
                        leds[ledIndex] = CRGB::OrangeRed;
                        break;
                    case 4: // Forest
                        leds[ledIndex] = CRGB::ForestGreen;
                        break;
                    case 5: // Love
                        leds[ledIndex] = CRGB::DeepPink;
                        break;
                    default:
                        leds[ledIndex] = CRGB::White; // Default to white if unknown palette
                        break;
                }
            } else {
                // Off LEDs are black (off)
                leds[ledIndex] = CRGB::Black;
            }
        }
    }
    FastLED.show();
}

void createRainPattern() {
  // Shift existing raindrops down
  for (int strip = 0; strip < NUM_STRIPS; strip++) {
    if (strip % 2 == 0) {
      // For even-numbered strips, reverse
      for (int led = 0; led < NUM_LEDS_PER_STRIP - 1; led++) {
        bitmap[strip][led] = bitmap[strip][led + 1];
      }
      // 1 in 8 to create a raindrop
      bitmap[strip][NUM_LEDS_PER_STRIP - 1] = random(8) == 0;
    } else {
      for (int led = NUM_LEDS_PER_STRIP - 1; led > 0; led--) {
        bitmap[strip][led] = bitmap[strip][led - 1];
      }
      bitmap[strip][0] = random(8) == 0;
    }
  }
}

void patternRain() {
  createRainPattern();
  updateRainLEDs();
  delay(100);
}

void patternFire() {
    double* bandAmplitudes = getFFT();

    // Normalize band amplitudes to the range of 0 to NUM_LEDS_PER_STRIP
    double maxAmplitude = *std::max_element(bandAmplitudes, bandAmplitudes + NUM_STRIPS);
    for (int band = 0; band < NUM_STRIPS; band++) {
        bandAmplitudes[band] = map(bandAmplitudes[band], 0, maxAmplitude, 0, NUM_LEDS_PER_STRIP);
    }
    
    // Clear the bitmap array
    clearBitmap();
    
    // Update the bitmap based on the band amplitudes
    for (int strip = 0; strip < NUM_STRIPS; strip++) {
      int centerIndex = NUM_LEDS_PER_STRIP / 2;
      int amplitude = bandAmplitudes[strip];

      for (int i = 0; i < amplitude; i++) {
        int index1 = centerIndex + i; // Right side
        int index2 = centerIndex - i; // Left side

        // Update LED on the current strip
        if (index1 < NUM_LEDS_PER_STRIP) {
            bitmap[strip][index1] = true;
        }
        if (index2 >= 0) {
            bitmap[strip][index2] = true;
        }

        // Update LEDs on the neighboring strips to create a circular effect
        for (int offset = 1; offset <= i; offset++) {
            int stripAbove = strip + offset;
            int stripBelow = strip - offset;
            if (stripAbove < NUM_STRIPS) {
                if (index1 < NUM_LEDS_PER_STRIP) {
                    bitmap[stripAbove][index1] = true;
                }
                if (index2 >= 0) {
                    bitmap[stripAbove][index2] = true;
                }
            }
            if (stripBelow >= 0) {
                if (index1 < NUM_LEDS_PER_STRIP) {
                    bitmap[stripBelow][index1] = true;
                }
                if (index2 >= 0) {
                    bitmap[stripBelow][index2] = true;
                }
              }
          }
        }
    }


    // Update the LED strip based on the bitmap array
    updateRainLEDs();

    // Adjust the delay to control the speed of the fire effect
    delay(50);
}

void patternGrowing() {
  // Empty pattern function
}

void patternHeartPulse() {
    int maxOffset = 7; // position above centre
    int minOffset = -5; // position below centre
    int offset = 0; // current position
    int step = 1;
    int iterations = 0; // Initialize the counter
    Serial.println("here1");

    while (iterations < 24) { // Run the loop only 24 times
        createHeartPattern(offset);
        Serial.println("here2");
        updateHeartLEDs();
        Serial.println("here3");

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

        iterations++; // Increment the counter
    }
}

void clearBitmap() {
    for (int i = 0; i < NUM_STRIPS; i++) {
        for (int j = 0; j < NUM_LEDS_PER_STRIP; j++) {
            bitmap[i][j] = false;
        }
    }
}

void updateHeartLEDs() {
    for (int strip = 0; strip < NUM_STRIPS; strip++) {
        for (int led = 0; led < NUM_LEDS_PER_STRIP; led++) {
            int ledIndex = strip * NUM_LEDS_PER_STRIP + led;
            if (bitmap[strip][led]) {
                // On LEDs are determined by the current palette
                switch (currentPalette) {
                    case 0: // Blue
                        leds[ledIndex] = CRGB::Blue;
                        break;
                    case 1: // Rainbow
                        leds[ledIndex] = CHSV(random(256), 255, 255); // Random hue for rainbow effect
                        break;
                    case 2: // Ocean
                        leds[ledIndex] = CRGB::Aqua;
                        break;
                    case 3: // Lava
                        leds[ledIndex] = CRGB::OrangeRed;
                        break;
                    case 4: // Forest
                        leds[ledIndex] = CRGB::ForestGreen;
                        break;
                    case 5: // Love
                        leds[ledIndex] = CRGB::DeepPink;
                        break;
                    default:
                        leds[ledIndex] = CRGB::White; // Default to white if unknown palette
                        break;
                }
            } else {
                // Off LEDs are black (off)
                leds[ledIndex] = CRGB::Black;
            }
        }
    }
    FastLED.show();
}

void createHeartPattern(int offset) {
    clearBitmap();
    
    // Define the heart pattern (flipped 90 degrees)
    const bool heartPattern[5][11] = {
        {0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0},
        {0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0}
    };
    
    // Starting position to find centre
    int startX = (NUM_LEDS_PER_STRIP - 5) / 2;
    
    // Map the heart pattern to the LED strips
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 11; j++) {
            int targetX = startX + j + offset;
            if (targetX >= 0 && targetX < NUM_LEDS_PER_STRIP) {
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
      currentPalette = colorPalette;
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
