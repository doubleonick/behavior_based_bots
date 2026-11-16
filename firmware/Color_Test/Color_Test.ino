/*
  Color_Test - just the M5 Color Unit (TCS3472), on one port, on the Atom Matrix.

  The whole matrix turns the colour the Color Unit sees. Hold it over something red:
  the matrix goes red.

  The Color Unit measures light through four filters: R (red), G (green), B (blue)
  and C ("clear": all the light). R, G and B together tell the colour; C tells how
  much light there is.

  Two things make "the colour it sees" less simple than it sounds, so there are two
  helpers:

    White balance (press the screen). The chip does not feel red, green and blue
    equally, and lamps are not pure white, so a white sheet of paper usually comes
    out yellowish. Hold the Color Unit over something WHITE and press the screen:
    from then on that counts as white. Until you do, the raw readings are used.

    SHOW_BRIGHTNESS (a setting below).
      false: the matrix shows the COLOUR at full strength, however dim the light is.
             (Below TOO_DARK there is not enough light to tell, and it goes dark.)
      true:  the matrix also gets dimmer when what the unit sees is dimmer.
             FULL_SCALE is the reading that counts as full brightness.

  Matrix:
    all 25 LEDs     the colour the Color Unit sees
    purple cross    no Color Unit found
    white flash     white balance taken

  Serial (115200), about 6 times a second: the four raw readings, and the red,
  green and blue (0..255) sent to the matrix.

  Pick the port by uncommenting ONE pair below, then upload. Which pin is SDA and
  which is SCL is tried both ways. (For a Color Unit on a PaHub, see HUB_CHANNEL.)

  Needs: FastLED.
*/
#include <Wire.h>
#include <FastLED.h>

// ---------- Settings ----------
// The port: pick ONE pair
// const int PORT_PIN_1 = 26;         // Port A
// const int PORT_PIN_2 = 32;
const int PORT_PIN_1 = 33;            // Port B (where the Color Unit usually goes)
const int PORT_PIN_2 = 23;
// const int PORT_PIN_1 = 19;         // Port C
// const int PORT_PIN_2 = 22;

// Only for a Color Unit plugged into a PaHub (in the port above): its channel, 0..5.
// -1 = no PaHub, the Color Unit is plugged straight into the port.
int HUB_CHANNEL = -1;

const bool SHOW_BRIGHTNESS = false;   // false: colour only. true: dimmer light, dimmer matrix.
const long FULL_SCALE = 20000;        // SHOW_BRIGHTNESS: a C reading this big is full brightness
const long TOO_DARK = 30;             // a C reading below this: too dark to tell a colour

const int MATRIX_PIN = 27;
const int BUTTON_PIN = 39;
const int LED_BRIGHTNESS = 20;        // keep low: the matrix is very bright and gets warm

// ---------- Hardware ----------
const uint8_t HUB_ADDRESS = 0x70;
const uint8_t COLOR_ADDRESS = 0x29;
const uint8_t COMMAND = 0x80;         // added to every register number

CRGB leds[25];
bool found = false;

// What the Color Unit read last
long rawR = 0;
long rawG = 0;
long rawB = 0;
long rawC = 0;

// White balance: each channel is multiplied by its own number. 1 = leave it alone.
float balanceR = 1;
float balanceG = 1;
float balanceB = 1;
bool buttonWasDown = false;

// ---------- The matrix ----------
void fill(CRGB colour) {
  for (int k = 0; k < 25; k++) {
    leds[k] = colour;
  }
  FastLED.show();
}

void showCross() {
  for (int k = 0; k < 25; k++) {
    leds[k] = CRGB(0, 0, 0);
  }
  for (int k = 0; k < 5; k++) {
    leds[k * 5 + k] = CRGB(160, 0, 200);
    leds[k * 5 + (4 - k)] = CRGB(160, 0, 200);
  }
  FastLED.show();
}

// ---------- The Color Unit ----------
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

