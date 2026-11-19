/*
  IR_Test - just the M5 Reflective IR Unit, on one port, on the Atom Matrix.

  The unit has two outputs:
    digital: on/off, flips when the reflection crosses the level set by the unit's pot
    analog:  how much IR comes back, as a voltage (0..4095 when read)

  Matrix:
    left column      the digital output: blue = HIGH, dark = LOW
    other 4 columns  the analog reading as a bar: 0..20 LEDs, filling from the bottom

  Serial (115200): both readings, 10 times a second.

  Pick the port by uncommenting ONE pair of lines below, then upload.
  Port B is wired the other way round from A and C (a unit's first wire lands on G33),
  so its analog output lands on G23, which cannot read a voltage: there it is read
  as on/off only.

  Needs: FastLED.
*/
#include <FastLED.h>

// PORT A
#define DIGITAL_PIN 26
#define ANALOG_PIN  32
#define ANALOG_HAS_ADC true

// PORT B
// #define DIGITAL_PIN 33
// #define ANALOG_PIN  23
// #define ANALOG_HAS_ADC false

const int MATRIX_PIN = 27;
const int LED_BRIGHTNESS = 20;

CRGB leds[25];

// The matrix: index = row * 5 + col, row 0 at the top
void put(int row, int col, CRGB colour) {
  leds[row * 5 + col] = colour;
}

void setup() {
  Serial.begin(115200);
  pinMode(DIGITAL_PIN, INPUT);
  if (!ANALOG_HAS_ADC) {
    pinMode(ANALOG_PIN, INPUT);
  }
  FastLED.addLeds<WS2812, MATRIX_PIN, GRB>(leds, 25);
  FastLED.setBrightness(LED_BRIGHTNESS);
  Serial.printf("IR test: digital on G%d, analog on G%d\n", DIGITAL_PIN, ANALOG_PIN);
}

void loop() {
  int digital = digitalRead(DIGITAL_PIN);

  int analog;
  if (ANALOG_HAS_ADC) {
    analog = analogRead(ANALOG_PIN);
  } else {
    // No ADC on this pin: the best it can do is on/off
    if (digitalRead(ANALOG_PIN) == HIGH) {
      analog = 4095;
    } else {
      analog = 0;
    }
  }

  for (int k = 0; k < 25; k++) {
    leds[k] = CRGB(0, 0, 0);
  }

  // Left column: the digital output
  for (int row = 0; row < 5; row++) {
    if (digital == HIGH) {
      put(row, 0, CRGB(0, 0, 255));
    }
  }

  // The other 4 columns: the analog bar, bottom row first, left to right
  int lit = (analog * 20 + 2047) / 4095;
  int count = 0;
  for (int row = 4; row >= 0; row--) {
    for (int col = 1; col < 5; col++) {
      if (count < lit) {
        put(row, col, CRGB(0, 255, 0));
      }
      count++;
    }
  }

  FastLED.show();

  if (digital == HIGH) {
    Serial.printf("digital G%d: HIGH   analog G%d: %4d\n", DIGITAL_PIN, ANALOG_PIN, analog);
  } else {
    Serial.printf("digital G%d: LOW    analog G%d: %4d\n", DIGITAL_PIN, ANALOG_PIN, analog);
  }
  delay(100);
}
