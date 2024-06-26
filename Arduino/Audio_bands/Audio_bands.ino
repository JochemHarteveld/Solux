#include <FastLED.h>
#include <arduinoFFT.h>

#define LED_PIN 2
#define NUM_LEDS_PER_STRIP 30
#define NUM_STRIPS 5
#define NUM_LEDS (NUM_LEDS_PER_STRIP * NUM_STRIPS)
#define LED_TYPE SK6812
#define COLOR_ORDER GRB
#define MIC_PIN 35

CRGB leds[NUM_LEDS];
CRGBPalette16 palette = RainbowColors_p;

const uint16_t samples = 512;       // Must be a power of 2
const double samplingFrequency = 10000; // Hz, must be less than 10000 due to ADC limitations
unsigned int samplingPeriodUs;
unsigned long microseconds;

double vReal[samples];
double vImag[samples];

ArduinoFFT FFT = ArduinoFFT(vReal, vImag, samples, samplingFrequency);

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(128);

  samplingPeriodUs = round(1000000 * (1.0 / samplingFrequency));
}

void loop() {
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

  // Step 3: Calculate amplitude for each band
  double bandAmplitudes[NUM_STRIPS] = {0};
  int bandSize = samples / (2 * NUM_STRIPS);

  for (int band = 0; band < NUM_STRIPS; band++) {
    double sum = 0;
    for (int i = band * bandSize; i < (band + 1) * bandSize; i++) {
      sum += vReal[i];
    }
    bandAmplitudes[band] = sum / bandSize;
  }

  // print amplitude values of each band
  for (int band = 0; band < NUM_STRIPS; band++) {
    Serial.print("Band ");
    Serial.print(band);
    Serial.print(": ");
    Serial.println(bandAmplitudes[band]);
  }

  // Step 5: Update the LEDs based on volume and pitch
  updateLEDs(bandAmplitudes);

  // Step 6: Show the LEDs
  FastLED.show();
  delay(10);
}

void updateLEDs(double bandAmplitudes[]) {
  bool ledMatrix[NUM_STRIPS][NUM_LEDS_PER_STRIP] = {false};

  // Apply a simple normalization
  double maxAmplitude = 0;
  for (int i = 0; i < NUM_STRIPS; i++) {
    if (bandAmplitudes[i] > maxAmplitude) {
      maxAmplitude = bandAmplitudes[i];
    }
  }

  // Apply a simple averaging filter to reduce sudden peaks
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
    Serial.print("Strip ");
    Serial.print(strip);
    Serial.print(": ");
    Serial.println(barHeight);

    for (int i = 0; i < barHeight; i++) {
      if (strip % 2 == 0) {
        ledMatrix[strip][i] = true;
      } else {
        ledMatrix[strip][NUM_LEDS_PER_STRIP - 1 - i] = true;
      }
    }
  }

  memset(leds, 0, NUM_LEDS * sizeof(CRGB));

  // Convert the 2D ledMatrix to the FastLED CRGB array
  for (int strip = 0; strip < NUM_STRIPS; strip++) {
    for (int i = 0; i < NUM_LEDS_PER_STRIP; i++) {
      if (ledMatrix[strip][i]) {
        int ledIndex = strip * NUM_LEDS_PER_STRIP + i;
        leds[ledIndex] = ColorFromPalette(palette, 255 * i / NUM_LEDS_PER_STRIP);
      }
    }
  }
}
