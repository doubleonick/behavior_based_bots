/*
  Val_Field - a two-wheeled robot steered by a potential field, with Val's face.

  The second control style for Val (the first is Val_Vehicle's Braitenberg wires).
  Instead of wires to one motor, every sensor pushes or pulls the robot in the
  direction it points, and the sum of those pushes and pulls sets both wheels.
  This is the Val version of PAW-Robotics' CogPotentialField.

  Hardware: M5 Atom Matrix on an Atom Motion base
            two continuous-rotation servos: S1 = LEFT wheel, S2 = RIGHT wheel
            up to 3 sensors, one each in Port A, B and C, any mix of:
              M5 Color Unit (TCS3472)  light: brighter = stronger
              M5 ToF Unit   (VL53L0X)  distance: nearer = stronger

  Keep all these files together in a folder named Val_Field. The shared ones
  (Face, Static, MotionServo, Port, I2CBus, Sensor, ColorSensor, DistanceSensor, Button)
  are copies of Val/shared/ - edit them there and run sync_shared.py.

  What it does
  - Val says "Hello, my name is, Val".
  - Setup runs the first time, or when you hold the button for 2 seconds:
      wheels (check each turns forward), then for each sensor:
      pull / push / off, which way it points, and its low/high calibration.
  - A newly plugged sensor (or a different kind on a port): Val looks surprised,
    then only that sensor is set up.
  - Then the robot drives: each sensor adds a vector, the vectors are summed,
    and the sum sets the two wheel speeds.
  - The face reacts as before: Unsure after 30 s with no activity, Asleep after 60 s.

  Libraries needed: FastLED, and VL53L0X by Pololu.
  Readings print on Serial at 115200 baud.
*/

#include <Wire.h>
#include <FastLED.h>
#include <Preferences.h>
#include "Face.h"
#include "Button.h"
#include "MotionServo.h"
#include "ColorSensor.h"
#include "DistanceSensor.h"
#include "PotentialField.h"
#include "FieldSetup.h"

// ---------- Settings ----------
const bool SAVE_SETUP = false;        // true: remember wheels, sensors and calibration after power-off
const uint8_t LED_BRIGHTNESS = 20;    // keep low: the matrix is very bright and gets warm

const float BASE_SPEED   = 0.3f;      // cruising speed with nothing in the field (0..1)
const float FORWARD_GAIN = 1.0f;      // how strongly the field speeds up / slows down
const float TURN_GAIN    = 1.0f;      // how strongly the field steers

const unsigned long UNSURE_AFTER_MS = 30000;   // no activity for this long -> Unsure
const unsigned long ASLEEP_AFTER_MS = 60000;   // ... and this long in total -> Asleep
const unsigned long SURPRISE_MS     = 3300;    // how long Val looks surprised at a new sensor

// ---------- Hardware ----------
CRGB leds[25];
Face face(leds);
Button button(39);                    // the Matrix screen is the button
MotionServo leftWheel(1);             // S1
MotionServo rightWheel(2);            // S2

// ---------- Ports and the sensors that can go in them ----------
// Each port can hold either kind of sensor; whichever answers is used.
// To stop using a port, set its entry in USE_PORT to false.
const int PORT_COUNT = 3;
const bool USE_PORT[PORT_COUNT] = {true, true, true};
ColorSensor    colorSensor[PORT_COUNT]    = {ColorSensor(PORT_A), ColorSensor(PORT_B), ColorSensor(PORT_C)};
DistanceSensor distanceSensor[PORT_COUNT] = {DistanceSensor(PORT_A), DistanceSensor(PORT_B), DistanceSensor(PORT_C)};

// The field. field.source[i] belongs to port i.
PotentialField field;

// What each port was last set up with. A sensor of a different kind counts as new.
struct PortRecord {
  char type = 0;                      // 0 = never set up, 'C' = light, 'D' = distance
  uint8_t policy = PotentialField::PULL;
  float angle = 0;
  long low = 0;
  long high = 0;
};
PortRecord record[PORT_COUNT];

