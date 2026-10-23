/*
  Val_Brooks - behaviours in a fixed hierarchy (subsumption, after Rodney Brooks).

  Several simple behaviours run all the time. Each one keeps asking "do I want the
  wheels right now?". The highest one in the hierarchy that says yes drives the
  wheels; the ones below it are held back (suppressed) until it lets go.

  Hardware: M5 Atom Matrix on an Atom Motion base
            two continuous-rotation servos: S1 = LEFT wheel, S2 = RIGHT wheel
            an M5 PaHub in one of the ports, with sensors on its channels:
              even channels (0, 2, 4) = the RIGHT side, odd channels (1, 3, 5) = the LEFT side
              M5 ToF Unit (VL53L0X)    distance
              M5 Color Unit (TCS3472)  light
              (for example: right ToF on 0, left ToF on 1, right light on 2, left light on 3)
            for button collisions: a left and a right bump switch (see BUMPER pins below)
            for inertial collisions: the Atom Matrix's own IMU (BMI270, see INERTIAL_COLLISION)

  Pick the hierarchy to test in the TEST HIERARCHY section below, by leaving exactly
  one line uncommented. This sketch is a test bench: it has no setup screens, and its
  settings (thresholds, speeds, pins) are near the top of this file.

  The matrix:
  - Val says "Hello, my name is, Val", then the behaviours start.
  - While running, one row per behaviour (top = top of the hierarchy):
      column 0: red = push / avoid, green = pull / approach (wander: green going
      forwards, red going backwards); column 1: a gap; columns 2-4: the behaviour's
      colour: collision white, distance purple, light yellow, wander blue.
    Rows not in control stay steady. The row in control sends spikes left to right,
    as often as the wheels' average speed says.
  - Short press on the matrix: pause (wheels stop, Neutral face) / carry on.

  Keep all these files together in a folder named Val_Brooks. The shared ones
  (Face, Static, MotionServo, Port, I2CBus, Sensor, ColorSensor, DistanceSensor, Button)
  are copies of firmware/shared/ - edit them there and run sync_shared.py.

  Libraries needed: FastLED, VL53L0X by Pololu, and only for INERTIAL_COLLISION 1:
  "SparkFun BMI270 Arduino Library".
  Readings print on Serial at 115200 baud.
*/

#include <Wire.h>
#include <FastLED.h>
#include "Face.h"
#include "Button.h"
#include "MotionServo.h"
#include "ColorSensor.h"
#include "DistanceSensor.h"
#include "I2CBus.h"
#include "Layers.h"

// =====================================================================
//  TEST HIERARCHY: uncomment exactly ONE line (top of the hierarchy first)
// =====================================================================
// #define TEST_ESCAPE_LEFT      // 1. escape left collision  -> wander
// #define TEST_ESCAPE_RIGHT     // 2. escape right collision -> wander
// #define TEST_AVOID_OBJECT     // 3. avoid object           -> wander
// #define TEST_APPROACH_OBJECT  // 4. approach object        -> wander
// #define TEST_AVOID_LIGHT      // 5. avoid light            -> wander
// #define TEST_APPROACH_LIGHT   // 6. approach light         -> wander
#define TEST_FULL                // escape front collision -> avoid object -> approach light -> wander

// ---------- Collision detection: which kind? ----------
// 0 = bump switches (is_button_collision), 1 = the IMU (is_inertial_collision)
#define INERTIAL_COLLISION 0

// ---------- Settings ----------
const uint8_t LED_BRIGHTNESS = 20;    // keep low: the matrix is very bright and gets warm

// Bump switches: pressed = LOW (they use the pin's pull-up). Port C is G19 / G22.
const int LEFT_BUMPER_PIN  = 19;
const int RIGHT_BUMPER_PIN = 22;

