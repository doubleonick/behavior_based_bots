/*
  IMU_Test - just the Atom Matrix's own motion sensor (its IMU). No extra parts.

  The IMU feels which way is down (gravity), and every push, jolt and drop. This
  sketch is for finding out what it can tell us about Val: tipping its nose over a
  table edge, being picked up, lying on its side, falling.

  Which chip your Matrix has is printed at the start:
    Atom Matrix v1.1: BMI270  (read with the "SparkFun BMI270 Arduino Library")
    Atom Matrix v1.0: MPU6886 (read directly; this part is written from the chip's
                               data sheet and has not been tried on a real one)

  Matrix: a bubble level. A dot sits in the middle when Val is level, and slides
  towards whichever side is LOW. Its colour says what the IMU makes of things:
    green   level (tilted less than TILT_LIMIT_DEG)
    red     tilted more than that
    amber   the whole matrix: falling (it feels almost no gravity)
    white   the whole matrix, for a moment: a jolt
    purple  cross: no IMU found

  Button (press the screen): "this is level". Val takes the way it is sitting now as
  level, so tilt reads 0 from here. It also does this once at the start, so start with
  Val sitting on the table.

  Serial (115200), 10 times a second:
    x y z      what the IMU feels along each of its axes, in g (1 g = gravity)
    forward    how far the nose is down (+) or up (-), in degrees
    left       how far the left side is down (+) or up (-), in degrees
    total      how much it feels altogether: about 1.00 sitting still, near 0 falling
    jolt       the size of the latest sudden change, in g
    and one word: LEVEL, TILTED, FALLING or JOLT

  The wheel (optional): set WHEEL_ANGLE to something other than 90 and the servo in
  S1 turns (90 = stop; 100 is a slow crawl one way, 80 the other). With
  STOP_WHEN_NOT_LEVEL true the wheel stops whenever the word is not LEVEL, and starts
  again once Val has been level for a second. That is the safety idea being tried.
  HOLD VAL, or put something soft under the table edge, while you try it.

  Needs: FastLED, and the SparkFun BMI270 Arduino Library.
*/
#include <Wire.h>
#include <FastLED.h>
#include "SparkFun_BMI270_Arduino_Library.h"

// ---------- Settings ----------
// Which IMU axis points to Val's front and which to its left (0 = x, 1 = y, 2 = z),
// and their signs. This depends on how the Atom is mounted. To find them: watch
// x y z on Serial and tip Val's nose DOWN: the axis that goes up towards +1 is the
// forward axis with sign +1 (if it goes towards -1, its sign is -1). Then the same
// with the LEFT side down. The axis left over is the one that points down.
const int   FORWARD_AXIS = 0;
const float FORWARD_SIGN = 1.0f;
const int   LEFT_AXIS    = 1;
const float LEFT_SIGN    = 1.0f;

const float TILT_LIMIT_DEG = 12.0f;   // more tilt than this is TILTED
const float FALLING_G      = 0.4f;    // feeling less than this in total is FALLING
const float JOLT_G         = 0.6f;    // a sudden change bigger than this is a JOLT
const unsigned long LEVEL_AGAIN_MS = 1000;   // level for this long before the wheel restarts

const int  WHEEL_ANGLE = 90;          // 90 = the wheel stays still. Try 100 for a slow crawl.
const bool STOP_WHEN_NOT_LEVEL = true;

const int MATRIX_PIN = 27;
const int BUTTON_PIN = 39;
const int LED_BRIGHTNESS = 20;

// ---------- Hardware ----------
const uint8_t IMU_ADDRESS = 0x68;     // both chips answer here
const uint8_t MOTION_ADDRESS = 0x38;  // the Atom Motion base (the servos)

CRGB leds[25];
BMI270 bmi;

const int NO_IMU = 0;
const int BMI270_CHIP = 1;
const int MPU6886_CHIP = 2;
int chip = NO_IMU;

// ---------- What the IMU says ----------
float accel[3] = {0, 0, 1};           // the latest reading, in g
float steady[3] = {0, 0, 1};          // the same, followed slowly (gravity, without the shaking)
float jolt = 0;                       // the size of the latest sudden change, in g
float forwardDeg = 0;                 // nose down is +
float leftDeg = 0;                    // left side down is +
float totalG = 1;
float forwardZero = 0;                // what "level" read when the button was last pressed
float leftZero = 0;

