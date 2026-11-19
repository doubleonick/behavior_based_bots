/*
  Example 7: read a DLight Unit (light, in lux) and draw it on the matrix
  =======================================================================

  The M5 DLight Unit (U136, BH1750FVI chip) measures how bright it is, the way our
  eyes judge brightness, in LUX:
      about 1 lux     a dark room, a candle a metre away
      about 300 lux   an ordinary indoor room
      about 1000 lux  under a bright desk lamp, or a flashlight close up
      10 000 lux +    daylight outdoors
  Unlike the Color Unit it doesn't see colour, and unlike the Light Unit it gives a
  real number that means the same on every robot.

  HOOK UP
    Plug the DLight Unit into ANY of the three Grove ports. It talks I2C, which works
    on all three:

      port      where                      pins
      Port A    the Atom Matrix's socket   G26, G32
      Port B    the Motion's socket        G33, G23
      Port C    the Motion's other socket  G19, G22

    Set PORT_PIN_1 / PORT_PIN_2 below to match. Which pin is SDA and which is SCL is
    tried both ways, so the order doesn't matter.
    (For a DLight Unit on a PaHub, see HUB_CHANNEL below.)

  HOW IT WORKS (no library needed)
    The chip answers at I2C address 0x23 (or 0x5C on some boards: both are tried).
    You send it one-byte commands: 0x01 = power on, 0x10 = measure again and again,
    at 1 lux steps (a new reading about every 120 ms). Then read 2 bytes, high byte
    first. That number divided by 1.2 is the brightness in lux.

  WHAT YOU SEE
    A bar that fills the matrix from the bottom, row by row: 0 to 25 LEDs.
    Lux goes from 0 to over 50 000, so the bar counts in steps that grow (a
    logarithmic scale): each extra row is about 9 times brighter.
      dark room: a few LEDs.   room light: about half.   flashlight: nearly full.
    The colour goes from blue (dim) to yellow (bright).
    Serial (115200) prints the lux.

  Needs: FastLED.
*/
#include <Wire.h>
#include <FastLED.h>

// The port: pick ONE pair
const int PORT_PIN_1 = 26;            // Port A
const int PORT_PIN_2 = 32;
// const int PORT_PIN_1 = 33;         // Port B
// const int PORT_PIN_2 = 23;
// const int PORT_PIN_1 = 19;         // Port C
// const int PORT_PIN_2 = 22;

// Only for a DLight Unit plugged into a PaHub (in the port above): its channel, 0..5.
// -1 = no PaHub, the DLight Unit is plugged straight into the port.
int HUB_CHANNEL = -1;
const uint8_t HUB_ADDRESS = 0x70;

const uint8_t POWER_ON = 0x01;
const uint8_t RESET = 0x07;
const uint8_t MEASURE_AGAIN_AND_AGAIN = 0x10;   // continuous, 1 lux steps, ~120 ms each
const float FULL_BAR_LUX = 50000;     // this bright (or brighter) fills the whole matrix

const int MATRIX_PIN = 27;
CRGB leds[25];
uint8_t address = 0;                  // which address the chip answered at (0 = none)

// Send the chip a one-byte command
void command(uint8_t c) {
  Wire1.beginTransmission(address);
  Wire1.write(c);
  Wire1.endTransmission();
}

// PaHub only: connect the hub's channel to the port (one bit per channel)
void selectHubChannel() {
  if (HUB_CHANNEL < 0) {
    return;
  }
  Wire1.beginTransmission(HUB_ADDRESS);
  Wire1.write(1 << HUB_CHANNEL);
  Wire1.endTransmission();
}

// Does anything answer at this address?
bool answers(uint8_t a) {
  Wire1.beginTransmission(a);
  return Wire1.endTransmission() == 0;
}

// Start I2C with SDA on one pin and SCL on the other, and look for the chip at
// either of its two addresses
bool tryPins(int sda, int scl) {
  Wire1.end();
  Wire1.begin(sda, scl);
  delay(5);
  selectHubChannel();
  if (answers(0x23)) {
    address = 0x23;
    return true;
  }
  if (answers(0x5C)) {
    address = 0x5C;
    return true;
  }
  return false;
}

// How many LEDs to light for a brightness: 0 lux = none, FULL_BAR_LUX = all 25.
// Counted logarithmically (log of 1 + lux), so dim light still shows.
int ledsFor(float lux) {
  float fraction = log10f(1 + lux) / log10f(1 + FULL_BAR_LUX);
  int count = (int)(fraction * 25 + 0.5f);
  if (count < 0) {
    count = 0;
  }
  if (count > 25) {
    count = 25;
  }
  return count;
}

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2812, MATRIX_PIN, GRB>(leds, 25);
  FastLED.setBrightness(20);

  // Try both wire orders
  bool found = tryPins(PORT_PIN_1, PORT_PIN_2);
  if (!found) {
    found = tryPins(PORT_PIN_2, PORT_PIN_1);
  }
  if (!found) {
    Serial.println("No DLight at 0x23 or 0x5C: check the port and the pins above");
    for (int k = 0; k < 5; k++) {
      leds[k * 5 + k] = CRGB(150, 0, 200);          // a purple cross: not found
      leds[k * 5 + (4 - k)] = CRGB(150, 0, 200);
    }
    FastLED.show();
    return;
  }

  command(POWER_ON);
  command(RESET);
  command(MEASURE_AGAIN_AND_AGAIN);
  delay(180);                         // the first reading takes a moment
  Serial.printf("DLight Unit started (address 0x%02X)\n", address);
}

void loop() {
  if (address == 0) {
    delay(200);
    return;
  }
  // Read the 2-byte result, high byte first
  if (Wire1.requestFrom((int)address, 2) != 2) {
    Serial.println("no answer from the DLight Unit");
    delay(200);
    return;
  }
  uint16_t raw = (Wire1.read() << 8) | Wire1.read();
  float lux = raw / 1.2f;

  // The bar: fill from the bottom-left, row by row; blue when dim, yellow when bright
  int count = ledsFor(lux);
  for (int k = 0; k < 25; k++) {
    int row = 4 - k / 5;              // bottom row first
    int col = k % 5;
    if (k < count) {
      float warmth = (float)k / 24;   // 0 at the bottom, 1 at the top
      leds[row * 5 + col] = CRGB((uint8_t)(255 * warmth), (uint8_t)(60 + 160 * warmth), (uint8_t)(255 * (1 - warmth)));
    } else {
      leds[row * 5 + col] = CRGB(0, 0, 0);
    }
  }
  FastLED.show();

  Serial.printf("%.1f lux  (%d of 25 LEDs)\n", lux, count);
  delay(150);                         // a new measurement about every 120 ms
}