// Distance: an object nearer than this (mm) on either side is "there"
const long DISTANCE_THRESHOLD_MM = 300;
// Light: (left - right) / (left + right) must be at least this, either way, to count
const float LIGHT_GRADIENT_THRESHOLD = 0.25f;
const long  LIGHT_MINIMUM = 50;       // left + right below this: too dark to tell

// IMU (only with INERTIAL_COLLISION 1). A bump is a sudden change in acceleration.
const float IMPACT_G = 0.6f;          // how big a sudden change counts as a bump (in g)
// Which IMU axis points to the robot's front and which to its left, and their signs.
// Depends on how the Atom sits on the robot: check with the Serial readings.
const int   IMU_FORWARD_AXIS = 0;     // 0 = x, 1 = y, 2 = z
const float IMU_FORWARD_SIGN = 1.0f;
const int   IMU_LEFT_AXIS    = 1;
const float IMU_LEFT_SIGN    = 1.0f;

// Speeds (-1..1, positive = forwards)
const float STRAIGHT_SPEED = 0.5f;
const float ARC_FAST = 0.55f;         // the outside wheel of an arc
const float ARC_SLOW = 0.25f;         // the inside wheel of an arc
const float CRUISE_SPEED = 0.4f;      // approach / avoid: speed before steering
const float STEER_GAIN = 0.5f;        // approach / avoid: how hard they steer

// Timing
const unsigned long PRIMITIVE_MS = 200;     // wander rolls the die this often
const unsigned long ESCAPE_BACK_MS = 400;   // escape: back straight up for this long...
const unsigned long ESCAPE_TURN_MS = 600;   // ... then arc backwards, turning away, for this long

#if INERTIAL_COLLISION
#include "SparkFun_BMI270_Arduino_Library.h"
BMI270 imu;
#endif

// ---------- Hardware ----------
CRGB leds[25];
Face face(leds);
Layers layers(leds);
Button button(39);                    // the Matrix screen is the button
MotionServo leftWheel(1);             // S1
MotionServo rightWheel(2);            // S2

// ---------- Sensors on the PaHub ----------
const int PORT_COUNT = 3;
const Port PORTS[PORT_COUNT] = {PORT_A, PORT_B, PORT_C};
const int CHANNELS = I2CBus::HUB_CHANNELS;
Port hubPlace[CHANNELS] = {
  {"Hub 0", -1, -1, 0}, {"Hub 1", -1, -1, 1}, {"Hub 2", -1, -1, 2},
  {"Hub 3", -1, -1, 3}, {"Hub 4", -1, -1, 4}, {"Hub 5", -1, -1, 5}
};
ColorSensor colorSensor[CHANNELS] = {
  ColorSensor(hubPlace[0]), ColorSensor(hubPlace[1]), ColorSensor(hubPlace[2]),
  ColorSensor(hubPlace[3]), ColorSensor(hubPlace[4]), ColorSensor(hubPlace[5])
};
DistanceSensor distanceSensor[CHANNELS] = {
  DistanceSensor(hubPlace[0]), DistanceSensor(hubPlace[1]), DistanceSensor(hubPlace[2]),
  DistanceSensor(hubPlace[3]), DistanceSensor(hubPlace[4]), DistanceSensor(hubPlace[5])
};
int hubPort = -1;                     // which port the PaHub is in (-1 = none found)

// The sensors each behaviour uses (nullptr = not plugged in)
Sensor* leftDistance  = nullptr;
Sensor* rightDistance = nullptr;
Sensor* leftLight     = nullptr;
Sensor* rightLight    = nullptr;

// ---------- The behaviours ----------
enum Behaviour : uint8_t {
  ESCAPE_LEFT_COLLISION, ESCAPE_RIGHT_COLLISION, ESCAPE_FRONT_COLLISION,
  AVOID_OBJECT, APPROACH_OBJECT, AVOID_LIGHT, APPROACH_LIGHT, WANDER
};

