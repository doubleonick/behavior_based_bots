/*
  Light_Test - just the M5 Light Unit (U021), on one port, on the Atom Matrix.

  The unit has two outputs (we think: this sketch is for finding out):
    analog: how much light, as a voltage
    on/off: flips when the light crosses the level set by the unit's knob (pot)
  So this sketch reads BOTH of the port's pins, and shows each one:
    a pin that can read a voltage (ADC) as a level, 0..4095
    a pin that can't, as on/off (0 or 4095)

  Matrix:
    left two columns   the port's first pin, blue bar, filling from the bottom
    right two columns  the port's second pin, green bar
    middle column      dark
  Each bar has 10 LEDs. A full bar is 4095 (or "on").

  Serial (115200): both pins, 10 times a second.

  Pick the port by uncommenting ONE block below, then upload.
  "First" and "second" are the pins in the order the Val sketches list them; Port B
  is wired the other way round, so on Port B the unit's first wire lands on G33.

  Needs: FastLED.
*/
#include <FastLED.h>

// PORT A (both pins can read a voltage)
#define PIN_ONE 26
#define PIN_ONE_HAS_ADC true
#define PIN_TWO 32
#define PIN_TWO_HAS_ADC true

// PORT B (only G33 can read a voltage)
// #define PIN_ONE 33
// #define PIN_ONE_HAS_ADC true
// #define PIN_TWO 23
// #define PIN_TWO_HAS_ADC false

// PORT C (neither can read a voltage: on/off only)
// #define PIN_ONE 19
// #define PIN_ONE_HAS_ADC false
// #define PIN_TWO 22
// #define PIN_TWO_HAS_ADC false

const int MATRIX_PIN = 27;
const int LED_BRIGHTNESS = 20;

CRGB leds[25];

// The matrix: index = row * 5 + col, row 0 at the top
void put(int row, int col, CRGB colour) {
  leds[row * 5 + col] = colour;
}

// One pin's reading, 0..4095
int readPin(int pin, bool hasAdc) {
  if (hasAdc) {
    return analogRead(pin);
  }
  if (digitalRead(pin) == HIGH) {
    return 4095;
  }
  return 0;
}

// A 10-LED bar in two columns, bottom row first, left to right
void drawBar(int firstCol, int reading, CRGB colour) {
  int lit = (reading * 10 + 2047) / 4095;
  int count = 0;
  for (int row = 4; row >= 0; row--) {
    for (int col = firstCol; col < firstCol + 2; col++) {
      if (count < lit) {
        put(row, col, colour);
      }
      count++;
    }
  }
}

void setup() {
  Serial.begin(115200);
  if (!PIN_ONE_HAS_ADC) {
    pinMode(PIN_ONE, INPUT);
  }
  if (!PIN_TWO_HAS_ADC) {
    pinMode(PIN_TWO, INPUT);
  }
  FastLED.addLeds<WS2812, MATRIX_PIN, GRB>(leds, 25);
  FastLED.setBrightness(LED_BRIGHTNESS);
  Serial.printf("Light test: first pin G%d, second pin G%d\n", PIN_ONE, PIN_TWO);
}

void loop() {
  int one = readPin(PIN_ONE, PIN_ONE_HAS_ADC);
  int two = readPin(PIN_TWO, PIN_TWO_HAS_ADC);

  for (int k = 0; k < 25; k++) {
    leds[k] = CRGB(0, 0, 0);
  }
  drawBar(0, one, CRGB(0, 0, 255));
  drawBar(3, two, CRGB(0, 255, 0));
  FastLED.show();

  Serial.printf("G%d: %4d %s   G%d: %4d %s\n",
                PIN_ONE, one, PIN_ONE_HAS_ADC ? "(level) " : "(on/off)",
                PIN_TWO, two, PIN_TWO_HAS_ADC ? "(level) " : "(on/off)");
  delay(100);
}
