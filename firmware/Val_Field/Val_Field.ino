/*
  Val_Field - a two-wheeled robot steered by a potential field, with Val's face.

  The second control style for Val (the first is Val_Vehicle's Braitenberg wires).
  Instead of wires to one motor, every sensor pushes or pulls the robot in the
  direction it points, and the sum of those pushes and pulls sets both wheels.
  This is the Val version of PAW-Robotics' CogPotentialField.

  Hardware: M5 Atom Matrix on an Atom Motion base
            two continuous-rotation servos: S1 = LEFT wheel, S2 = RIGHT wheel
            up to 3 sensors, any mix of:
              M5 Color Unit (TCS3472)  light: brighter = stronger
              M5 ToF Unit   (VL53L0X)  distance: nearer = stronger
            in Port A, B and C, or on an M5 PaHub (PCA9548A) plugged into one
            of the ports. On a hub, two sensors of the same kind can share one
            port (for example two distance sensors pointing left and right).
            And, if switched on below, in ports where no I2C sensor (or hub) answers,
            if there is room:
              M5 Reflective IR Unit or M5 Light Unit   Port A only (a level, on G32)
              M5 Key Unit              Port C (off by default: a key is not a field)
            These can't say what they are, so Val only uses them where PORT_A_UNIT /
            ASSUME_KEY_ON_PORT_C say; switch one "off" (x) in setup if it isn't there.
            Real I2C sensors always come first.
            The usual layout: Color or ToF in Port B (or on a PaHub), IR or Light
            Unit in Port A.

  Keep all these files together in a folder named Val_Field. The shared ones
  (Face, Spikes, Static, Canvas, Palette, Icons, MotionServo, Port, I2CBus, Sensor,
  ColorSensor, DistanceSensor, ReflectiveIRSensor, LightUnitSensor, KeySensor, Button)
  are copies of Val/shared/ - edit them there and run sync_shared.py.

  What it does
  - Val says "Hello, my name is, Val".
  - Setup runs the first time, or when you hold the button for 2 seconds:
      wheels (check each turns forward), then for each sensor:
      pull / push / off, which way it points, and its low/high calibration.
  - A Color Unit is one sensor with up to four wires: light (as before), and
    magenta, yellow and cyan (each starts off). They share where it points and its
    low and high points, and each pulls or pushes on its own: approach one colour,
    avoid another. Pointed at the floor, close up, it tastes the ground under it.
    Only 3 wires can be on at once (the display's lanes).
  - A newly plugged sensor (or a different kind on a port): Val looks surprised,
    then only that sensor is set up.
  - A PaHub is found the same way as a sensor: if one answers on a port, its six
    channels are searched instead of the port itself.
  - Then the robot drives: each sensor adds a vector, the vectors are summed,
    and the sum sets the two wheel speeds.
  - The face is for talking to people: hello, setup, surprise at a new sensor.
    While the wheels turn, the matrix shows spikes instead: one lane per sensor
    (green = pull, red = push), placed left to right by where the sensor points,
    and the two wheels firing along the bottom row (left two LEDs = left wheel,
    right two = right wheel; white = forwards, amber = backwards).
  - When both wheels have been still for a moment, the Neutral face comes back.
    Then Unsure after 30 s with no activity, Asleep after 60 s.

  Libraries needed: FastLED, and VL53L0X by Pololu.
  Readings print on Serial at 115200 baud.
*/

#include <Wire.h>
#include <FastLED.h>
#include <Preferences.h>
#include "Face.h"
#include "Spikes.h"
#include "Button.h"
#include "MotionServo.h"
#include "ColorSensor.h"
#include "DistanceSensor.h"
#include "ReflectiveIRSensor.h"
#include "LightUnitSensor.h"
#include "KeySensor.h"
#include "Palette.h"
#include "I2CBus.h"
#include "PotentialField.h"
#include "FieldSetup.h"

// ---------- Settings ----------
const bool SAVE_SETUP = false;        // true: remember wheels, sensors and calibration after power-off
const uint8_t LED_BRIGHTNESS = 20;    // keep low: the matrix is very bright and gets warm

