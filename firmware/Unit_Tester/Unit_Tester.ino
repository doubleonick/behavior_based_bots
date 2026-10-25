/*
  Unit_Tester - see what the Grove ports' pins are doing, on the matrix and on Serial.

  For trying units that can't say what they are (Reflective IR, Key, ...) before any
  of Val's sketches rely on them. It knows nothing about the units: it reads the six
  port pins, and for each pin tests whether anything is driving it.

  Hardware: M5 Atom Matrix (on the Atom Motion base, for Ports B and C)
              Port A: G26, G32   both can read analog
              Port B: G23, G33   only G33 can read analog
              Port C: G19, G22   neither can read analog

  The matrix shows one port at a time:
    middle column: which port (top dot = A, middle = B, bottom = C)
    left two columns:  the port's first pin  (A: G26, B: G23, C: G19)
    right two columns: the port's second pin (A: G32, B: G33, C: G22)
    Each pin is a bar from the bottom: how high its reading is (0..4095).
    Its colour says whether something is driving the pin:
      blue  = driven, analog pin (the bar is the voltage)
      green = driven, on/off pin (full = HIGH, empty = LOW)
      grey  = nothing driving it (an empty pin, or an input like an LED's data line)

  Button (press the matrix):
    short press: show the next port
    long press:  arm the KEY LED TEST on the port shown. Then press the Key: the pin
                 that goes LOW is the button, and the other pin is driven as an SK6812
                 LED, cycling through colours. (Only after the press, so a pin is never
                 driven by accident.) Long press again to stop it.

  "Is something driving the pin?": the pin is read once with the ESP32's weak pull-up
  and once with its weak pull-down. An empty pin just follows the pull (HIGH, then LOW).
  A unit's output holds the pin where it wants it.

  Serial (115200 baud), twice a second: every pin's reading and whether it is driven.

  Library needed: FastLED.
*/

#include <FastLED.h>

// ---------- Settings ----------
const uint8_t LED_BRIGHTNESS = 20;    // keep low: the matrix is very bright and gets warm
const int MATRIX_PIN = 27;
const int BUTTON_PIN = 39;            // the matrix is the button (LOW when pressed)

// ---------- The six port pins ----------
struct PortPin {
  const char* port;
  int pin;
  bool hasAdc;                        // can it read a voltage (analog)?
};

const int PIN_COUNT = 6;
const PortPin PINS[PIN_COUNT] = {
  {"A", 26, true}, {"A", 32, true},
  {"B", 23, false}, {"B", 33, true},
  {"C", 19, false}, {"C", 22, false}
};
const char* const PORT_NAMES[3] = {"A", "B", "C"};

// What we know about each pin
struct PinState {
  int value = 0;                      // 0..4095 (an on/off pin reads 0 or 4095)
  bool driven = false;                // something holds the pin against the weak pulls
};
PinState state[PIN_COUNT];

// The key LED test
struct KeyTest {
  bool armed = false;                 // waiting for the key to be pressed
  int port = -1;                      // which port it was armed on
  int buttonPin = -1;                 // index into PINS, once found
  int ledPin = -1;
  bool ledAdded = false;
};
KeyTest keyTest;

CRGB leds[25];
CRGB keyLed[1];
int shownPort = 0;
unsigned long lastTestMs = 0;
unsigned long lastReadMs = 0;
unsigned long lastPrintMs = 0;
unsigned long lastDrawMs = 0;
bool buttonDown = false;
unsigned long buttonDownMs = 0;

// =====================================================================
//  Reading the pins
// =====================================================================
// Is the pin a key LED we are driving? Then leave it alone.
bool isDrivenByUs(int k) {
  return keyTest.ledAdded && k == keyTest.ledPin;
}