// Read all four channels. Returns false if the unit did not answer.
bool readColor() {
  // Ask for 8 bytes starting at register 0x14 (0x20 = read several in a row)
  Wire1.beginTransmission(COLOR_ADDRESS);
  Wire1.write(COMMAND | 0x20 | 0x14);
  Wire1.endTransmission();
  if (Wire1.requestFrom((int)COLOR_ADDRESS, 8) != 8) {
    return false;
  }
  uint8_t b[8];
  for (int i = 0; i < 8; i++) {
    b[i] = Wire1.read();
  }
  rawC = b[0] | (b[1] << 8);          // low byte first
  rawR = b[2] | (b[3] << 8);
  rawG = b[4] | (b[5] << 8);
  rawB = b[6] | (b[7] << 8);
  return true;
}

// ---------- White balance ----------
// What the unit sees now is white: choose the three numbers that make its red, green
// and blue come out equal (each as big as the biggest of them).
void takeWhiteBalance() {
  if (rawR < 1 || rawG < 1 || rawB < 1) {
    Serial.println("White balance: too dark, not taken");
    return;
  }
  long biggest = max(rawR, max(rawG, rawB));
  balanceR = (float)biggest / rawR;
  balanceG = (float)biggest / rawG;
  balanceB = (float)biggest / rawB;
  Serial.printf("White balance taken: R x%.2f  G x%.2f  B x%.2f\n", balanceR, balanceG, balanceB);
  fill(CRGB(255, 255, 255));
  delay(200);
}

// ---------- Setup / loop ----------
void setup() {
  Serial.begin(115200);
  delay(200);
  pinMode(BUTTON_PIN, INPUT);
  FastLED.addLeds<WS2812, MATRIX_PIN, GRB>(leds, 25);
  FastLED.setBrightness(LED_BRIGHTNESS);

  // Try both wire orders
  found = tryPins(PORT_PIN_1, PORT_PIN_2);
  if (!found) {
    found = tryPins(PORT_PIN_2, PORT_PIN_1);
  }
  if (!found) {
    Serial.println("Color test: nothing at 0x29. Check the port and the pins at the top.");
    showCross();
    return;
  }

  writeRegister(0x01, 0xC0);          // ATIME: measure for 64 steps (about 154 ms)
  writeRegister(0x0F, 0x01);          // CONTROL: gain 4x (0 = 1x, 1 = 4x, 2 = 16x, 3 = 60x)
  writeRegister(0x00, 0x01);          // ENABLE: power on
  delay(3);
  writeRegister(0x00, 0x03);          // ENABLE: power on + start measuring
  Serial.println("Color test: Color Unit started. Press the screen over something white.");
}

void loop() {
  if (!found) {
    delay(200);
    return;
  }
  if (!readColor()) {
    Serial.println("no answer from the Color Unit");
    showCross();
    delay(200);
    return;
  }

  // A press of the screen (once per press): what it sees now is white
  bool buttonDown = (digitalRead(BUTTON_PIN) == LOW);
  if (buttonDown && !buttonWasDown) {
    takeWhiteBalance();
  }
  buttonWasDown = buttonDown;

  // The colour: red, green and blue after white balance
  float r = rawR * balanceR;
  float g = rawG * balanceG;
  float b = rawB * balanceB;
  float biggest = max(r, max(g, b));

  int red = 0;
  int green = 0;
  int blue = 0;
  if (rawC >= TOO_DARK && biggest > 0) {
    // Stretch so the strongest of the three is 255: that is the colour at full strength
    float strength = 1;
    if (SHOW_BRIGHTNESS) {
      // ... then dim it by how much light there is (square root, so indoor light still shows)
      strength = sqrtf(constrain((float)rawC / FULL_SCALE, 0.0f, 1.0f));
    }
    red   = (int)(255 * strength * r / biggest);
    green = (int)(255 * strength * g / biggest);
    blue  = (int)(255 * strength * b / biggest);
  }
  fill(CRGB(red, green, blue));

  Serial.printf("R %5ld   G %5ld   B %5ld   C %5ld   ->  matrix  r %3d  g %3d  b %3d\n",
                rawR, rawG, rawB, rawC, red, green, blue);
  delay(160);                         // a new measurement every ~154 ms
}