#if defined(TEST_ESCAPE_LEFT)
const Behaviour HIERARCHY[] = {ESCAPE_LEFT_COLLISION, WANDER};
#elif defined(TEST_ESCAPE_RIGHT)
const Behaviour HIERARCHY[] = {ESCAPE_RIGHT_COLLISION, WANDER};
#elif defined(TEST_AVOID_OBJECT)
const Behaviour HIERARCHY[] = {AVOID_OBJECT, WANDER};
#elif defined(TEST_APPROACH_OBJECT)
const Behaviour HIERARCHY[] = {APPROACH_OBJECT, WANDER};
#elif defined(TEST_AVOID_LIGHT)
const Behaviour HIERARCHY[] = {AVOID_LIGHT, WANDER};
#elif defined(TEST_APPROACH_LIGHT)
const Behaviour HIERARCHY[] = {APPROACH_LIGHT, WANDER};
#elif defined(TEST_FULL)
const Behaviour HIERARCHY[] = {ESCAPE_FRONT_COLLISION, AVOID_OBJECT, APPROACH_LIGHT, WANDER};
#else
#error "Uncomment one TEST_ line in the TEST HIERARCHY section"
#endif
const int LEVELS = sizeof(HIERARCHY) / sizeof(HIERARCHY[0]);

// ---------- State ----------
float wheelLeft = 0;                  // what the behaviour in control asked for
float wheelRight = 0;
int inControl = -1;                   // index into HIERARCHY
bool paused = false;
bool greetingDone = false;
unsigned long lastPrintMs = 0;

// An escape is a fixed move: once it starts, it runs to the end
struct Escape {
  bool running = false;
  unsigned long startMs = 0;
  bool turnRight = true;              // which way the nose turns while backing away
};
Escape escape;

// Wander: the current roll of the die and when it was rolled
int wanderRoll = 0;                   // 0..5
unsigned long lastRollMs = 0;
bool wanderStarted = false;

// =====================================================================
//  Finding the hub and the sensors on it
// =====================================================================
void findHub() {
  for (int p = 0; p < PORT_COUNT; p++) {
    const int orders[2][2] = {{PORTS[p].pin1, PORTS[p].pin2}, {PORTS[p].pin2, PORTS[p].pin1}};
    for (int n = 0; n < 2; n++) {
      I2CBus::use(orders[n][0], orders[n][1]);
      if (!I2CBus::hubAnswers()) {
        continue;
      }
      I2CBus::hubAllOff();
      hubPort = p;
      for (int ch = 0; ch < CHANNELS; ch++) {
        hubPlace[ch].pin1 = PORTS[p].pin1;
        hubPlace[ch].pin2 = PORTS[p].pin2;
        colorSensor[ch].setPort(hubPlace[ch]);
        distanceSensor[ch].setPort(hubPlace[ch]);
      }
      Serial.printf("PaHub found on %s\n", PORTS[p].name);
      return;
    }
  }
  Serial.println("No PaHub found: distance and light behaviours will never act");
}

// Even channels are the right side, odd channels the left
void findSensors() {
  if (hubPort < 0) {
    return;
  }
  for (int ch = 0; ch < CHANNELS; ch++) {
    bool left = (ch % 2 == 1);
    if (distanceSensor[ch].begin()) {
      if (left && leftDistance == nullptr) {
        leftDistance = &distanceSensor[ch];
      } else if (!left && rightDistance == nullptr) {
        rightDistance = &distanceSensor[ch];
      }
    } else if (colorSensor[ch].begin()) {
      if (left && leftLight == nullptr) {
        leftLight = &colorSensor[ch];
      } else if (!left && rightLight == nullptr) {
        rightLight = &colorSensor[ch];
      }
    }
  }
}

bool present(Sensor* s) {
  return s != nullptr && s->isConnected();
}

void updateSensors() {
  Sensor* all[4] = {leftDistance, rightDistance, leftLight, rightLight};
  for (int k = 0; k < 4; k++) {
    if (all[k] != nullptr) {
      all[k]->update();
    }
  }
}