FieldSetup setupMode(leds, leftWheel, rightWheel);
Preferences saved;

unsigned long lastActivityMs = 0;
unsigned long lastSearchMs = 0;
unsigned long lastPrintMs = 0;
bool greetingDone = false;
bool surprisePending = false;         // a new sensor was found; setup starts after the surprise
unsigned long surpriseStartMs = 0;
Face::Expression lastPrinted = Face::TALKING;

// Functions defined further down
void saveSetup();
void printSetup();

// =====================================================================
//  Finding sensors
// =====================================================================
// A new sensor's starting setup: distance pushes (avoid things),
// light pulls (seek light), both pointing straight ahead.
void setDefaults(int i) {
  PotentialField::Source& s = field.source[i];
  if (s.sensor->typeCode() == 'D') {
    s.policy = PotentialField::PUSH;
  } else {
    s.policy = PotentialField::PULL;
  }
  s.angle = 0;
}

// Look in port i for a sensor of either kind. Returns true if one was found.
// If the port was set up before with the same kind of sensor, its setup comes back.
bool detectPort(int i) {
  PotentialField::Source& s = field.source[i];
  s.sensor = nullptr;
  if (!USE_PORT[i]) {
    return false;
  }
  if (colorSensor[i].begin()) {
    s.sensor = &colorSensor[i];
  } else if (distanceSensor[i].begin()) {
    s.sensor = &distanceSensor[i];
  } else {
    return false;
  }
  if (record[i].type == s.sensor->typeCode()) {
    s.policy = (PotentialField::Policy)record[i].policy;
    s.angle = record[i].angle;
    s.sensor->setCalibration(record[i].low, record[i].high);
  } else {
    setDefaults(i);
  }
  return true;
}

bool isPlugged(int i) {
  return field.source[i].sensor != nullptr && field.source[i].sensor->isConnected();
}

bool isSetUp(int i) {
  return isPlugged(i) && record[i].type == field.source[i].sensor->typeCode();
}

bool anySensorSetUp() {
  for (int i = 0; i < PORT_COUNT; i++) {
    if (isSetUp(i)) {
      return true;
    }
  }
  return false;
}

bool anySensorNeedsSetup() {
  for (int i = 0; i < PORT_COUNT; i++) {
    if (isPlugged(i) && !isSetUp(i)) {
      return true;
    }
  }
  return false;
}

// Every 5 seconds, look in ports that have nothing working in them
void searchForSensors() {
  if (millis() - lastSearchMs < 5000) {
    return;
  }
  lastSearchMs = millis();
  for (int i = 0; i < PORT_COUNT; i++) {
    if (!isPlugged(i)) {
      detectPort(i);
    }
  }
}

// =====================================================================
//  Setup screens
// =====================================================================
// full = true:  wheels, then every plugged-in sensor
// full = false: only sensors that have not been set up (new ones)
void startSetup(bool full) {
  surprisePending = false;
  PotentialField::Source* toSetUp[PORT_COUNT];
  int count = 0;
  for (int i = 0; i < PORT_COUNT; i++) {
    if (!isPlugged(i)) {
      continue;
    }
    if (full || !isSetUp(i)) {
      toSetUp[count] = &field.source[i];
      count++;
    }
  }
  if (full) {
    Serial.printf("Full setup: %d sensor(s)\n", count);
  } else {
    Serial.printf("Setup for %d new sensor(s)\n", count);
  }
  setupMode.start(toSetUp, count, full);
}

// A new sensor appeared: look surprised first, then set up just that sensor
void reactToNewSensor() {
  surprisePending = true;
  surpriseStartMs = millis();
  lastActivityMs = millis();
  face.surprised();
  Serial.println("New sensor found!");
}

void finishSetup() {
  for (int i = 0; i < PORT_COUNT; i++) {
    if (isPlugged(i)) {
      PotentialField::Source& s = field.source[i];
      record[i].type = s.sensor->typeCode();
      record[i].policy = s.policy;
      record[i].angle = s.angle;
      record[i].low = s.sensor->lowPoint();
      record[i].high = s.sensor->highPoint();
    }
  }
  saveSetup();
  printSetup();
  face.tuneIn(Face::HAPPY);          // static settles into the face
  lastActivityMs = millis();
}