const int LEVEL = 0;
const int TILTED = 1;
const int FALLING = 2;
const int JOLT = 3;
int state = LEVEL;
unsigned long joltUntilMs = 0;        // JOLT is shown for a moment so it can be seen
unsigned long notLevelMs = 0;         // when Val was last not level
bool wheelTurning = false;
unsigned long lastReadMs = 0;
unsigned long lastPrintMs = 0;
bool zeroPending = true;              // take "level" from the first steady readings

// ---------- The MPU6886 (Atom Matrix v1.0), read directly ----------
uint8_t readRegister(uint8_t reg) {
  Wire.beginTransmission(IMU_ADDRESS);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(IMU_ADDRESS, (uint8_t)1);
  if (Wire.available()) {
    return Wire.read();
  }
  return 0;
}

void writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(IMU_ADDRESS);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

bool beginMpu6886() {
  if (readRegister(0x75) != 0x19) {   // WHO_AM_I: an MPU6886 answers 0x19
    return false;
  }
  writeRegister(0x6B, 0x00);          // wake up
  delay(10);
  writeRegister(0x1C, 0x10);          // accelerometer range: +-8 g (4096 counts per g)
  return true;
}

bool readMpu6886() {
  Wire.beginTransmission(IMU_ADDRESS);
  Wire.write(0x3B);                   // the six accelerometer bytes start here
  Wire.endTransmission(false);
  Wire.requestFrom(IMU_ADDRESS, (uint8_t)6);
  if (Wire.available() < 6) {
    return false;
  }
  for (int k = 0; k < 3; k++) {
    int16_t counts = (int16_t)((Wire.read() << 8) | Wire.read());
    accel[k] = counts / 4096.0f;
  }
  return true;
}

// ---------- Reading whichever chip is there ----------
void beginImu() {
  if (bmi.beginI2C(IMU_ADDRESS, Wire) == BMI2_OK) {
    chip = BMI270_CHIP;
    Serial.println("IMU: BMI270 (Atom Matrix v1.1)");
  } else if (beginMpu6886()) {
    chip = MPU6886_CHIP;
    Serial.println("IMU: MPU6886 (Atom Matrix v1.0)");
  } else {
    chip = NO_IMU;
    Serial.println("IMU: NOT found");
  }
}

bool readImu() {
  if (chip == BMI270_CHIP) {
    if (bmi.getSensorData() != BMI2_OK) {
      return false;
    }
    accel[0] = bmi.data.accelX;
    accel[1] = bmi.data.accelY;
    accel[2] = bmi.data.accelZ;
    return true;
  }
  if (chip == MPU6886_CHIP) {
    return readMpu6886();
  }
  return false;
}

// ---------- Working out tilt, falling and jolts ----------
void think() {
  // How much the reading has suddenly changed, and the slow, steady part
  float change = 0;
  for (int k = 0; k < 3; k++) {
    float sudden = accel[k] - steady[k];
    change += sudden * sudden;
    steady[k] += 0.2f * sudden;
  }
  jolt = sqrtf(change);
  totalG = sqrtf(accel[0] * accel[0] + accel[1] * accel[1] + accel[2] * accel[2]);

  // The axis that is neither forward nor left points down (or up) when Val is level
  int downAxis = 3 - FORWARD_AXIS - LEFT_AXIS;
  float forward = FORWARD_SIGN * steady[FORWARD_AXIS];
  float left = LEFT_SIGN * steady[LEFT_AXIS];
  float down = fabsf(steady[downAxis]);
  float rawForwardDeg = atan2f(forward, down) * 180.0f / PI;
  float rawLeftDeg = atan2f(left, down) * 180.0f / PI;

  if (zeroPending && millis() > 500) {
    forwardZero = rawForwardDeg;      // "this is level"
    leftZero = rawLeftDeg;
    zeroPending = false;
    Serial.println("Level set here");
  }
  forwardDeg = rawForwardDeg - forwardZero;
  leftDeg = rawLeftDeg - leftZero;

  // One word for what is going on. A jolt is shown for a third of a second.
  if (jolt >= JOLT_G) {
    joltUntilMs = millis() + 300;
  }
  if (totalG < FALLING_G) {
    state = FALLING;
  } else if (millis() < joltUntilMs) {
    state = JOLT;
  } else if (fabsf(forwardDeg) > TILT_LIMIT_DEG || fabsf(leftDeg) > TILT_LIMIT_DEG) {
    state = TILTED;
  } else {
    state = LEVEL;
  }
  if (state != LEVEL) {
    notLevelMs = millis();
  }
}