// Read with the weak pull-up, then with the weak pull-down. An empty pin follows them.
void testDriven(int k) {
  int pin = PINS[k].pin;
  pinMode(pin, INPUT_PULLUP);
  delayMicroseconds(200);
  int withPullUp = digitalRead(pin);
  pinMode(pin, INPUT_PULLDOWN);
  delayMicroseconds(200);
  int withPullDown = digitalRead(pin);
  state[k].driven = !(withPullUp == HIGH && withPullDown == LOW);
}

// Leave the pin ready for reading: analog pins with no pull, on/off pins with the pull-up
// (so a button that only connects to ground still reads HIGH when it isn't pressed)
void prepareForReading(int k) {
  if (PINS[k].hasAdc) {
    pinMode(PINS[k].pin, INPUT);
  } else {
    pinMode(PINS[k].pin, INPUT_PULLUP);
  }
}

void readPin(int k) {
  if (PINS[k].hasAdc) {
    state[k].value = analogRead(PINS[k].pin);
  } else {
    state[k].value = (digitalRead(PINS[k].pin) == HIGH) ? 4095 : 0;
  }
}

void updatePins() {
  unsigned long now = millis();
  if (now - lastTestMs >= 250) {
    lastTestMs = now;
    for (int k = 0; k < PIN_COUNT; k++) {
      if (isDrivenByUs(k)) {
        continue;
      }
      testDriven(k);
      prepareForReading(k);
    }
  }
  if (now - lastReadMs >= 20) {
    lastReadMs = now;
    for (int k = 0; k < PIN_COUNT; k++) {
      if (isDrivenByUs(k)) {
        continue;
      }
      readPin(k);
    }
  }
}

// =====================================================================
//  The key LED test
// =====================================================================
// FastLED needs the pin when it is compiled, so there is one line per possible pin
void addKeyLed(int pin) {
  if (pin == 26) {
    FastLED.addLeds<SK6812, 26, GRB>(keyLed, 1);
  } else if (pin == 32) {
    FastLED.addLeds<SK6812, 32, GRB>(keyLed, 1);
  } else if (pin == 23) {
    FastLED.addLeds<SK6812, 23, GRB>(keyLed, 1);
  } else if (pin == 33) {
    FastLED.addLeds<SK6812, 33, GRB>(keyLed, 1);
  } else if (pin == 19) {
    FastLED.addLeds<SK6812, 19, GRB>(keyLed, 1);
  } else if (pin == 22) {
    FastLED.addLeds<SK6812, 22, GRB>(keyLed, 1);
  }
}

void updateKeyTest() {
  if (!keyTest.armed || keyTest.buttonPin >= 0) {
    return;
  }
  // Waiting for the press: whichever of the port's on/off-read pins goes LOW
  for (int j = 0; j < 2; j++) {
    int k = keyTest.port * 2 + j;
    int other = keyTest.port * 2 + (1 - j);
    if (digitalRead(PINS[k].pin) == LOW && state[k].driven) {
      keyTest.buttonPin = k;
      keyTest.ledPin = other;
      Serial.printf("Key test: button on G%d, LED on G%d. The LED should now cycle colours.\n",
                    PINS[k].pin, PINS[other].pin);
      addKeyLed(PINS[other].pin);
      keyTest.ledAdded = true;
      return;
    }
  }
}

void armOrStopKeyTest() {
  if (keyTest.armed) {
    if (keyTest.ledAdded) {
      keyLed[0] = CRGB::Black;
      FastLED.show();
      Serial.println("Key test stopped (the LED pin stays an LED until you restart)");
    } else {
      Serial.println("Key test stopped");
    }
    keyTest.armed = false;
    return;
  }
  keyTest = KeyTest();
  keyTest.armed = true;
  keyTest.port = shownPort;
  Serial.printf("Key test armed on Port %s: press the key\n", PORT_NAMES[shownPort]);
}

// =====================================================================
//  The matrix
// =====================================================================
void putLed(int row, int col, const CRGB& c) {
  leds[row * 5 + col] = c;
}