void printSetup() {
  Serial.printf("Wheels: S1 forward = %s, S2 forward = %s\n",
                MotionServo::mountName(leftWheel.mount()), MotionServo::mountName(rightWheel.mount()));
  for (int i = 0; i < PORT_COUNT; i++) {
    if (!isPlugged(i)) {
      continue;
    }
    PotentialField::Source& s = field.source[i];
    Serial.printf("  %s: %s, %s at %+.0f deg, low %ld, high %ld\n", s.sensor->portName(),
                  s.sensor->typeName(), PotentialField::policyName(s.policy), s.angle,
                  s.sensor->lowPoint(), s.sensor->highPoint());
  }
}

// =====================================================================
//  Saving (only when SAVE_SETUP is true)
// =====================================================================
void saveSetup() {
  if (!SAVE_SETUP) {
    return;
  }
  saved.begin("valfield", false);
  saved.putUChar("left", (uint8_t)leftWheel.mount());
  saved.putUChar("right", (uint8_t)rightWheel.mount());
  for (int i = 0; i < PORT_COUNT; i++) {
    if (record[i].type == 0) {
      continue;
    }
    char key[8];
    snprintf(key, sizeof(key), "t%d", i);
    saved.putUChar(key, (uint8_t)record[i].type);
    snprintf(key, sizeof(key), "p%d", i);
    saved.putUChar(key, record[i].policy);
    snprintf(key, sizeof(key), "a%d", i);
    saved.putFloat(key, record[i].angle);
    snprintf(key, sizeof(key), "l%d", i);
    saved.putLong(key, record[i].low);
    snprintf(key, sizeof(key), "h%d", i);
    saved.putLong(key, record[i].high);
  }
  saved.end();
}

void loadSetup() {
  if (!SAVE_SETUP) {
    return;
  }
  saved.begin("valfield", true);
  leftWheel.setMount((MotionServo::Mount)saved.getUChar("left", MotionServo::LEFT));
  rightWheel.setMount((MotionServo::Mount)saved.getUChar("right", MotionServo::RIGHT));
  for (int i = 0; i < PORT_COUNT; i++) {
    char key[8];
    snprintf(key, sizeof(key), "t%d", i);
    if (!saved.isKey(key)) {
      continue;
    }
    record[i].type = (char)saved.getUChar(key, 0);
    snprintf(key, sizeof(key), "p%d", i);
    record[i].policy = saved.getUChar(key, PotentialField::PULL);
    snprintf(key, sizeof(key), "a%d", i);
    record[i].angle = saved.getFloat(key, 0);
    snprintf(key, sizeof(key), "l%d", i);
    record[i].low = saved.getLong(key, 0);
    snprintf(key, sizeof(key), "h%d", i);
    record[i].high = saved.getLong(key, 0);
  }
  saved.end();
}

// =====================================================================
//  Face: activity and idle
// =====================================================================
void noteActivity(const char* source) {
  lastActivityMs = millis();
  if (face.isTalking() || setupMode.active() || surprisePending) {
    return;
  }
  Face::Expression e = face.expression();
  if (e == Face::UNSURE || e == Face::ASLEEP) {
    if (esp_random() & 1) {                      // coin flip
      face.happy();
    } else {
      face.neutral();
    }
    Serial.printf("Activity (%s) -> woke up\n", source);
  }
}

void updateIdle() {
  if (face.isTalking()) {
    lastActivityMs = millis();
    return;
  }
  unsigned long idle = millis() - lastActivityMs;
  Face::Expression e = face.expression();
  if (idle >= ASLEEP_AFTER_MS) {
    if (e != Face::ASLEEP) {
      face.asleep();
    }
  } else if (idle >= UNSURE_AFTER_MS) {
    if (e != Face::UNSURE && e != Face::ASLEEP) {
      face.unsure();
    }
  }
}