const float BASE_SPEED   = 0.3f;      // cruising speed with nothing in the field (0..1)
const float FORWARD_GAIN = 1.0f;      // how strongly the field speeds up / slows down
const float TURN_GAIN    = 1.0f;      // how strongly the field steers

// The Color Unit's colours. Each colour's hue round the colour wheel (red 0, yellow 60,
// green 120, cyan 180, blue 240, magenta 300), and how many degrees away still count,
// a little, as that colour.
#ifndef MAGENTA_HUE
#define MAGENTA_HUE 300.0f
#endif
#ifndef YELLOW_HUE
#define YELLOW_HUE 60.0f
#endif
#ifndef CYAN_HUE
#define CYAN_HUE 180.0f
#endif
#ifndef HUE_WIDTH
#define HUE_WIDTH 90.0f
#endif

const unsigned long UNSURE_AFTER_MS = 30000;   // no activity for this long -> Unsure
const unsigned long ASLEEP_AFTER_MS = 60000;   // ... and this long in total -> Asleep
const unsigned long SURPRISE_MS     = 3300;    // how long Val looks surprised at a new sensor
const unsigned long REST_AFTER_MS   = 1500;    // wheels still this long -> the run is over, face comes back
const float MOVING_SPEED = 0.05f;              // below this a wheel counts as still

// Units that can't say what they are (not I2C): say here which one is plugged in.
// Change, then upload again. Val still uses an I2C sensor (or hub) if one answers there.
#define NO_UNIT    0
#define IR_UNIT    1                  // M5 Reflective IR Unit
#define LIGHT_UNIT 2                  // M5 Light Unit
#ifndef PORT_A_UNIT
#define PORT_A_UNIT NO_UNIT           // NO_UNIT, IR_UNIT or LIGHT_UNIT: read as a level on G32
#endif
#ifndef ASSUME_KEY_ON_PORT_C
#define ASSUME_KEY_ON_PORT_C 0        // 1 = an M5 Key Unit in Port C
#endif

// ---------- Hardware ----------
CRGB leds[25];
Face face(leds);
Spikes spikes(leds);                  // shown instead of the face while the wheels turn
Button button(39);                    // the Matrix screen is the button
MotionServo leftWheel(1);             // S1
MotionServo rightWheel(2);            // S2

// ---------- Places a sensor can be, and the sensors that can go in them ----------
// Places 0-2 are Ports A, B and C. Places 3-8 are the six channels of a PaHub,
// if one is plugged into a port (that port then holds the hub, not a sensor).
// Each place can hold either kind of sensor; whichever answers is used.
// To stop using a port, set its entry in USE_PORT to false.
const int PORT_COUNT = 3;
const bool USE_PORT[PORT_COUNT] = {true, true, true};
const Port PORTS[PORT_COUNT] = {PORT_A, PORT_B, PORT_C};
const int FIRST_HUB_PLACE = PORT_COUNT;
const int PLACE_COUNT = PORT_COUNT + I2CBus::HUB_CHANNELS;
const int MAX_SENSORS = 3;            // the setup screens have room for 3 sensors

// The hub's channels get their pins when the hub is found. They are named by
// channel number, 0 to 5.
Port place[PLACE_COUNT] = {
  PORT_A, PORT_B, PORT_C,
  {"Hub 0", -1, -1, 0}, {"Hub 1", -1, -1, 1}, {"Hub 2", -1, -1, 2},
  {"Hub 3", -1, -1, 3}, {"Hub 4", -1, -1, 4}, {"Hub 5", -1, -1, 5}
};
int hubPort = -1;                     // which port the PaHub is in (-1 = no hub)

