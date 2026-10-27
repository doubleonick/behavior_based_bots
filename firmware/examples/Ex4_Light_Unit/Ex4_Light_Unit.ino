/*
  Example 4: read a Light Unit and draw a bar
  ===========================================

  The M5 Light Unit (U021) has a light-dependent resistor: its resistance changes
  with how much light falls on it. Like the Reflective IR Unit, it has TWO outputs:
    yellow wire: ON/OFF. Flips when the light passes the level set by the small
                 knob (pot) on the unit.
    white wire:  a LEVEL. How much light, as a voltage.
  (Its label says "Port B": that is M5Stack's name for this kind of socket, on their
  other products. On the Atom + Motion it is Port A that gives a level.)

  HOOK UP
  It is not I2C: the Atom reads the wires directly, so the port matters a lot.
  To read a level, a pin needs an ADC (analog-to-digital converter).

      port      yellow (on/off)   white (level)                    what you get
      Port A    G26               G32 (has an ADC)                 a LEVEL  (tested)
      Port B    G33               G23 (no ADC!)                    ON/OFF only (tested)
      Port C    G19               G22 (no ADC!)                    ON/OFF only (not tried)

    Port B is wired the other way round from Port A: there the level wire lands on
    G23, which cannot read a voltage.

    Pick the port by uncommenting ONE block below.

  WHAT YOU SEE
    Port A: the matrix fills (yellow) as it gets brighter, bottom row first. If
            covering the sensor FILLS the bar, set FLIP to true.
    Port B or C: the matrix is either full or dark, flipping where the knob is set.
    Serial (115200) prints both readings.

  Needs: FastLED.
*/
#include <FastLED.h>

// PORT A: a level
const int ON_OFF_PIN = 26;
const int LEVEL_PIN = 32;
const bool LEVEL_HAS_ADC = true;

// PORT B: on/off only
// const int ON_OFF_PIN = 33;
// const int LEVEL_PIN = 23;
// const bool LEVEL_HAS_ADC = false;

// PORT C: on/off only
// const int ON_OFF_PIN = 19;
// const int LEVEL_PIN = 22;
// const bool LEVEL_HAS_ADC = false;

const bool FLIP = false;              // true: a bigger reading means DARKER

const int MATRIX_PIN = 27;
CRGB leds[25];

// Light `count` of the 25 LEDs, bottom row first
void drawBar(int count, CRGB colour) {
  for (int k = 0; k < 25; k++) {
    int row = 4 - k / 5;              // k = 0..4 is the bottom row
    int col = k % 5;
    if (k < count) {
      leds[row * 5 + col] = colour;
    } else {
      leds[row * 5 + col] = CRGB(0, 0, 0);
    }
  }
  FastLED.show();
}

void setup() {
  Serial.begin(115200);
  pinMode(ON_OFF_PIN, INPUT);
  pinMode(LEVEL_PIN, INPUT);
  FastLED.addLeds<WS2812, MATRIX_PIN, GRB>(leds, 25);
  FastLED.setBrightness(20);
}

void loop() {
  bool on = (digitalRead(ON_OFF_PIN) == HIGH);    // the on/off wire

  int count;
  if (LEVEL_HAS_ADC) {
    int level = analogRead(LEVEL_PIN);            // 0..4095
    if (FLIP) {
      level = 4095 - level;
    }
    count = level * 25 / 4095;
    Serial.printf("level %4d   on/off: %s\n", level, on ? "HIGH" : "LOW");
  } else {
    if (on) {
      count = 25;
    } else {
      count = 0;
    }
    Serial.printf("on/off: %s\n", on ? "HIGH" : "LOW");
  }

  drawBar(count, CRGB(255, 200, 0));
  delay(50);
}