// One pin as a bar in two columns, filling from the bottom
void drawPin(int k, int col) {
  CRGB colour;
  if (isDrivenByUs(k)) {
    colour = keyLed[0];               // the key LED: show what it is showing
  } else if (!state[k].driven) {
    colour = CRGB(50, 50, 60);        // grey: nothing driving it
  } else if (PINS[k].hasAdc) {
    colour = CRGB(0, 150, 255);       // blue: an analog reading
  } else {
    colour = CRGB(0, 255, 60);        // green: an on/off reading
  }
  float level = isDrivenByUs(k) ? 5 : state[k].value / 4095.0f * 5;
  for (int row = 0; row < 5; row++) {
    int fromBottom = 4 - row;
    float fill = constrain(level - fromBottom, 0.0f, 1.0f);
    CRGB c = colour;
    c.nscale8_video((uint8_t)(40 + 215 * fill));
    if (fill <= 0) {
      c = CRGB(colour).nscale8_video(18);   // a dim outline, so an empty bar still shows
    }
    putLed(row, col, c);
    putLed(row, col + 1, c);
  }
}

void draw() {
  unsigned long now = millis();
  if (now - lastDrawMs < 40) {
    return;
  }
  lastDrawMs = now;
  for (int n = 0; n < 25; n++) {
    leds[n] = CRGB::Black;
  }
  drawPin(shownPort * 2, 0);
  drawPin(shownPort * 2 + 1, 3);
  // which port: a dot in the middle column, top = A, middle = B, bottom = C
  CRGB marker = keyTest.armed ? CRGB(255, 110, 0) : CRGB(180, 180, 200);   // amber while the key test is armed
  putLed(shownPort * 2, 2, marker);
  if (keyTest.ledAdded) {
    keyLed[0] = CHSV((now / 10) % 256, 255, 255);   // cycle through colours
  }
  FastLED.show();
}

// =====================================================================
//  Serial
// =====================================================================
void printPins() {
  unsigned long now = millis();
  if (now - lastPrintMs < 500) {
    return;
  }
  lastPrintMs = now;
  for (int k = 0; k < PIN_COUNT; k++) {
    if (isDrivenByUs(k)) {
      Serial.printf("%s G%d  key LED   ", PINS[k].port, PINS[k].pin);
    } else {
      Serial.printf("%s G%d %4d %s  ", PINS[k].port, PINS[k].pin, state[k].value,
                    state[k].driven ? "driven" : "empty ");
    }
    if (k % 2 == 1) {
      Serial.printf("| ");
    }
  }
  Serial.printf("\n");
}

// =====================================================================
//  The button
// =====================================================================
void updateButton() {
  bool down = digitalRead(BUTTON_PIN) == LOW;
  unsigned long now = millis();
  if (down && !buttonDown) {
    buttonDown = true;
    buttonDownMs = now;
  } else if (!down && buttonDown) {
    buttonDown = false;
    if (now - buttonDownMs < 30) {
      return;                         // a bounce
    }
    if (now - buttonDownMs < 700) {
      shownPort = (shownPort + 1) % 3;
      Serial.printf("Showing Port %s\n", PORT_NAMES[shownPort]);
    } else {
      armOrStopKeyTest();
    }
  }
}

// =====================================================================
//  Setup / loop
// =====================================================================
void setup() {
  Serial.begin(115200);
  delay(200);
  pinMode(BUTTON_PIN, INPUT);
  FastLED.addLeds<WS2812, MATRIX_PIN, GRB>(leds, 25);
  FastLED.setBrightness(LED_BRIGHTNESS);
  FastLED.clear(true);
  for (int k = 0; k < PIN_COUNT; k++) {
    prepareForReading(k);
  }
  Serial.println("Unit tester: short press = next port, long press = key LED test on this port");
  Serial.println("Each pin: reading (0..4095) and whether something is driving it");
}

void loop() {
  updateButton();
  updatePins();
  updateKeyTest();
  draw();
  printPins();
}
