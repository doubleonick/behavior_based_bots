/*
  Example 6: read a Color Unit (light and colour) and draw a bar for each channel
  ===============================================================================

  The M5 Color Unit (TCS3472 chip) measures light through four filters:
    R (red), G (green), B (blue), and C ("clear": all the light, no filter).
  C is the one to use as a plain light sensor. R, G and B together tell the colour.

  HOOK UP
    Plug the Color Unit into ANY of the three Grove ports. It talks I2C, which works
    on all three, and gives levels on all three:

      port      where                      pins          Color Unit works?
      Port A    the Atom Matrix's socket   G26, G32      yes, all four levels
      Port B    the Motion's socket        G33, G23      yes, all four levels
      Port C    the Motion's other socket  G19, G22      yes, all four levels

    Set PORT_PIN_1 / PORT_PIN_2 below to match. Which pin is SDA and which is SCL is
    tried both ways, so the order doesn't matter.
    (For a Color Unit on a PaHub, see HUB_CHANNEL below.)

  HOW IT WORKS (no library needed)
    The chip is at I2C address 0x29 (the ToF Unit is too: if both are plugged in, put
    them in different ports or on different PaHub channels). You write a few setup
    registers once, then read 8 bytes: C, R, G, B, two bytes each, low byte first.
    Every register number is sent with 0x80 added (the chip's "command" bit).

  WHAT YOU SEE
    Four bars, filling from the bottom:
      column 0 = R (red), 1 = G (green), 2 = B (blue), 3 = C (white), column 4 dark.
    Bars use a square root, so dim indoor light still shows: a bar is full at
    FULL_SCALE. Hold something red over it: the red bar rises most.
    Serial (115200) prints all four numbers.

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

// Only for a Color Unit plugged into a PaHub (in the port above): its channel, 0..5.
// -1 = no PaHub, the Color Unit is plugged straight into the port.
int HUB_CHANNEL = -1;
const uint8_t HUB_ADDRESS = 0x70;

const uint8_t COLOR_ADDRESS = 0x29;
const uint8_t COMMAND = 0x80;         // added to every register number
const long FULL_SCALE = 20000;        // a reading this big fills a bar

const int MATRIX_PIN = 27;
CRGB leds[25];

// One column's bar: fraction 0..1, filling from the bottom
void drawColumn(int col, float fraction, CRGB colour) {
  int count = (int)(fraction * 5 + 0.5f);
  for (int row = 0; row < 5; row++) {
    bool lit = (4 - row) < count;     // row 4 is the bottom
    if (lit) {
      leds[row * 5 + col] = colour;
    } else {
      leds[row * 5 + col] = CRGB(0, 0, 0);
    }
  }
}

// A reading as a bar fraction, square-root scaled so small readings still show
float barFor(long reading) {
  float fraction = (float)reading / FULL_SCALE;
  return sqrtf(constrain(fraction, 0.0f, 1.0f));
}

void writeRegister(uint8_t reg, uint8_t value) {
  Wire1.beginTransmission(COLOR_ADDRESS);
  Wire1.write(COMMAND | reg);
  Wire1.write(value);
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

// Start I2C with SDA on one pin and SCL on the other, and look for the chip
bool tryPins(int sda, int scl) {
  Wire1.end();
  Wire1.begin(sda, scl);
  delay(5);
  selectHubChannel();
  Wire1.beginTransmission(COLOR_ADDRESS);
  return Wire1.endTransmission() == 0;
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
    Serial.println("Nothing at 0x29: check the port and the pins above");
    return;
  }

  writeRegister(0x01, 0xC0);          // ATIME: measure for 64 steps (about 154 ms)
  writeRegister(0x0F, 0x01);          // CONTROL: gain 4x (0 = 1x, 1 = 4x, 2 = 16x, 3 = 60x)
  writeRegister(0x00, 0x01);          // ENABLE: power on
  delay(3);
  writeRegister(0x00, 0x03);          // ENABLE: power on + start measuring
  Serial.println("Color Unit started");
}

void loop() {
  // Ask for 8 bytes starting at register 0x14 (0x20 = read several in a row)
  Wire1.beginTransmission(COLOR_ADDRESS);
  Wire1.write(COMMAND | 0x20 | 0x14);
  Wire1.endTransmission();
  if (Wire1.requestFrom((int)COLOR_ADDRESS, 8) != 8) {
    Serial.println("no answer from the Color Unit");
    delay(200);
    return;
  }
  uint8_t b[8];
  for (int i = 0; i < 8; i++) {
    b[i] = Wire1.read();
  }
  long c = b[0] | (b[1] << 8);        // low byte first
  long r = b[2] | (b[3] << 8);
  long g = b[4] | (b[5] << 8);
  long bl = b[6] | (b[7] << 8);

  drawColumn(0, barFor(r), CRGB(255, 0, 0));
  drawColumn(1, barFor(g), CRGB(0, 255, 0));
  drawColumn(2, barFor(bl), CRGB(0, 0, 255));
  drawColumn(3, barFor(c), CRGB(255, 255, 255));
  drawColumn(4, 0, CRGB(0, 0, 0));
  FastLED.show();

  Serial.printf("R %5ld   G %5ld   B %5ld   C (light) %5ld\n", r, g, bl, c);
  delay(160);                         // a new measurement every ~154 ms
}
