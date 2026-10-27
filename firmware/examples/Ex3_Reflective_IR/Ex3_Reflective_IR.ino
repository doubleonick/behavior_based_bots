/*
  Example 3: read a Reflective IR Unit and draw a bar
  ===================================================

  The M5 Reflective IR Unit shines infrared light and measures how much bounces back:
  more when something pale or shiny is close. It has TWO outputs on its Grove cable:
    yellow wire: ON/OFF. Flips when the reflection passes the level set by the small
                 knob (pot) on the unit. (On the unit we tested: LOW = sees something.)
    white wire:  a LEVEL. How much bounces back, as a voltage.

  HOOK UP
  It is not I2C: the Atom reads the wires directly, so the port matters a lot.
  To read a level, a pin needs an ADC (analog-to-digital converter).

      port      yellow (on/off)   white (level)                    what you get
      Port A    G26               G32 (has an ADC)                 a LEVEL  (tested)
      Port B    G33               G23 (no ADC!)                    ON/OFF only (tested)
      Port C    G19               G22 (no ADC!)                    ON/OFF only (not tried)

    Port B is wired the other way round from Port A: there the on/off wire lands on
    G33 (which has an ADC, but on/off is all that wire carries) and the level wire
    lands on G23, which cannot read a voltage.

    Pick the port by uncommenting ONE block below.

  WHAT YOU SEE
    Port A: the matrix fills (teal) as the reflection gets stronger, bottom row first.
            Which way the voltage runs depends on the unit: if a hand close by EMPTIES
            the bar, set FLIP to true.
    Port B or C: the matrix is either full (sees something) or dark.
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

const bool FLIP = false;              // true: a bigger reading means LESS reflection

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
  bool sees = (digitalRead(ON_OFF_PIN) == LOW);   // the on/off wire

  int count;
  if (LEVEL_HAS_ADC) {
    int level = analogRead(LEVEL_PIN);            // 0..4095
    if (FLIP) {
      level = 4095 - level;
    }
    count = level * 25 / 4095;
    Serial.printf("level %4d   on/off: %s\n", level, sees ? "sees something" : "nothing");
  } else {
    if (sees) {
      count = 25;
    } else {
      count = 0;
    }
    Serial.printf("on/off: %s\n", sees ? "sees something" : "nothing");
  }

  drawBar(count, CRGB(0, 180, 160));
  delay(50);
}