// =====================================================================
//  Detection: what is out there?
// =====================================================================
bool is_left_bumper_pressed() {
  return digitalRead(LEFT_BUMPER_PIN) == LOW;
}

bool is_right_bumper_pressed() {
  return digitalRead(RIGHT_BUMPER_PIN) == LOW;
}

// A bump switch was pressed (either side)
bool is_button_collision() {
  return is_left_bumper_pressed() || is_right_bumper_pressed();
}

// ---- The IMU: a bump is a sudden change in acceleration ----
float impactForward = 0;              // how big the latest sudden change is, toward the front
float impactLeft = 0;                 // ... and toward the left

#if INERTIAL_COLLISION
float slowAccel[3] = {0, 0, 0};       // acceleration, followed slowly (gravity and steady driving)
bool imuReady = false;
unsigned long lastImuMs = 0;

void beginImu() {
  imuReady = (imu.beginI2C(BMI2_I2C_PRIM_ADDR, Wire) == BMI2_OK);
  Serial.printf("IMU (BMI270) %s\n", imuReady ? "found" : "NOT found");
}

// Read the IMU every 10 ms; a sudden change = this reading minus the slow average
void updateImu() {
  if (!imuReady || millis() - lastImuMs < 10) {
    return;
  }
  lastImuMs = millis();
  if (imu.getSensorData() != BMI2_OK) {
    return;
  }
  float a[3] = {imu.data.accelX, imu.data.accelY, imu.data.accelZ};
  float sudden[3];
  for (int k = 0; k < 3; k++) {
    sudden[k] = a[k] - slowAccel[k];
    slowAccel[k] += 0.05f * (a[k] - slowAccel[k]);
  }
  impactForward = IMU_FORWARD_SIGN * sudden[IMU_FORWARD_AXIS];
  impactLeft = IMU_LEFT_SIGN * sudden[IMU_LEFT_AXIS];
}
#else
void beginImu() {
}

void updateImu() {
}
#endif

// A sudden jolt bigger than IMPACT_G, from any side
bool is_inertial_collision() {
#if INERTIAL_COLLISION
  return sqrtf(impactForward * impactForward + impactLeft * impactLeft) >= IMPACT_G;
#else
  return false;                       // not compiled in: set INERTIAL_COLLISION to 1
#endif
}

// Something hit the left side: the left bumper, or a jolt that pushes the robot right
bool is_left_collision() {
#if INERTIAL_COLLISION
  return is_inertial_collision() && impactLeft < 0;
#else
  return is_left_bumper_pressed();
#endif
}

bool is_right_collision() {
#if INERTIAL_COLLISION
  return is_inertial_collision() && impactLeft > 0;
#else
  return is_right_bumper_pressed();
#endif
}

// Something hit the front: left and right together (bump switches), or any jolt (IMU)
bool is_front_collision() {
#if INERTIAL_COLLISION
  return is_inertial_collision();
#else
  return is_button_collision();
#endif
}

// Is an object nearer than DISTANCE_THRESHOLD_MM on either side?
bool is_distance_threshold() {
  bool leftNear = present(leftDistance) && leftDistance->raw() < DISTANCE_THRESHOLD_MM;
  bool rightNear = present(rightDistance) && rightDistance->raw() < DISTANCE_THRESHOLD_MM;
  return leftNear || rightNear;
}

// How much brighter the left is than the right: -1 (all right) .. +1 (all left)
float light_gradient() {
  if (!present(leftLight) || !present(rightLight)) {
    return 0;
  }
  long total = leftLight->raw() + rightLight->raw();
  if (total < LIGHT_MINIMUM) {
    return 0;                          // too dark to tell
  }
  return (float)(leftLight->raw() - rightLight->raw()) / total;
}

// Is one side clearly brighter than the other?
bool is_light_gradient_threshold() {
  return fabsf(light_gradient()) >= LIGHT_GRADIENT_THRESHOLD;
}