void printStatus(float left, float right) {
  if (millis() - lastPrintMs < 500) {
    return;
  }
  lastPrintMs = millis();
  for (int i = 0; i < PORT_COUNT; i++) {
    if (isPlugged(i)) {
      PotentialField::Source& s = field.source[i];
      Serial.printf("%s %c %5ld (%.2f %s %+.0f)  ", s.sensor->portName(), s.sensor->typeCode(),
                    s.sensor->raw(), s.sensor->value(), PotentialField::policyName(s.policy), s.angle);
    }
  }
  Serial.printf("| field (%+.2f, %+.2f) -> L %+.2f R %+.2f | %s\n", field.vx(), field.vy(),
                left, right, Face::name(face.expression()));
}

// =====================================================================
//  Setup / loop
// =====================================================================
void setup() {
  Serial.begin(115200);
  delay(200);
  button.begin();
  FastLED.addLeds<WS2812, 27, GRB>(leds, 25);
  FastLED.setBrightness(LED_BRIGHTNESS);
  FastLED.clear(true);

  leftWheel.begin();                  // finds the Motion base (both servos are on it)
  leftWheel.setMount(MotionServo::LEFT);
  rightWheel.setMount(MotionServo::RIGHT);
  leftWheel.setAllowReverse(true);    // the field can push the robot backwards
  rightWheel.setAllowReverse(true);
  rightWheel.stopNow();

  field.baseSpeed = BASE_SPEED;
  field.forwardGain = FORWARD_GAIN;
  field.turnGain = TURN_GAIN;

  loadSetup();
  for (int i = 0; i < PORT_COUNT; i++) {
    detectPort(i);
  }
  lastSearchMs = millis();

  face.sayHello();
  lastActivityMs = millis();
}

void loop() {
  // Sensors keep reading all the time, so the setup meter is live
  for (int i = 0; i < PORT_COUNT; i++) {
    Sensor* s = field.source[i].sensor;
    if (s != nullptr) {
      s->update();
      if (s->takeChange()) {
        noteActivity(s->typeName());
      }
    }
  }

  Button::Event press = button.update(!setupMode.active() && greetingDone);

  if (setupMode.active()) {
    // ---- Setup screens own the matrix and both wheels ----
    setupMode.handle(press);
    setupMode.update();
    if (setupMode.takeFinished()) {
      finishSetup();
    }
  } else {
    // ---- Normal running ----
    face.update();

    if (!greetingDone && !face.isTalking()) {
      greetingDone = true;
      if (!anySensorSetUp()) {
        startSetup(true);              // first time: full setup
      } else if (anySensorNeedsSetup()) {
        reactToNewSensor();            // saved setup, plus a new sensor
      }
    }

    if (press == Button::HOLD) {
      startSetup(true);
    } else if (press == Button::SHORT_PRESS || press == Button::LONG_PRESS) {
      noteActivity("button");
    }

    if (greetingDone && !setupMode.active()) {
      if (surprisePending) {
        if (millis() - surpriseStartMs >= SURPRISE_MS) {
          startSetup(false);           // just the new sensor
        }
      } else {
        searchForSensors();
        if (anySensorNeedsSetup()) {
          reactToNewSensor();          // a new sensor was plugged in
        }
      }
    }

    // Only sensors that are plugged in AND set up take part in the field
    for (int i = 0; i < PORT_COUNT; i++) {
      field.source[i].enabled = isSetUp(i);
    }

    float left = 0;
    float right = 0;
    if (greetingDone && !setupMode.active()) {
      field.compute();
      field.toWheels(left, right);
    }
    if (!setupMode.active()) {
      leftWheel.drive(left);
      rightWheel.drive(right);
    }

    updateIdle();
    printStatus(left, right);

    if (face.expression() != lastPrinted) {
      lastPrinted = face.expression();
      Serial.printf("Face: %s\n", Face::name(lastPrinted));
    }
  }

  leftWheel.update();
  rightWheel.update();
}