const char* stateName() {
  if (state == TILTED) {
    return "TILTED";
  }
  if (state == FALLING) {
    return "FALLING";
  }
  if (state == JOLT) {
    return "JOLT";
  }
  return "LEVEL";
}

// ---------- The wheel (servo in S1, through the Motion's angle register) ----------
void setWheel(int degrees) {
  Wire.beginTransmission(MOTION_ADDRESS);
  Wire.write(0x00);                   // register 0x00 = S1's angle
  Wire.write((uint8_t)degrees);
  Wire.endTransmission();
}

void updateWheel() {
  bool shouldTurn = (WHEEL_ANGLE != 90);
  if (shouldTurn && STOP_WHEN_NOT_LEVEL) {
    // stopped while not level, and until it has been level for a while
    if (state != LEVEL || millis() - notLevelMs < LEVEL_AGAIN_MS) {
      shouldTurn = false;
    }
  }
  if (shouldTurn != wheelTurning) {
    wheelTurning = shouldTurn;
    Serial.println(wheelTurning ? "Wheel: turning" : "Wheel: STOPPED");
  }
  if (wheelTurning) {
    setWheel(WHEEL_ANGLE);
  } else {
    setWheel(90);
  }
}

// ---------- The matrix: index = row * 5 + col, row 0 at the top ----------
void fill(CRGB colour) {
  for (int k = 0; k < 25; k++) {
    leds[k] = colour;
  }
}

// How many LEDs from the middle the dot sits, for a tilt in degrees (2 = the edge)
int dotOffset(float degrees) {
  int offset = (int)lroundf(degrees / (TILT_LIMIT_DEG / 2));   // one LED per half limit
  if (offset > 2) {
    offset = 2;
  }
  if (offset < -2) {
    offset = -2;
  }
  return offset;
}

void draw() {
  if (chip == NO_IMU) {
    fill(CRGB(0, 0, 0));
    for (int k = 0; k < 5; k++) {
      leds[k * 5 + k] = CRGB(160, 0, 200);          // a purple cross
      leds[k * 5 + (4 - k)] = CRGB(160, 0, 200);
    }
    FastLED.show();
    return;
  }
  if (state == FALLING) {
    fill(CRGB(255, 140, 0));
    FastLED.show();
    return;
  }
  if (state == JOLT) {
    fill(CRGB(255, 255, 255));
    FastLED.show();
    return;
  }
  fill(CRGB(0, 0, 0));
  leds[2 * 5 + 2] = CRGB(40, 40, 40);               // a dim mark in the middle
  // The dot slides towards the low side. The top of the matrix is Val's front
  // (the face is upright when seen from behind), the left of the matrix its left.
  int row = 2 - dotOffset(forwardDeg);              // nose down: the dot moves up (forward)
  int col = 2 - dotOffset(leftDeg);                 // left side down: the dot moves left
  CRGB colour = CRGB(0, 255, 0);
  if (state == TILTED) {
    colour = CRGB(255, 0, 0);
  }
  leds[row * 5 + col] = colour;
  FastLED.show();
}

void printReadings() {
  Serial.printf("x %+.2f  y %+.2f  z %+.2f | forward %+6.1f  left %+6.1f deg | total %.2f g | jolt %.2f | %s\n",
                accel[0], accel[1], accel[2], forwardDeg, leftDeg, totalG, jolt, stateName());
}

// ---------- Setup / loop ----------
void setup() {
  Serial.begin(115200);
  delay(200);
  pinMode(BUTTON_PIN, INPUT);
  FastLED.addLeds<WS2812, MATRIX_PIN, GRB>(leds, 25);
  FastLED.setBrightness(LED_BRIGHTNESS);
  Wire.begin(25, 21, 100000);         // the Matrix's internal bus: IMU and Motion base
  setWheel(90);                       // the wheel starts still
  Serial.println("IMU test");
  beginImu();
  notLevelMs = millis();
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    zeroPending = true;               // "this is level"
  }

  if (millis() - lastReadMs >= 10) {  // read 100 times a second
    lastReadMs = millis();
    if (readImu()) {
      think();
    }
    updateWheel();
    draw();
  }

  if (millis() - lastPrintMs >= 100) {
    lastPrintMs = millis();
    if (chip != NO_IMU) {
      printReadings();
    }
  }
}