// How near something is on each side: 0 = at the threshold or further, 1 = touching
float nearness(Sensor* s) {
  if (!present(s)) {
    return 0;
  }
  float n = 1.0f - (float)s->raw() / DISTANCE_THRESHOLD_MM;
  return constrain(n, 0.0f, 1.0f);
}

// =====================================================================
//  Primitives: set the two wheels
// =====================================================================
void set_wheels(float left, float right) {
  wheelLeft = left;
  wheelRight = right;
}

void straight_forward()   { set_wheels(STRAIGHT_SPEED, STRAIGHT_SPEED); }
void straight_backward()  { set_wheels(-STRAIGHT_SPEED, -STRAIGHT_SPEED); }
void arc_forward_left()   { set_wheels(ARC_SLOW, ARC_FAST); }      // the left wheel is on the inside
void arc_forward_right()  { set_wheels(ARC_FAST, ARC_SLOW); }
void arc_backward_left()  { set_wheels(-ARC_SLOW, -ARC_FAST); }    // the back swings left, the nose turns right
void arc_backward_right() { set_wheels(-ARC_FAST, -ARC_SLOW); }    // the back swings right, the nose turns left

// =====================================================================
//  Behaviours: what each one does when it has the wheels
// =====================================================================
// Back straight up, then arc backwards so the nose turns away from the hit
void start_escape(bool turnRight) {
  escape.running = true;
  escape.startMs = millis();
  escape.turnRight = turnRight;
}

// Is an escape still under way? (It lets go of the wheels once its time is up.)
bool escape_still_running() {
  if (escape.running && millis() - escape.startMs >= ESCAPE_BACK_MS + ESCAPE_TURN_MS) {
    escape.running = false;
  }
  return escape.running;
}

void run_escape() {
  unsigned long t = millis() - escape.startMs;
  if (t < ESCAPE_BACK_MS) {
    straight_backward();
  } else if (escape.turnRight) {
    arc_backward_left();               // nose turns right
  } else {
    arc_backward_right();              // nose turns left
  }
}

void escape_left_collision() {
  if (!escape.running) {
    start_escape(true);                // hit on the left: turn right
  }
  run_escape();
}

void escape_right_collision() {
  if (!escape.running) {
    start_escape(false);               // hit on the right: turn left
  }
  run_escape();
}

// Left and right combined: turn away from the side that was hit (a coin flip if unclear)
void escape_front_collision() {
  if (!escape.running) {
    bool left = is_left_collision();
    bool right = is_right_collision();
    bool turnRight;
    if (left && !right) {
      turnRight = true;
    } else if (right && !left) {
      turnRight = false;
    } else {
      turnRight = (esp_random() & 1);
    }
    start_escape(turnRight);
  }
  run_escape();
}

// Steer toward (approach) or away from (avoid) the nearer side
void steer_by_distance(float direction) {
  float turn = STEER_GAIN * (nearness(leftDistance) - nearness(rightDistance));   // + = nearer on the left
  set_wheels(CRUISE_SPEED + direction * turn, CRUISE_SPEED - direction * turn);
}

void avoid_object()    { steer_by_distance(+1); }   // nearer on the left: left wheel faster, turn right
void approach_object() { steer_by_distance(-1); }

// Steer toward (approach) or away from (avoid) the brighter side
void steer_by_light(float direction) {
  float turn = STEER_GAIN * light_gradient();        // + = brighter on the left
  set_wheels(CRUISE_SPEED + direction * turn, CRUISE_SPEED - direction * turn);
}

void avoid_light()    { steer_by_light(+1); }       // brighter on the left: turn right
void approach_light() { steer_by_light(-1); }

// Roll a die every PRIMITIVE_MS for one of six moves (no turning on the spot)
void roll_wander() {
  wanderRoll = esp_random() % 6;
  lastRollMs = millis();
}

bool wander_goes_forward() {
  return wanderRoll == 0 || wanderRoll == 2 || wanderRoll == 3;
}

