/*
  Example 1: drive a servo
  ========================

  The smallest program that turns a servo plugged into the Atom Motion base.

  HOOK UP
    - Sit the Atom Matrix on the Atom Motion base.
    - Plug the servo into the Motion's S1 header (S1 to S4 all work: see SERVO below).
      Brown / black wire = ground, red = power, orange / yellow = signal: check the
      markings beside the header.
    - Switch the Motion ON. The servo gets its power from the Motion's battery, not
      from USB: with the switch off, nothing moves.

  HOW IT WORKS
    The Atom (ESP32) can't drive the servo pins itself. The Motion has its own little
    computer (an STM32) that does. The Atom talks to it over I2C, on the Atom's
    inside pins G25 (SDA) and G21 (SCL), at address 0x38.
    You send it two bytes: which servo (register 0, 1, 2 or 3 = S1, S2, S3, S4) and
    an angle from 0 to 180.

  WHAT THE ANGLE MEANS
    Continuous-rotation servo (a wheel):   90 = stop
                                            above 90 = turn one way, faster toward 180
                                            below 90 = turn the other way, faster toward 0
                                           (if 90 still creeps, try 88..92)
    Ordinary servo (an arm):               the angle it goes to and holds, 0..180

  WHAT YOU SEE
    The servo turns one way for 2 s (green on the matrix), stops for 1 s (dark),
    turns the other way for 2 s (red), stops for 1 s, and repeats.
    Serial (115200) says what it is doing.

  Needs: FastLED (for the matrix only).
*/
#include <Wire.h>
#include <FastLED.h>

const uint8_t MOTION_ADDRESS = 0x38;  // the Motion base's I2C address
const uint8_t SERVO = 0;              // 0 = S1, 1 = S2, 2 = S3, 3 = S4

const int MATRIX_PIN = 27;            // the 5x5 matrix
CRGB leds[25];

// Tell the Motion to put the servo at this angle (0..180)
void setServoAngle(uint8_t servo, uint8_t angle) {
  Wire.beginTransmission(MOTION_ADDRESS);
  Wire.write(servo);                  // which servo
  Wire.write(angle);                  // where (or, for a wheel, how fast and which way)
  Wire.endTransmission();
}

// Light the whole matrix one colour
void showColour(CRGB colour) {
  for (int k = 0; k < 25; k++) {
    leds[k] = colour;
  }
  FastLED.show();
}

void setup() {
  Serial.begin(115200);
  Wire.begin(25, 21);                 // the Atom's inside I2C: SDA = G25, SCL = G21
  FastLED.addLeds<WS2812, MATRIX_PIN, GRB>(leds, 25);
  FastLED.setBrightness(20);          // keep low: the matrix is very bright

  Wire.beginTransmission(MOTION_ADDRESS);
  if (Wire.endTransmission() == 0) {
    Serial.println("Motion base found");
  } else {
    Serial.println("Motion base NOT found: is the Atom seated, and the Motion switched on?");
  }
}

void loop() {
  Serial.println("one way");
  setServoAngle(SERVO, 135);          // halfway between stop (90) and full speed (180)
  showColour(CRGB(0, 80, 0));
  delay(2000);

  Serial.println("stop");
  setServoAngle(SERVO, 90);
  showColour(CRGB(0, 0, 0));
  delay(1000);

  Serial.println("the other way");
  setServoAngle(SERVO, 45);           // halfway between stop (90) and full speed (0)
  showColour(CRGB(80, 0, 0));
  delay(2000);

  Serial.println("stop");
  setServoAngle(SERVO, 90);
  showColour(CRGB(0, 0, 0));
  delay(1000);
}
