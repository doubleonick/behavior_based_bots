/*
  Example 5: read a Key Unit and draw a bar
  =========================================

  The M5 Key Unit is a button with a colour LED (an SK6812, the same kind as the
  matrix's LEDs) inside it. Its Grove cable carries:
    yellow wire: the LED's data line (the Atom sends it a colour)
    white wire:  the button. Read with the pin's pull-up: HIGH = not pressed,
                 LOW = pressed.

  HOOK UP
  It is not I2C: the Atom drives and reads the wires directly. On/off is all a
  button needs, so it works in all three ports (all tested):

      port      LED (yellow)   button (white)
      Port A    G26            G32
      Port B    G33            G23      (Port B is wired the other way round)
      Port C    G19            G22

    Pick the port by uncommenting ONE pair below.
    Note: on the robot we tested, presses were NOT seen while powered from USB, only
    on the Motion's battery (switch on). If nothing happens on USB, try the battery.

  WHAT YOU SEE
    Not pressed: the key glows blue, the matrix is dark.
    Pressed: the key glows red, the whole matrix lights (a full bar).
    Serial (115200) prints "pressed" / "not pressed".

  Needs: FastLED.
*/
#include <FastLED.h>

// The port: pick ONE pair. FastLED needs the LED pin fixed when the sketch is
// compiled, so these are #defines.
#define LED_PIN    26                 // Port A
#define BUTTON_PIN 32
// #define LED_PIN    33              // Port B
// #define BUTTON_PIN 23
// #define LED_PIN    19              // Port C
// #define BUTTON_PIN 22

const int MATRIX_PIN = 27;
CRGB leds[25];
CRGB keyLed[1];                       // the key's own LED

// Light `count` of the 25 LEDs, bottom row first (FastLED.show() is called in loop)
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
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // the pull-up holds it HIGH until pressed
  FastLED.addLeds<WS2812, MATRIX_PIN, GRB>(leds, 25);
  FastLED.addLeds<SK6812, LED_PIN, GRB>(keyLed, 1);
  FastLED.setBrightness(20);          // the matrix and the key's LED both
}

void loop() {
  bool pressed = (digitalRead(BUTTON_PIN) == LOW);

  if (pressed) {
    keyLed[0] = CRGB(255, 0, 0);
    drawBar(25, CRGB(255, 255, 255));
    Serial.println("pressed");
  } else {
    keyLed[0] = CRGB(0, 0, 255);
    drawBar(0, CRGB(0, 0, 0));
    Serial.println("not pressed");
  }

  FastLED.show();                     // sends both: the matrix and the key's LED
  delay(50);
}