ColorSensor colorSensor[PLACE_COUNT] = {
  ColorSensor(place[0]), ColorSensor(place[1]), ColorSensor(place[2]),
  ColorSensor(place[3]), ColorSensor(place[4]), ColorSensor(place[5]),
  ColorSensor(place[6]), ColorSensor(place[7]), ColorSensor(place[8])
};
DistanceSensor distanceSensor[PLACE_COUNT] = {
  DistanceSensor(place[0]), DistanceSensor(place[1]), DistanceSensor(place[2]),
  DistanceSensor(place[3]), DistanceSensor(place[4]), DistanceSensor(place[5]),
  DistanceSensor(place[6]), DistanceSensor(place[7]), DistanceSensor(place[8])
};

ReflectiveIRSensor reflectiveIR(PORT_A);   // in Port A if PORT_A_UNIT says so
LightUnitSensor lightUnit(PORT_A);         // in Port A if PORT_A_UNIT says so
KeySensor key(PORT_C);                     // assumed in Port C when nothing else answers
const int PORT_A_INDEX = 0;
const int PORT_C_INDEX = 2;

// The field. field.source[i] belongs to place i. A Color Unit in place i also has
// three colour wires (magenta, yellow, cyan): field.source[colourSlot(i, k)].
PotentialField field;
const int COLOURS = PotentialField::COLOURS_PER_PLACE;
int colourSlot(int i, int k) {
  return PLACE_COUNT + i * COLOURS + k;
}

// What each place was last set up with. A sensor of a different kind counts as new.
struct PortRecord {
  char type = 0;                      // 0 = never set up, 'C' = light, 'D' = distance
  uint8_t policy = PotentialField::PULL;
  float angle = 0;
  long low = 0;
  long high = 0;
  uint8_t colourPolicy[PotentialField::COLOURS_PER_PLACE] = {PotentialField::OFF, PotentialField::OFF, PotentialField::OFF};
};
PortRecord record[PLACE_COUNT];

FieldSetup setupMode(leds, leftWheel, rightWheel);
Preferences saved;

unsigned long lastActivityMs = 0;
unsigned long lastSearchMs = 0;
unsigned long lastPrintMs = 0;
bool greetingDone = false;
bool surprisePending = false;         // a new sensor was found; setup starts after the surprise
unsigned long surpriseStartMs = 0;
Face::Expression lastPrinted = Face::TALKING;
bool showingSpikes = false;           // the matrix shows spikes, not the face
bool printLanes = false;              // say which sensor is in which lane, once
unsigned long lastMovingMs = 0;

// Functions defined further down
void saveSetup();
void printSetup();

// =====================================================================
//  Finding sensors
// =====================================================================
// A new sensor's starting setup: distance, reflective IR and the key push (avoid
// things); light (Color Unit or Light Unit) pulls (seek light); all pointing straight ahead.
void setDefaults(int i) {
  PotentialField::Source& s = field.source[i];
  if (s.sensor->typeCode() == 'C' || s.sensor->typeCode() == 'U') {
    s.policy = PotentialField::PULL;
  } else {
    s.policy = PotentialField::PUSH;
  }
  s.angle = 0;
}

bool isPlugged(int i);
bool isSetUp(int i);

// A Color Unit in place i: its colour wires get its sensor, angle and on/off state.
// Anything else (or nothing) in place i: its colour wires are empty.
void syncColourWires(int i) {
  PotentialField::Source& main = field.source[i];
  bool colourUnit = main.sensor != nullptr && main.sensor->typeCode() == 'C';
  for (int k = 0; k < COLOURS; k++) {
    PotentialField::Source& c = field.source[colourSlot(i, k)];
    c.channel = k + 1;
    if (colourUnit) {
      c.sensor = main.sensor;
    } else {
      c.sensor = nullptr;
    }
    c.angle = main.angle;
    c.enabled = colourUnit && main.enabled;
  }
}