void wander() {
  switch (wanderRoll) {
    case 0: straight_forward();   break;
    case 1: straight_backward();  break;
    case 2: arc_forward_left();   break;
    case 3: arc_forward_right();  break;
    case 4: arc_backward_left();  break;
    case 5: arc_backward_right(); break;
  }
}

// =====================================================================
//  The hierarchy
// =====================================================================
const char* behaviourName(Behaviour b) {
  switch (b) {
    case ESCAPE_LEFT_COLLISION:  return "Escape left collision";
    case ESCAPE_RIGHT_COLLISION: return "Escape right collision";
    case ESCAPE_FRONT_COLLISION: return "Escape front collision";
    case AVOID_OBJECT:           return "Avoid object";
    case APPROACH_OBJECT:        return "Approach object";
    case AVOID_LIGHT:            return "Avoid light";
    case APPROACH_LIGHT:         return "Approach light";
    case WANDER:                 return "Wander";
  }
  return "?";
}

// Does this behaviour want the wheels right now?
bool wantsControl(Behaviour b) {
  switch (b) {
    case ESCAPE_LEFT_COLLISION:  return escape_still_running() || is_left_collision();
    case ESCAPE_RIGHT_COLLISION: return escape_still_running() || is_right_collision();
    case ESCAPE_FRONT_COLLISION: return escape_still_running() || is_front_collision();
    case AVOID_OBJECT:           return is_distance_threshold();
    case APPROACH_OBJECT:        return is_distance_threshold();
    case AVOID_LIGHT:            return is_light_gradient_threshold();
    case APPROACH_LIGHT:         return is_light_gradient_threshold();
    case WANDER:                 return true;       // always: the bottom of the hierarchy
  }
  return false;
}

void act(Behaviour b) {
  switch (b) {
    case ESCAPE_LEFT_COLLISION:  escape_left_collision();  break;
    case ESCAPE_RIGHT_COLLISION: escape_right_collision(); break;
    case ESCAPE_FRONT_COLLISION: escape_front_collision(); break;
    case AVOID_OBJECT:           avoid_object();           break;
    case APPROACH_OBJECT:        approach_object();        break;
    case AVOID_LIGHT:            avoid_light();            break;
    case APPROACH_LIGHT:         approach_light();         break;
    case WANDER:                 wander();                 break;
  }
}

// The first behaviour from the top that wants control gets it
void arbitrate() {
  // Wander keeps rolling its die underneath, whoever is in control
  if (!wanderStarted || millis() - lastRollMs >= PRIMITIVE_MS) {
    wanderStarted = true;
    roll_wander();
  }
  int winner = LEVELS - 1;
  for (int level = 0; level < LEVELS; level++) {
    if (wantsControl(HIERARCHY[level])) {
      winner = level;
      break;
    }
  }
  if (winner != inControl) {
    inControl = winner;
    layers.start();
  }
  act(HIERARCHY[inControl]);           // sets wheelLeft / wheelRight
}

// =====================================================================
//  The matrix
// =====================================================================
const uint8_t* signFor(Behaviour b) {
  switch (b) {
    case APPROACH_OBJECT:
    case APPROACH_LIGHT:
      return Layers::GREEN;
    case WANDER:
      return wander_goes_forward() ? Layers::GREEN : Layers::RED;
    default:
      return Layers::RED;              // escapes and avoids push away
  }
}

const uint8_t* colourFor(Behaviour b) {
  switch (b) {
    case ESCAPE_LEFT_COLLISION:
    case ESCAPE_RIGHT_COLLISION:
    case ESCAPE_FRONT_COLLISION:
      return Layers::WHITE;
    case AVOID_OBJECT:
    case APPROACH_OBJECT:
      return Layers::PURPLE;
    case AVOID_LIGHT:
    case APPROACH_LIGHT:
      return Layers::YELLOW;
    case WANDER:
      return Layers::BLUE;
  }
  return Layers::WHITE;
}

