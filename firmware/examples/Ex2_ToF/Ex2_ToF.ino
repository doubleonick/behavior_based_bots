/*
  Example 2: read a 90 degree ToF Unit (distance) and draw a bar
  ==============================================================

  The M5 ToF Unit (VL53L0X chip) times how long a flash of infrared light takes to
  bounce back, and says how far away something is, in millimetres (about 30 to 1200).

  HOOK UP
    Plug the ToF Unit into ANY of the three Grove ports. It talks I2C, which works
    on all three, and gives a real distance (a level) on all three:

      port      where                      pins          ToF works?
      Port A    the Atom Matrix's socket   G26, G32      yes, distance in mm
      Port B    the Motion's socket        G33, G23      yes, distance in mm
      Port C    the Motion's other socket  G19, G22      yes, distance in mm

    Set PORT_PIN_1 / PORT_PIN_2 below to match. Which pin is SDA and which is SCL is
    tried both ways, so the order doesn't matter.
    (For a ToF on a PaHub, see HUB_CHANNEL below.)

  WHAT YOU SEE
    The nearer something is, the more of the matrix lights up (purple), bottom row
    first: nothing within NEAR_MM..FAR_MM = dark, closer than NEAR_MM = full.
    Serial (115200) prints the distance in mm.

  Needs: FastLED, and the "VL53L0X" library by Pololu (Arduino Library Manager).
*/
#include <Wire.h>
#include <FastLED.h>
#include <VL53L0X.h>

// The port: pick ONE pair
const int PORT_PIN_1 = 26;            // Port A
const int PORT_PIN_2 = 32;
// const int PORT_PIN_1 = 33;         // Port B
// const int PORT_PIN_2 = 23;
// const int PORT_PIN_1 = 19;         // Port C
// const int PORT_PIN_2 = 22;

// Only for a ToF plugged into a PaHub (in the port above): its channel, 0..5.
// -1 = no PaHub, the ToF is plugged straight into the port.
int HUB_CHANNEL = -1;
const uint8_t HUB_ADDRESS = 0x70;

const int NEAR_MM = 50;               // this close or closer: a full bar
const int FAR_MM = 1000;              // this far or further: no bar

const int MATRIX_PIN = 27;
CRGB leds[25];
VL53L0X tof;

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

// PaHub only: connect the hub's channel to the port (one bit per channel)
void selectHubChannel() {
  if (HUB_CHANNEL < 0) {
    return;
  }
  Wire1.beginTransmission(HUB_ADDRESS);
  Wire1.write(1 << HUB_CHANNEL);
  Wire1.endTransmission();
}

// Start I2C on the port with SDA on one pin and SCL on the other, and look for the ToF
// (address 0x29). Returns true if it answered.
bool tryPins(int sda, int scl) {
  Wire1.end();
  Wire1.begin(sda, scl);
  delay(5);
  selectHubChannel();
  Wire1.beginTransmission(0x29);
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
    Serial.println("No ToF found: check the port and the pins above");
    return;
  }

  tof.setBus(&Wire1);                 // the ToF is on Wire1 (Wire is the Motion's)
  tof.setTimeout(100);
  if (!tof.init()) {
    Serial.println("Something answered, but the ToF did not start");
    return;
  }
  tof.startContinuous();              // measure non-stop
  Serial.println("ToF found");
}

void loop() {
  int mm = tof.readRangeContinuousMillimeters();
  if (tof.timeoutOccurred()) {
    Serial.println("no answer from the ToF");
    drawBar(0, CRGB(0, 0, 0));
    delay(100);
    return;
  }

  // Nearer = more LEDs: NEAR_MM -> 25, FAR_MM -> 0
  int clamped = constrain(mm, NEAR_MM, FAR_MM);
  int count = map(clamped, FAR_MM, NEAR_MM, 0, 25);
  drawBar(count, CRGB(150, 0, 255));

  Serial.printf("distance: %d mm\n", mm);
  delay(50);
}
