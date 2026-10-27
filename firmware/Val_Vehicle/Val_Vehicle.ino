/*
  Val_Vehicle - a Braitenberg-style vehicle module with a face.

  Hardware: M5 Atom Matrix on an Atom Motion base
            continuous-rotation servo in S1
            up to 3 sensors, one each in Port A, B and C, any mix of:
              M5 Color Unit (TCS3472)  used as a light sensor
              M5 ToF Unit   (VL53L0X)  used as a distance sensor
            and, if switched on below, in ports where no I2C sensor answers:
              M5 Reflective IR Unit    Port B only (on/off)
              M5 Key Unit              Port C (a button; its LED shows the wire's colour)
            These two can't say what they are, so Val only looks for them if
            ASSUME_IR_ON_PORT_B / ASSUME_KEY_ON_PORT_C is 1. Switch one "off" (x)
            in setup if it isn't there.

  Keep all these files together in a folder named Val_Vehicle:
    Val_Vehicle.ino                        this file: wiring + behaviour
    Face.h / Face.cpp                      Val's face
    Canvas, Palette.h, Icons.h             drawing on the matrix: the picture, colours, setup icons
    ReflectiveIRSensor.h / .cpp            reflective IR (Port B, on/off)
    KeySensor.h / .cpp                     key (Port C)
    Spikes.h / Spikes.cpp                  the wiring's spikes, shown while driving
    Static.h                               white static (into setup, and tuning back in)
    MotionServo.h / MotionServo.cpp        the servo
    Port.h                                 the three Grove ports
    I2CBus.h / I2CBus.cpp                  the sensors' shared I2C bus
    Sensor.h / Sensor.cpp                  parent class for all sensors
    ColorSensor.h / ColorSensor.cpp        light sensor
    DistanceSensor.h / DistanceSensor.cpp  distance sensor
    Connection.h                           one wire from a sensor to the motor
    Button.h                               short press / long press / hold
    SetupMode.h / SetupMode.cpp            the one-button setup screens

  What it does
  - Val says "Hello, my name is, Val".
  - Every sensor that is plugged in is wired to this module's servo.
  - Setup (mount side, then each wire's sign and calibration) runs the first time,
    or any time you hold the button for 2 seconds.
  - Plugging in a new sensor while running (or a different kind of sensor on a port
    that was set up before): Val looks surprised, then only that sensor's wire is
    set up. The other wires are kept.
  - Then the vehicle runs: the servo's speed is the sum of what every wire delivers.
  - The reflective IR cruises at a quarter speed forwards. When it sees something, a
    + wire speeds up to three quarters, a - wire slows past stopping to a quarter
    speed backwards (IR_CRUISE, IR_CHANGE).
  - The key is a bumper: on a + wire Val drives forwards until the key is pressed,
    then backwards; on a - wire, backwards until pressed, then forwards.
  - The face is for talking to people: hello, setup, surprise at a new sensor.
    While the wheel turns, the matrix shows spikes instead: one lane per sensor
    (green +, red -), and the wheel firing along the bottom row.
  - When it becomes clear which "emotion" the run is playing out, Val shows it.
    This module only knows its own wires and what its sensors feel (not whether
    the wires are crossed), so it reads the emotion from how the run goes:
      + wires, feeling it strongly at full speed (charging at it)  -> Angry, 3 s
      - wires, felt it, then it faded while speeding up (moving on) -> Curious, 3 s
      + wires, wheel stopped (it got away, into the dark)           -> Scared
      - wires, wheel stopped (it stays by what it loves)            -> Love
      both kinds, wheel stopped                                     -> Neutral
    The 3 s faces come and go during a run; the others come when the wheel has
    been still for a moment. Then Unsure after 30 s with no activity, Asleep
    after 60 s, Happy or Neutral when something happens.

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
#include "KeySensor.h"
#include "Palette.h"
#include "Connection.h"
#include "SetupMode.h"

// ---------- Settings ----------
const bool SAVE_SETUP = false;        // true: remember mount, wiring and calibration after power-off
const bool ALLOW_REVERSE = false;     // true: a negative sum drives the wheel backwards
const uint8_t LED_BRIGHTNESS = 20;    // keep low: the matrix is very bright and gets warm
// The reflective IR wire: a slow cruise, sped up (+) or slowed past stopping (-) by
// the same amount when the IR sees something. 1 = full speed forwards.
const float IR_CRUISE = 0.25f;        // nothing seen: a quarter speed forwards
const float IR_CHANGE = 0.5f;         // seen: + gives 0.25 + 0.5 = 0.75, - gives 0.25 - 0.5 = -0.25

// Which vehicle to build. The ToF and Color Units say what they are, so Val finds them
// in any port by itself. These two units can't, so Val only looks for one if it is
// switched on here (1). Change, then upload again.
//   ToF vehicle:  both 0, ToF Unit in Port A
//   IR vehicle:   ASSUME_IR_ON_PORT_B 1, Reflective IR Unit in Port B
//   Key vehicle:  ASSUME_KEY_ON_PORT_C 1, Key Unit in Port C
// (Both can be 1 at once.) Val still uses an I2C sensor if one answers in that port.
#ifndef ASSUME_IR_ON_PORT_B
#define ASSUME_IR_ON_PORT_B 0         // M5 Reflective IR Unit, on/off on G33
#endif
#ifndef ASSUME_KEY_ON_PORT_C
#define ASSUME_KEY_ON_PORT_C 0        // M5 Key Unit
#endif

const unsigned long UNSURE_AFTER_MS = 30000;   // no activity for this long -> Unsure
const unsigned long ASLEEP_AFTER_MS = 60000;   // ... and this long in total -> Asleep
const unsigned long SURPRISE_MS     = 3300;    // how long Val looks surprised at a new sensor
const unsigned long REST_AFTER_MS   = 1500;    // wheel still this long -> the run is over, face comes back
const float MOVING_SPEED = 0.05f;              // below this the wheel counts as still
const unsigned long MOMENT_MS = 3000;          // how long a face shows during a run
// Charging (Angry): feeling at least this much, wheel at least this fast, for this long
const float CHARGE_FEELING = 0.85f;
const float CHARGE_SPEED   = 0.8f;
const unsigned long CHARGE_MS = 1000;
// Moving on (Curious): felt at least this much, then this much less, while this much faster
const float NOTICE_FEELING = 0.6f;
const float FADE_BY        = 0.3f;
const float SPEED_UP_BY    = 0.2f;

// ---------- Hardware ----------
CRGB leds[25];
Face face(leds);
Spikes spikes(leds);                  // shown instead of the face while the wheel turns
Button button(39);                    // the Matrix screen is the button
MotionServo servo(1);                 // servo in S1

// ---------- Ports and the sensors that can go in them ----------
// Each port can hold either kind of sensor; whichever answers is used.
// To stop using a port, set its entry in USE_PORT to false.
const int PORT_COUNT = 3;
const bool USE_PORT[PORT_COUNT] = {true, true, true};
ColorSensor    colorSensor[PORT_COUNT]    = {ColorSensor(PORT_A), ColorSensor(PORT_B), ColorSensor(PORT_C)};
DistanceSensor distanceSensor[PORT_COUNT] = {DistanceSensor(PORT_A), DistanceSensor(PORT_B), DistanceSensor(PORT_C)};
ReflectiveIRSensor reflectiveIR(PORT_B);   // assumed in Port B when no I2C sensor answers
KeySensor key(PORT_C);                     // assumed in Port C when no I2C sensor answers
const int PORT_B_INDEX = 1;
const int PORT_C_INDEX = 2;

// One wire per port. wires[i].sensor points at whichever sensor is in port i (or nothing).
Connection wires[PORT_COUNT];

// What each port was last set up with. A sensor of a different kind counts as new.
struct PortRecord {
  char type = 0;                      // 0 = never set up, 'C' = light, 'D' = distance
  float weight = 1;
  long low = 0;
  long high = 0;
};
PortRecord record[PORT_COUNT];

// Which kinds of wire this module has (only wires that are set up and active).
// (Up here, before any function: the Arduino IDE writes a declaration for every
// function above the first one, and they need this type to exist already.)
enum Wiring { NO_WIRES, EXCITATORY_ONLY, INHIBITORY_ONLY, MIXED };

SetupMode setupMode(leds, servo);
Preferences saved;

unsigned long lastActivityMs = 0;
unsigned long lastSearchMs = 0;
unsigned long lastPrintMs = 0;
bool greetingDone = false;
bool surprisePending = false;         // a new sensor was found; setup starts after the surprise
unsigned long surpriseStartMs = 0;
Face::Expression lastPrinted = Face::TALKING;
bool showingSpikes = false;           // the matrix shows spikes, not the face
unsigned long lastMovingMs = 0;
bool inMoment = false;                // a face shown for a few seconds during a run
unsigned long momentEndsMs = 0;
// What this run has been like, for reading its emotion
float peakFeeling = 0;                // strongest feeling since it last moved on
float speedAtPeak = 0;
unsigned long chargingSinceMs = 0;    // 0 = not charging
bool canCharge = true;                // false after Angry, until the feeling drops

// Functions defined further down
void saveSetup();
void printWiring();
bool isSetUp(int i);

// =====================================================================
//  The wiring: add up what every wire delivers to the motor
// =====================================================================
// Each wire delivers weight x sensor value:
//   excitatory (+1): more stimulus -> more speed
//   inhibitory (-1): more stimulus -> less speed
// If there are only inhibitory wires, the motor needs something to slow down
// FROM, so it starts at full speed (bias = 1). Otherwise it starts at 0.
// The key and the reflective IR are different: each drives the wheel itself (see below),
// and they don't count for the bias.
float sumWires() {
  bool anyExcitatory = false;
  bool anyInhibitory = false;
  float total = 0;

  for (int i = 0; i < PORT_COUNT; i++) {
    if (!isSetUp(i) || !wires[i].isActive()) {
      continue;                        // a new sensor does nothing until it has been set up
    }
    if (wires[i].sensor == &key) {
      // + wire: forwards (+1) until pressed, then backwards (-1). - wire: the opposite.
      float pressed = wires[i].sensor->value();
      total += wires[i].weight * (1 - 2 * pressed);
      continue;                        // not counted for the bias
    }
    if (wires[i].sensor == &reflectiveIR) {
      // Cruise slowly forwards; seeing something adds (+) or takes away (-) the same amount
      float seen = wires[i].sensor->value();
      total += IR_CRUISE + wires[i].weight * IR_CHANGE * seen;
      continue;                        // not counted for the bias
    }
    total += wires[i].output();
    if (wires[i].weight > 0) {
      anyExcitatory = true;
    }
    if (wires[i].weight < 0) {
      anyInhibitory = true;
    }
  }

  float bias = 0;
  if (anyInhibitory && !anyExcitatory) {
    bias = 1;
  }
  return bias + total;
}

// =====================================================================
//  Finding sensors
// =====================================================================
// Look in port i for a sensor of either kind. Returns true if one was found.
// If the port was set up before with the same kind of sensor, its wiring and
// calibration come back; otherwise it will need setting up.
bool detectPort(int i) {
  wires[i].sensor = nullptr;
  if (!USE_PORT[i]) {
    return false;
  }
  if (colorSensor[i].begin()) {
    wires[i].sensor = &colorSensor[i];
  } else if (distanceSensor[i].begin()) {
    wires[i].sensor = &distanceSensor[i];
  } else if (i == PORT_B_INDEX && ASSUME_IR_ON_PORT_B && reflectiveIR.begin()) {
    wires[i].sensor = &reflectiveIR;    // nothing answered: assume the reflective IR
  } else if (i == PORT_C_INDEX && ASSUME_KEY_ON_PORT_C && key.begin()) {
    wires[i].sensor = &key;             // nothing answered: assume the key
  } else {
    return false;
  }
  if (record[i].type == wires[i].sensor->typeCode()) {
    wires[i].weight = record[i].weight;
    wires[i].sensor->setCalibration(record[i].low, record[i].high);
  } else {
    wires[i].weight = 1;               // new here: starts as + until set up
  }
  return true;
}

bool isPlugged(int i) {
  return wires[i].sensor != nullptr && wires[i].sensor->isConnected();
}

bool isSetUp(int i) {
  return isPlugged(i) && record[i].type == wires[i].sensor->typeCode();
}

bool anyWireSetUp() {
  for (int i = 0; i < PORT_COUNT; i++) {
    if (isSetUp(i)) {
      return true;
    }
  }
  return false;
}

bool anyWireNeedsSetup() {
  for (int i = 0; i < PORT_COUNT; i++) {
    if (isPlugged(i) && !isSetUp(i)) {
      return true;
    }
  }
  return false;
}

// Is port i holding a unit Val only assumed (and might really be empty)?
// A key that has been pressed has proved it is there.
bool isAssumedUnit(int i) {
  if (wires[i].sensor == &reflectiveIR) {
    return true;
  }
  return wires[i].sensor == &key && !key.pinsKnown();
}

// An assumed unit's port might get an I2C sensor plugged in: if one answers, use it
void lookForI2CInstead(int i) {
  Sensor* found = nullptr;
  if (colorSensor[i].begin()) {
    found = &colorSensor[i];
  } else if (distanceSensor[i].begin()) {
    found = &distanceSensor[i];
  }
  if (found == nullptr) {
    if (wires[i].sensor == &reflectiveIR) {
      reflectiveIR.reclaimPins();       // the search borrowed the pins: give them back
    } else {
      key.reclaimPins();
    }
    return;
  }
  wires[i].sensor = found;
  wires[i].weight = 1;                  // new here: starts as + until set up
  if (record[i].type == found->typeCode()) {
    wires[i].weight = record[i].weight;
    found->setCalibration(record[i].low, record[i].high);
  }
}

// Every 5 seconds, look in ports that have nothing working in them (or only an
// assumed unit, in case an I2C sensor has been plugged in instead)
void searchForSensors() {
  if (millis() - lastSearchMs < 5000) {
    return;
  }
  lastSearchMs = millis();
  for (int i = 0; i < PORT_COUNT; i++) {
    if (!isPlugged(i)) {
      detectPort(i);
    } else if (isAssumedUnit(i)) {
      lookForI2CInstead(i);
    }
  }
}

// Key and reflective IR wires can drive backwards, so the wheel must be allowed to reverse
bool reverseAllowed() {
  if (ALLOW_REVERSE) {
    return true;
  }
  for (int i = 0; i < PORT_COUNT; i++) {
    bool drivesBackwards = (wires[i].sensor == &key || wires[i].sensor == &reflectiveIR);
    if (drivesBackwards && isSetUp(i) && wires[i].isActive()) {
      return true;
    }
  }
  return false;
}

// The key's LED shows its wire: green +, red -, off when not connected
void updateKeyLed() {
  if (wires[PORT_C_INDEX].sensor != &key) {
    return;
  }
  float w = wires[PORT_C_INDEX].weight;
  if (w > 0) {
    key.showColour(&Palette::GREEN);
  } else if (w < 0) {
    key.showColour(&Palette::RED);
  } else {
    key.showColour(nullptr);
  }
}

// =====================================================================
//  Setup screens
// =====================================================================
// full = true:  mount, then every plugged-in wire
// full = false: only the wires that have not been set up (new sensors)
void startSetup(bool full) {
  surprisePending = false;
  showingSpikes = false;               // setup owns the matrix now
  inMoment = false;
  Connection* toSetUp[PORT_COUNT];
  int count = 0;
  for (int i = 0; i < PORT_COUNT; i++) {
    if (!isPlugged(i)) {
      continue;
    }
    if (full || !isSetUp(i)) {
      toSetUp[count] = &wires[i];
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

// A new sensor appeared: look surprised first, then set up just that wire
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
      record[i].type = wires[i].sensor->typeCode();
      record[i].weight = wires[i].weight;
      record[i].low = wires[i].sensor->lowPoint();
      record[i].high = wires[i].sensor->highPoint();
    }
  }
  saveSetup();
  printWiring();
  face.tuneIn(Face::HAPPY);          // static settles into the face
  lastActivityMs = millis();
}

void printWiring() {
  Serial.printf("Mount: %s\n", MotionServo::mountName(servo.mount()));
  for (int i = 0; i < PORT_COUNT; i++) {
    if (!isPlugged(i)) {
      continue;
    }
    Serial.printf("  %s: %s, weight %+.0f, low %ld, high %ld\n", wires[i].sensor->portName(),
                  wires[i].sensor->typeName(), wires[i].weight,
                  wires[i].sensor->lowPoint(), wires[i].sensor->highPoint());
  }
}

// =====================================================================
//  Saving (only when SAVE_SETUP is true)
// =====================================================================
void saveSetup() {
  if (!SAVE_SETUP) {
    return;
  }
  saved.begin("val", false);
  saved.putUChar("mount", (uint8_t)servo.mount());
  for (int i = 0; i < PORT_COUNT; i++) {
    if (record[i].type == 0) {
      continue;
    }
    char key[8];
    snprintf(key, sizeof(key), "t%d", i);
    saved.putUChar(key, (uint8_t)record[i].type);
    snprintf(key, sizeof(key), "w%d", i);
    saved.putFloat(key, record[i].weight);
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
  saved.begin("val", true);
  servo.setMount((MotionServo::Mount)saved.getUChar("mount", MotionServo::LEFT));
  for (int i = 0; i < PORT_COUNT; i++) {
    char key[8];
    snprintf(key, sizeof(key), "t%d", i);
    if (!saved.isKey(key)) {
      continue;
    }
    record[i].type = (char)saved.getUChar(key, 0);
    snprintf(key, sizeof(key), "w%d", i);
    record[i].weight = saved.getFloat(key, 1.0f);
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
  if (face.isTalking() || showingSpikes || inMoment) {
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
// Which kinds of wire this module has
Wiring wiringKind() {
  bool anyExcitatory = false;
  bool anyInhibitory = false;
  for (int i = 0; i < PORT_COUNT; i++) {
    if (!isSetUp(i) || !wires[i].isActive()) {
      continue;
    }
    if (wires[i].weight > 0) {
      anyExcitatory = true;
    }
    if (wires[i].weight < 0) {
      anyInhibitory = true;
    }
  }
  if (anyExcitatory && anyInhibitory) {
    return MIXED;
  }
  if (anyExcitatory) {
    return EXCITATORY_ONLY;
  }
  if (anyInhibitory) {
    return INHIBITORY_ONLY;
  }
  return NO_WIRES;
}

// How strongly this module feels its stimulus right now: its strongest wire, 0..1
float feeling() {
  float strongest = 0;
  for (int i = 0; i < PORT_COUNT; i++) {
    if (!isSetUp(i) || !wires[i].isActive()) {
      continue;
    }
    strongest = max(strongest, wires[i].sensor->value());
  }
  return strongest;
}

// The face that ends a run (when the wheel has been still for a moment).
// One module only knows the signs of its own wires, not whether they are crossed:
//   only + wires, stopped: nothing left to drive it, it got away -> Scared
//     (Vehicle 1 and Vehicle 2a end like this)
//   only - wires, stopped: it slowed right down next to what it feels -> Love
//   both, stopped: nothing to tell -> Neutral
Face::Expression restingFace() {
  Wiring w = wiringKind();
  if (w == EXCITATORY_ONLY) {
    return Face::SCARED;
  }
  if (w == INHIBITORY_ONLY) {
    return Face::LOVE;
  }
  return Face::NEUTRAL;
}

// A new run starts: forget what the last one was like
void startRunMemory(float feel, float speed) {
  peakFeeling = feel;
  speedAtPeak = speed;
  chargingSinceMs = 0;
  canCharge = true;
}

// Is the run showing an emotion right now? Returns the face, or NEUTRAL for "not yet".
//   + wires: feeling it strongly at full speed for a second -> charging at it -> Angry
//   - wires: felt it, then it faded while the wheel sped up  -> moving on -> Curious
Face::Expression emotionDuringRun(float feel, float speed, unsigned long now) {
  Wiring w = wiringKind();
  if (w == EXCITATORY_ONLY) {
    if (feel < 0.5f) {
      canCharge = true;                // it backed off: it can charge again
    }
    if (feel >= CHARGE_FEELING && speed >= CHARGE_SPEED) {
      if (chargingSinceMs == 0) {
        chargingSinceMs = now;
      }
      if (canCharge && now - chargingSinceMs >= CHARGE_MS) {
        canCharge = false;
        return Face::ANGRY;
      }
    } else {
      chargingSinceMs = 0;
    }
  }
  if (w == INHIBITORY_ONLY) {
    if (feel > peakFeeling) {
      peakFeeling = feel;
      speedAtPeak = speed;
    }
    bool fadedAway = feel <= peakFeeling - FADE_BY;
    bool spedUp = speed >= speedAtPeak + SPEED_UP_BY;
    if (peakFeeling >= NOTICE_FEELING && fadedAway && spedUp) {
      peakFeeling = feel;              // start watching for the next thing
      speedAtPeak = speed;
      return Face::CURIOUS;
    }
  }
  return Face::NEUTRAL;
}

// Hand the spikes what every set-up wire feels, and how fast the wheel goes
void feedSpikes(float sum) {
  int lane = 0;
  for (int i = 0; i < PORT_COUNT; i++) {
    if (!isSetUp(i) || !wires[i].isActive()) {
      continue;
    }
    spikes.setLane(lane, wires[i].sensor->value(), wires[i].weight);
    lane++;
  }
  spikes.setLanes(lane);
  float speed = sum;
  if (reverseAllowed() && speed < 0) {
    speed = -speed;                    // backwards still fires the wheel
  }
  spikes.setWheel(speed);
}

// Spikes while the wheel turns; the face for everything else
void updateDisplay(float sum) {
  bool talkingToPeople = face.isTalking() || surprisePending || !greetingDone;
  float speed = sum;
  if (reverseAllowed()) {
    speed = fabsf(sum);
  }
  bool moving = speed > MOVING_SPEED;
  float feel = feeling();
  unsigned long now = millis();

  if (talkingToPeople) {
    if (showingSpikes || inMoment) {
      showingSpikes = false;
      inMoment = false;
      Serial.println("Matrix: face");
    }
    face.update();
    return;
  }

  // A face shown during a run: when its time is up, back to spikes (or, if the
  // wheel has stopped meanwhile, the face that ends the run)
  if (inMoment && now >= momentEndsMs) {
    inMoment = false;
    if (moving) {
      showingSpikes = true;
      spikes.start();
      Serial.println("Matrix: spikes");
    } else {
      face.tuneIn(restingFace());
      Serial.println("Matrix: face");
    }
  }

  if (moving) {
    lastMovingMs = now;
    if (!showingSpikes && !inMoment) {
      showingSpikes = true;            // a new run
      spikes.start();
      startRunMemory(feel, speed);
      Serial.println("Matrix: spikes");
    }
  }

  if (showingSpikes) {
    Face::Expression e = emotionDuringRun(feel, speed, now);
    if (e != Face::NEUTRAL) {
      showingSpikes = false;
      inMoment = true;
      momentEndsMs = now + MOMENT_MS;
      face.tuneIn(e);
      Serial.printf("Matrix: face (%s, during the run)\n", Face::name(e));
    } else if (now - lastMovingMs >= REST_AFTER_MS) {
      showingSpikes = false;           // the run is over: static settles into its face
      face.tuneIn(restingFace());
      Serial.println("Matrix: face");
    }
  }

  if (showingSpikes || inMoment) {
    lastActivityMs = now;              // driving counts as activity
  }
  if (showingSpikes) {
    feedSpikes(sum);
    spikes.update();
  } else {
    face.update();
  }
}

void printStatus(float sum) {
  if (millis() - lastPrintMs < 500) {
    return;
  }
  lastPrintMs = millis();
  for (int i = 0; i < PORT_COUNT; i++) {
    if (isPlugged(i)) {
      Sensor* s = wires[i].sensor;
      Serial.printf("%s %c %5ld (%.2f x %+.0f)  ", s->portName(), s->typeCode(), s->raw(),
                    s->value(), wires[i].weight);
    }
  }
  Serial.printf("| sum %+.2f -> servo %d deg | %s\n", sum, servo.angle(),
                showingSpikes ? "Spikes" : Face::name(face.expression()));
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

  servo.begin();
  servo.setAllowReverse(ALLOW_REVERSE);

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
    if (wires[i].sensor != nullptr) {
      wires[i].sensor->update();
      if (wires[i].sensor->takeChange()) {
        noteActivity(wires[i].sensor->typeName());
      }
    }
  }

  Button::Event press = button.update(!setupMode.active() && greetingDone);

  if (setupMode.active()) {
    // ---- Setup screens own the matrix and the servo ----
    setupMode.handle(press);
    setupMode.update();
    if (setupMode.takeFinished()) {
      finishSetup();
    }
  } else {
    // ---- Normal running ----
    if (!greetingDone && !face.isTalking()) {
      greetingDone = true;
      if (!anyWireSetUp()) {
        startSetup(true);              // first time: full setup
      } else if (anyWireNeedsSetup()) {
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
          startSetup(false);           // just the new sensor's wire
        }
      } else {
        searchForSensors();
        if (anyWireNeedsSetup()) {
          reactToNewSensor();          // a new sensor was plugged in
        }
      }
    }

    float sum = 0;
    if (greetingDone && !setupMode.active()) {
      sum = sumWires();
      servo.setAllowReverse(reverseAllowed());
      servo.drive(sum);
    } else if (!setupMode.active()) {
      servo.drive(0);                  // stay stopped during the greeting
    }

    updateDisplay(sum);
    updateIdle();
    printStatus(sum);

    if (!showingSpikes && face.expression() != lastPrinted) {
      lastPrinted = face.expression();
      Serial.printf("Face: %s\n", Face::name(lastPrinted));
    }
  }

  updateKeyLed();
  servo.update();
}