void drawLayers() {
  layers.setRows(LEVELS);
  for (int level = 0; level < LEVELS; level++) {
    layers.setRow(level, signFor(HIERARCHY[level]), colourFor(HIERARCHY[level]));
  }
  layers.setInControl(inControl);
  layers.setWheels(wheelLeft, wheelRight);
  layers.update();
}

// =====================================================================
//  Serial
// =====================================================================
void printLong(const char* label, Sensor* s) {
  if (present(s)) {
    Serial.printf("%s %5ld  ", label, s->raw());
  }
}

void printStatus() {
  if (millis() - lastPrintMs < 500) {
    return;
  }
  lastPrintMs = millis();
  printLong("distL", leftDistance);
  printLong("distR", rightDistance);
  printLong("lightL", leftLight);
  printLong("lightR", rightLight);
#if INERTIAL_COLLISION
  Serial.printf("jolt fwd %+.2f left %+.2f  ", impactForward, impactLeft);
#else
  Serial.printf("bumpers %c%c  ", is_left_bumper_pressed() ? 'L' : '-', is_right_bumper_pressed() ? 'R' : '-');
#endif
  if (paused) {
    Serial.printf("| paused\n");
    return;
  }
  Serial.printf("| %s -> L %+.2f R %+.2f\n", behaviourName(HIERARCHY[inControl]), wheelLeft, wheelRight);
}

// =====================================================================
//  Setup / loop
// =====================================================================
void setup() {
  Serial.begin(115200);
  delay(200);
  button.begin();
  pinMode(LEFT_BUMPER_PIN, INPUT_PULLUP);
  pinMode(RIGHT_BUMPER_PIN, INPUT_PULLUP);
  FastLED.addLeds<WS2812, 27, GRB>(leds, 25);
  FastLED.setBrightness(LED_BRIGHTNESS);
  FastLED.clear(true);

  leftWheel.begin();                  // finds the Motion base (both servos are on it)
  leftWheel.setMount(MotionServo::LEFT);
  rightWheel.setMount(MotionServo::RIGHT);
  leftWheel.setAllowReverse(true);
  rightWheel.setAllowReverse(true);
  rightWheel.stopNow();
  beginImu();                         // the IMU shares the Motion's bus

  findHub();
  findSensors();
  if (hubPort >= 0 && (PORTS[hubPort].pin1 == LEFT_BUMPER_PIN || PORTS[hubPort].pin2 == LEFT_BUMPER_PIN)) {
    Serial.println("Warning: the PaHub is in the bump switches' port");
  }

  Serial.printf("Hierarchy (top first):");
  for (int level = 0; level < LEVELS; level++) {
    Serial.printf(" %s%s", behaviourName(HIERARCHY[level]), level < LEVELS - 1 ? " ->" : "\n");
  }

  face.sayHello();
}

void loop() {
  updateSensors();
  updateImu();

  Button::Event press = button.update(false);
  if (greetingDone && press == Button::SHORT_PRESS) {
    paused = !paused;
    Serial.println(paused ? "Paused" : "Carrying on");
    if (paused) {
      face.tuneIn(Face::NEUTRAL);
    } else {
      layers.start();
    }
  }

  if (!greetingDone) {
    face.update();
    if (!face.isTalking()) {
      greetingDone = true;
      layers.start();
    }
    leftWheel.drive(0);
    rightWheel.drive(0);
  } else if (paused) {
    face.update();
    leftWheel.drive(0);
    rightWheel.drive(0);
  } else {
    int before = inControl;
    arbitrate();
    if (inControl != before) {
      Serial.printf("Control: %s\n", behaviourName(HIERARCHY[inControl]));
    }
    leftWheel.drive(wheelLeft);
    rightWheel.drive(wheelRight);
    drawLayers();
  }

  printStatus();
  leftWheel.update();
  rightWheel.update();
}