// Is there a PaHub in a port? Look in each port (both wire orders). When one is
// found, its six channels become places to look for sensors.
void findHub() {
  for (int p = 0; p < PORT_COUNT; p++) {
    if (!USE_PORT[p]) {
      continue;
    }
    const int orders[2][2] = {{PORTS[p].pin1, PORTS[p].pin2}, {PORTS[p].pin2, PORTS[p].pin1}};
    for (int n = 0; n < 2; n++) {
      I2CBus::use(orders[n][0], orders[n][1]);
      if (!I2CBus::hubAnswers()) {
        continue;
      }
      I2CBus::hubAllOff();
      hubPort = p;
      field.source[p].sensor = nullptr;          // the port holds the hub now
      for (int i = FIRST_HUB_PLACE; i < PLACE_COUNT; i++) {
        place[i].pin1 = PORTS[p].pin1;
        place[i].pin2 = PORTS[p].pin2;
        colorSensor[i].setPort(place[i]);
        distanceSensor[i].setPort(place[i]);
      }
      Serial.printf("PaHub found on %s\n", PORTS[p].name);
      return;
    }
  }
}

// Is the hub still there? (Unplugged: its port becomes an ordinary port again.)
void checkHub() {
  if (hubPort < 0) {
    return;
  }
  const int orders[2][2] = {{PORTS[hubPort].pin1, PORTS[hubPort].pin2}, {PORTS[hubPort].pin2, PORTS[hubPort].pin1}};
  for (int n = 0; n < 2; n++) {
    I2CBus::use(orders[n][0], orders[n][1]);
    if (I2CBus::hubAnswers()) {
      return;
    }
  }
  Serial.printf("PaHub on %s is gone\n", PORTS[hubPort].name);
  hubPort = -1;
}

int sensorsInUse() {
  int count = 0;
  for (int i = 0; i < PLACE_COUNT; i++) {
    if (isPlugged(i)) {
      count++;
    }
  }
  return count;
}

// Look in place i for a sensor of either kind. Returns true if one was found.
// If the place was set up before with the same kind of sensor, its setup comes back.
// allowAssumed: if nothing answers in Port B or C, assume the reflective IR / key there.
bool detectPort(int i, bool allowAssumed) {
  PotentialField::Source& s = field.source[i];
  s.sensor = nullptr;
  if (i < PORT_COUNT && (!USE_PORT[i] || i == hubPort)) {
    return false;                      // port not used, or it holds the hub
  }
  if (i >= FIRST_HUB_PLACE && hubPort < 0) {
    return false;                      // no hub, so no hub channels
  }
  if (sensorsInUse() >= MAX_SENSORS) {
    return false;                      // no room on the setup screens for another
  }
  if (colorSensor[i].begin()) {
    s.sensor = &colorSensor[i];
  } else if (distanceSensor[i].begin()) {
    s.sensor = &distanceSensor[i];
  } else if (allowAssumed && i == PORT_A_INDEX && PORT_A_UNIT == IR_UNIT && reflectiveIR.begin()) {
    s.sensor = &reflectiveIR;          // nothing answered: the reflective IR the settings name
  } else if (allowAssumed && i == PORT_A_INDEX && PORT_A_UNIT == LIGHT_UNIT && lightUnit.begin()) {
    s.sensor = &lightUnit;             // nothing answered: the Light Unit the settings name
  } else if (allowAssumed && i == PORT_C_INDEX && ASSUME_KEY_ON_PORT_C && key.begin()) {
    s.sensor = &key;                   // nothing answered: assume the key
  } else {
    return false;
  }
  bool known = (record[i].type == s.sensor->typeCode());
  if (known) {
    s.policy = (PotentialField::Policy)record[i].policy;
    s.angle = record[i].angle;
    s.sensor->setCalibration(record[i].low, record[i].high);
  } else {
    setDefaults(i);
  }
  // a Color Unit's colour wires: as they were, or off
  for (int k = 0; k < COLOURS; k++) {
    PotentialField::Source& c = field.source[colourSlot(i, k)];
    if (known) {
      c.policy = (PotentialField::Policy)record[i].colourPolicy[k];
    } else {
      c.policy = PotentialField::OFF;
    }
  }
  syncColourWires(i);
  return true;
}

bool isPlugged(int i) {
  return field.source[i].sensor != nullptr && field.source[i].sensor->isConnected();
}

bool isSetUp(int i) {
  return isPlugged(i) && record[i].type == field.source[i].sensor->typeCode();
}

bool anySensorSetUp() {
  for (int i = 0; i < PLACE_COUNT; i++) {
    if (isSetUp(i)) {
      return true;
    }
  }
  return false;
}

bool anySensorNeedsSetup() {
  for (int i = 0; i < PLACE_COUNT; i++) {
    if (isPlugged(i) && !isSetUp(i)) {
      return true;
    }
  }
  return false;
}

// Is place i holding a unit Val only assumed (and might really be empty)?
// A key that has been pressed has proved it is there.
bool isAssumedUnit(int i) {
  if (field.source[i].sensor == &reflectiveIR || field.source[i].sensor == &lightUnit) {
    return true;
  }
  return field.source[i].sensor == &key && !key.pinsKnown();
}

// Look everywhere: real I2C sensors first (ports and hub), then, if there is still
// room, the units that can only be assumed. An assumed unit gives way to an I2C
// sensor if one turns up in its port.
void detectAll() {
  for (int i = 0; i < PLACE_COUNT; i++) {
    if (isAssumedUnit(i)) {
      Sensor* assumed = field.source[i].sensor;
      if (!detectPort(i, false)) {
        field.source[i].sensor = assumed;        // still nothing else: keep the assumed unit
        if (assumed == &reflectiveIR) {
          reflectiveIR.reclaimPins();            // the search borrowed the pins: give them back
        } else if (assumed == &lightUnit) {
          lightUnit.reclaimPins();
        } else {
          key.reclaimPins();
        }
      }
    } else if (!isPlugged(i)) {
      detectPort(i, false);
    }
  }
  for (int i = 0; i < PLACE_COUNT; i++) {
    if (!isPlugged(i)) {
      detectPort(i, true);
    }
  }
}

// Every 5 seconds, look for a hub, and in places that have nothing working in them
void searchForSensors() {
  if (millis() - lastSearchMs < 5000) {
    return;
  }
  lastSearchMs = millis();
  checkHub();
  if (hubPort < 0) {
    findHub();
  }
  detectAll();
}

// The key's LED shows its policy: green pull, red push, off when off
void updateKeyLed() {
  PotentialField::Source& s = field.source[PORT_C_INDEX];
  if (s.sensor != &key) {
    return;
  }
  if (s.policy == PotentialField::PULL) {
    key.showColour(&Palette::GREEN);
  } else if (s.policy == PotentialField::PUSH) {
    key.showColour(&Palette::RED);
  } else {
    key.showColour(nullptr);
  }
}

// =====================================================================
//  Setup screens
// =====================================================================
// full = true:  wheels, then every plugged-in sensor
// full = false: only sensors that have not been set up (new ones)
void startSetup(bool full) {
  surprisePending = false;
  showingSpikes = false;               // setup owns the matrix now
  PotentialField::Source* toSetUp[FieldSetup::MAX_ENTRIES];
  int count = 0;
  int sensors = 0;                     // how many sensors (a Color Unit is one, with up to 4 wires)
  int othersOn = 0;                    // wires already on that are not being set up
  for (int i = 0; i < PLACE_COUNT; i++) {
    if (!isPlugged(i)) {
      continue;
    }
    syncColourWires(i);
    bool colourUnit = (field.source[i].sensor->typeCode() == 'C');
    if (full || !isSetUp(i)) {
      if (count < FieldSetup::MAX_ENTRIES) {
        toSetUp[count] = &field.source[i];
        count++;
      }
      sensors++;
      // a Color Unit's colour wires come straight after it
      if (colourUnit) {
        for (int k = 0; k < COLOURS && count < FieldSetup::MAX_ENTRIES; k++) {
          toSetUp[count] = &field.source[colourSlot(i, k)];
          count++;
        }
      }
    } else {
      if (field.source[i].policy != PotentialField::OFF) {
        othersOn++;
      }
      if (colourUnit) {
        for (int k = 0; k < COLOURS; k++) {
          if (field.source[colourSlot(i, k)].policy != PotentialField::OFF) {
            othersOn++;
          }
        }
      }
    }
  }
  if (full) {
    Serial.printf("Full setup: %d sensor(s)\n", sensors);
    if (sensors == 0) {
      Serial.println("No sensor found. Is it plugged in? A Light Unit, Reflective IR or Key can't say what it is: switch it on with PORT_A_UNIT or ASSUME_KEY_ON_PORT_C at the top of this sketch (Light Unit and Reflective IR: Port A only).");
    }
  } else {
    Serial.printf("Setup for %d new sensor(s)\n", sensors);
  }
  setupMode.start(toSetUp, count, full, othersOn);
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
  for (int i = 0; i < PLACE_COUNT; i++) {
    syncColourWires(i);                // colour wires take the angle just chosen
    if (isPlugged(i)) {
      PotentialField::Source& s = field.source[i];
      record[i].type = s.sensor->typeCode();
      record[i].policy = s.policy;
      record[i].angle = s.angle;
      record[i].low = s.sensor->lowPoint();
      record[i].high = s.sensor->highPoint();
      for (int k = 0; k < COLOURS; k++) {
        record[i].colourPolicy[k] = field.source[colourSlot(i, k)].policy;
      }
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
  for (int i = 0; i < PLACE_COUNT; i++) {
    if (!isPlugged(i)) {
      continue;
    }
    PotentialField::Source& s = field.source[i];
    Serial.printf("  %s: %s, %s at %+.0f deg, low %ld, high %ld\n", s.sensor->portName(),
                  s.sensor->typeName(), PotentialField::policyName(s.policy), s.angle,
                  s.sensor->lowPoint(), s.sensor->highPoint());
    for (int k = 0; k < COLOURS; k++) {
      PotentialField::Source& c = field.source[colourSlot(i, k)];
      if (c.sensor != nullptr && c.policy != PotentialField::OFF) {
        Serial.printf("  %s: %s, %s at %+.0f deg\n", c.sensor->portName(),
                      ColorSensor::channelName((ColorSensor::Channel)c.channel),
                      PotentialField::policyName(c.policy), c.angle);
      }
    }
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
  for (int i = 0; i < PLACE_COUNT; i++) {
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
    for (int k = 0; k < COLOURS; k++) {
      snprintf(key, sizeof(key), "c%dk%d", i, k);
      saved.putUChar(key, record[i].colourPolicy[k]);
    }
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
  for (int i = 0; i < PLACE_COUNT; i++) {
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
    for (int k = 0; k < COLOURS; k++) {
      snprintf(key, sizeof(key), "c%dk%d", i, k);
      record[i].colourPolicy[k] = saved.getUChar(key, PotentialField::OFF);
    }
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
  if (face.isTalking() || showingSpikes) {
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

// =====================================================================
//  The matrix: face or spikes
// =====================================================================
// How far to the left a sensor points: +1 = straight left, -1 = straight right
float leftness(float angle) {
  return sinf(angle * PI / 180.0f);
}

// Hand the spikes what every sensor in the field feels, in lanes from left to right
// (by where each sensor points, so the matrix matches the robot when seen from behind)
void feedSpikes(float left, float right) {
  int order[PotentialField::MAX_SOURCES];
  int count = 0;
  for (int i = 0; i < PotentialField::MAX_SOURCES; i++) {
    PotentialField::Source& s = field.source[i];
    if (!s.enabled || s.sensor == nullptr || s.policy == PotentialField::OFF) {
      continue;
    }
    // insert in order: most left-pointing first
    int k = count;
    while (k > 0 && leftness(field.source[order[k - 1]].angle) < leftness(s.angle)) {
      order[k] = order[k - 1];
      k--;
    }
    order[k] = i;
    count++;
  }
  if (count > Spikes::MAX_LANES) {
    count = Spikes::MAX_LANES;
  }
  for (int lane = 0; lane < count; lane++) {
    PotentialField::Source& s = field.source[order[lane]];
    float weight = (s.policy == PotentialField::PULL) ? 1 : -1;   // pull green, push red
    spikes.setLane(lane, s.sensor->valueFor(s.channel), weight);
  }
  if (printLanes) {
    printLanes = false;
    Serial.printf("Lanes, left to right:");
    for (int lane = 0; lane < count; lane++) {
      PotentialField::Source& s = field.source[order[lane]];
      if (s.channel == 0) {
        Serial.printf(" %s", s.sensor->portName());
      } else {
        Serial.printf(" %s %s", s.sensor->portName(), ColorSensor::channelName((ColorSensor::Channel)s.channel));
      }
    }
    Serial.printf("\n");
  }
  spikes.setLanes(count);
  spikes.setWheels(left, right);
}

// Spikes while the wheels turn; the face for everything else
void updateDisplay(float left, float right) {
  bool talkingToPeople = face.isTalking() || surprisePending || !greetingDone;
  bool moving = fabsf(left) > MOVING_SPEED || fabsf(right) > MOVING_SPEED;
  unsigned long now = millis();

  if (!talkingToPeople && moving) {
    lastMovingMs = now;
    if (!showingSpikes) {
      showingSpikes = true;
      spikes.start();
      printLanes = true;
      Serial.println("Matrix: spikes");
    }
  }
  if (showingSpikes && (talkingToPeople || now - lastMovingMs >= REST_AFTER_MS)) {
    showingSpikes = false;
    lastActivityMs = now;
    if (!talkingToPeople) {
      face.tuneIn(Face::NEUTRAL);     // the run is over: static settles into the face
    }
    Serial.println("Matrix: face");
  }

  if (showingSpikes) {
    lastActivityMs = now;              // driving counts as activity
    feedSpikes(left, right);
    spikes.update();
  } else {
    face.update();
  }
}

void printStatus(float left, float right) {
  if (millis() - lastPrintMs < 500) {
    return;
  }
  lastPrintMs = millis();
  for (int i = 0; i < PLACE_COUNT; i++) {
    if (isPlugged(i)) {
      PotentialField::Source& s = field.source[i];
      Serial.printf("%s %c %5ld (%.2f %s %+.0f)  ", s.sensor->portName(), s.sensor->typeCode(),
                    s.sensor->raw(), s.sensor->value(), PotentialField::policyName(s.policy), s.angle);
      for (int k = 0; k < COLOURS; k++) {
        PotentialField::Source& c = field.source[colourSlot(i, k)];
        if (c.sensor != nullptr && c.policy != PotentialField::OFF) {
          Serial.printf("%s (%.2f %s)  ", ColorSensor::channelName((ColorSensor::Channel)c.channel),
                        c.sensor->valueFor(c.channel), PotentialField::policyName(c.policy));
        }
      }
    }
  }
  Serial.printf("| field (%+.2f, %+.2f) -> L %+.2f R %+.2f | %s\n", field.vx(), field.vy(),
                left, right, showingSpikes ? "Spikes" : Face::name(face.expression()));
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
  ColorSensor::setHues(MAGENTA_HUE, YELLOW_HUE, CYAN_HUE, HUE_WIDTH);
  for (int i = 0; i < PLACE_COUNT; i++) {
    syncColourWires(i);                // the colour wires' channels, empty until a Color Unit turns up
  }

  loadSetup();
  findHub();
  detectAll();
  lastSearchMs = millis();

  face.sayHello();
  lastActivityMs = millis();
}

void loop() {
  // Sensors keep reading all the time, so the setup meter is live
  for (int i = 0; i < PLACE_COUNT; i++) {
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
    for (int i = 0; i < PLACE_COUNT; i++) {
      field.source[i].enabled = isSetUp(i);
      syncColourWires(i);              // a Color Unit's colour wires follow its light wire
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

    updateDisplay(left, right);
    updateIdle();
    printStatus(left, right);

    if (!showingSpikes && face.expression() != lastPrinted) {
      lastPrinted = face.expression();
      Serial.printf("Face: %s\n", Face::name(lastPrinted));
    }
  }

  updateKeyLed();
  leftWheel.update();
  rightWheel.update();
}
