#include "FieldSetup.h"
#include "Static.h"

namespace {
// LED colours
const uint8_t WHITE[3]  = {180, 180, 200};
const uint8_t GREEN[3]  = {0, 255, 60};
const uint8_t RED[3]    = {255, 0, 0};
const uint8_t GREY[3]   = {70, 70, 80};
const uint8_t BLUE[3]   = {0, 150, 255};
const uint8_t AMBER[3]  = {255, 110, 0};
const uint8_t MOON[3]   = {40, 60, 255};
const uint8_t SUN[3]    = {255, 160, 0};
const uint8_t TEAL[3]   = {0, 200, 160};

const char* const PLUS[5]  = {".....", ".....", "..#..", ".###.", "..#.."};
const char* const MINUS[5] = {".....", ".....", ".....", ".###.", "....."};
const char* const CROSS[5] = {".....", ".....", ".#.#.", "..#..", ".#.#."};
const char* const MOON_ROWS[5] = {".....", ".##..", "#....", "#....", ".##.."};
const char* const SUN_ROWS[5]  = {".....", "#..#.", ".##..", ".##..", "#..#."};
const char* const FAR_ROWS[5]  = {".....", ".....", ".##..", ".##..", "....."};   // small = far away
const char* const NEAR_ROWS[5] = {".....", "####.", "#..#.", "#..#.", "####."};   // big = close
const char* const CHECK[5] = {".....", "....#", "...#.", "#.#..", ".#..."};

// The "forward" arrow for the wheel steps (row, col)
const int8_t WHEEL_ARROW[9][2] = {{0, 2}, {1, 1}, {1, 2}, {1, 3}, {2, 0}, {2, 2}, {2, 4}, {3, 2}, {4, 2}};

// The 8 compass directions as (row, col) steps, going anticlockwise from forward
// (forward is UP on the matrix, left is LEFT).
const int8_t DIRECTION[8][2] = {
  {-1, 0},    // 0:    0 deg  forward
  {-1, -1},   // 1:   45 deg  forward-left
  {0, -1},    // 2:   90 deg  left
  {1, -1},    // 3:  135 deg  back-left
  {1, 0},     // 4:  180 deg  back
  {1, 1},     // 5: -135 deg  back-right
  {0, 1},     // 6:  -90 deg  right
  {-1, 1},    // 7:  -45 deg  forward-right
};

// Where the sensor dots go on the top row, for 1, 2 or 3 sensors
const int8_t DOT_COLS[4][3] = {{0, 0, 0}, {2, 0, 0}, {1, 3, 0}, {0, 2, 4}};

const uint32_t FRAME_MS    = 40;
const uint32_t ENTER_MS    = 500;    // half a second of static
const uint32_t DONE_MS     = 1600;
const float    WHEEL_SPEED = 0.3f;    // how fast a wheel turns while checking its direction

const uint8_t* colorForPolicy(PotentialField::Policy p) {
  if (p == PotentialField::PULL) {
    return GREEN;
  }
  if (p == PotentialField::PUSH) {
    return RED;
  }
  return GREY;
}
}

FieldSetup::FieldSetup(CRGB* leds, MotionServo& leftWheel, MotionServo& rightWheel)
    : _leds(leds), _left(leftWheel), _right(rightWheel) {
  clear();
}

float FieldSetup::angleFromStep(int step) {
  step = ((step % 8) + 8) % 8;
  if (step <= 4) {
    return step * 45.0f;
  }
  return (step - 8) * 45.0f;          // 5, 6, 7 -> -135, -90, -45
}

int FieldSetup::stepFromAngle(float angle) {
  int step = (int)lroundf(angle / 45.0f);
  return ((step % 8) + 8) % 8;
}

void FieldSetup::start(PotentialField::Source** sources, int count, bool withWheels) {
  _count = min(count, 3);
  for (int n = 0; n < _count; n++) {
    _sources[n] = sources[n];
  }
  _current = 0;
  _finished = false;
  _withWheels = withWheels;
  goTo(ENTER);
}

void FieldSetup::goTo(Phase p) {
  _phase = p;
  _phaseMs = millis();
  _lastDrawMs = 0;
  _left.stopNow();
  _right.stopNow();
  if (p == WHEEL_LEFT) {
    _left.drive(WHEEL_SPEED);             // turn slowly "forward" so you can check it
  } else if (p == WHEEL_RIGHT) {
    _right.drive(WHEEL_SPEED);
  }
}

void FieldSetup::nextSource() {
  _current++;
  if (_current < _count) {
    goTo(POLICY);
  } else {
    goTo(DONE);
  }
}

void FieldSetup::flip(MotionServo& servo) {
  if (servo.mount() == MotionServo::LEFT) {
    servo.setMount(MotionServo::RIGHT);
  } else {
    servo.setMount(MotionServo::LEFT);
  }
}

bool FieldSetup::takeFinished() {
  bool f = _finished;
  _finished = false;
  return f;
}

void FieldSetup::handle(Button::Event e) {
  if (e == Button::NONE) {
    return;
  }
  if (_phase == WHEEL_LEFT) {
    if (e == Button::SHORT_PRESS) {
      flip(_left);
    } else if (e == Button::LONG_PRESS) {
      goTo(WHEEL_RIGHT);
    }
  } else if (_phase == WHEEL_RIGHT) {
    if (e == Button::SHORT_PRESS) {
      flip(_right);
    } else if (e == Button::LONG_PRESS) {
      if (_count > 0) {
        _current = 0;
        goTo(POLICY);
      } else {
        goTo(DONE);
      }
    }
  } else if (_phase == POLICY) {
    PotentialField::Source* s = _sources[_current];
    if (e == Button::SHORT_PRESS) {
      // cycle pull -> push -> off -> pull
      if (s->policy == PotentialField::PULL) {
        s->policy = PotentialField::PUSH;
      } else if (s->policy == PotentialField::PUSH) {
        s->policy = PotentialField::OFF;
      } else {
        s->policy = PotentialField::PULL;
      }
    } else if (e == Button::LONG_PRESS) {
      if (s->policy == PotentialField::OFF) {
        nextSource();                      // off: nothing more to set
      } else {
        goTo(ANGLE);
      }
    }
  } else if (_phase == ANGLE) {
    PotentialField::Source* s = _sources[_current];
    if (e == Button::SHORT_PRESS) {
      s->angle = angleFromStep(stepFromAngle(s->angle) + 1);   // 45 degrees anticlockwise
    } else if (e == Button::LONG_PRESS) {
      goTo(LOW_POINT);
    }
  } else if (_phase == LOW_POINT) {
    if (e == Button::LONG_PRESS) {
      _sources[_current]->sensor->calibrateLow();
      goTo(HIGH_POINT);
    }
  } else if (_phase == HIGH_POINT) {
    if (e == Button::LONG_PRESS) {
      _sources[_current]->sensor->calibrateHigh();
      nextSource();
    }
  }
}

void FieldSetup::update() {
  if (_phase == OFF) {
    return;
  }
  uint32_t now = millis();
  if (_phase == ENTER && now - _phaseMs >= ENTER_MS) {
    if (_withWheels) {
      goTo(WHEEL_LEFT);
    } else if (_count > 0) {
      goTo(POLICY);
    } else {
      goTo(DONE);
    }
  } else if (_phase == DONE && now - _phaseMs >= DONE_MS) {
    _phase = OFF;
    _finished = true;
    return;
  }
  if (_lastDrawMs != 0 && now - _lastDrawMs < FRAME_MS) {
    return;
  }
  _lastDrawMs = now;
  draw();
  push();
}

// ---------------------------------------------------------------------
//  Drawing
// ---------------------------------------------------------------------
void FieldSetup::clear() {
  for (int n = 0; n < 25; n++) {
    _frame[n] = {0, 0, 0, 0};
  }
}

void FieldSetup::put(int row, int col, const uint8_t* rgb, float intensity) {
  if (row < 0 || row > 4 || col < 0 || col > 4 || intensity <= 0) {
    return;
  }
  Px& p = _frame[row * 5 + col];
  if (intensity > p.i) {
    p.r = rgb[0];
    p.g = rgb[1];
    p.b = rgb[2];
    p.i = min(intensity, 1.0f);
  }
}

void FieldSetup::drawRows(const char* const rows[5], const uint8_t* rgb, float intensity) {
  for (int r = 0; r < 5; r++) {
    for (int c = 0; c < 5; c++) {
      if (rows[r][c] == '#') {
        put(r, c, rgb, intensity);
      }
    }
  }
}

// Top row: one dot per sensor
void FieldSetup::drawDots(float t) {
  for (int j = 0; j < _count; j++) {
    int col = DOT_COLS[_count][j];
    if (_phase == DONE || j < _current) {
      PotentialField::Policy p = _sources[j]->policy;
      float level = 0.7f;
      if (p == PotentialField::OFF) {
        level = 0.35f;
      }
      put(0, col, colorForPolicy(p), level);
    } else if (j == _current) {
      float level = 0.15f;
      if (sinf(t * PI * 3) > -0.3f) {
        level = 0.9f;
      }
      put(0, col, WHITE, level);
    } else {
      put(0, col, WHITE, 0.22f);
    }
  }
}

// Column 4: live meter of the sensor's stimulus, filling from the bottom
void FieldSetup::drawMeter(float level) {
  for (int k = 0; k < 4; k++) {
    float fill = constrain(level * 4 - k, 0.0f, 1.0f);
    put(4 - k, 4, WHITE, 0.15f + 0.75f * fill);
  }
}

// An arrow from the centre of the matrix toward the edge, in one of 8 directions
void FieldSetup::drawArrow(int step, const uint8_t* rgb, float intensity) {
  int dr = DIRECTION[step][0];
  int dc = DIRECTION[step][1];
  put(2, 2, WHITE, 0.3f);                              // the robot
  put(2 + dr, 2 + dc, rgb, intensity);                 // shaft
  int tipR = 2 + 2 * dr;
  int tipC = 2 + 2 * dc;
  put(tipR, tipC, rgb, intensity);                     // tip
  // the two barbs point back from the tip, 45 degrees either side of the shaft
  int back = (step + 4) % 8;
  int barb1 = (back + 1) % 8;
  int barb2 = (back + 7) % 8;
  put(tipR + DIRECTION[barb1][0], tipC + DIRECTION[barb1][1], rgb, intensity);
  put(tipR + DIRECTION[barb2][0], tipC + DIRECTION[barb2][1], rgb, intensity);
}

void FieldSetup::draw() {
  clear();
  float t = (millis() - _phaseMs) / 1000.0f;
  float pulse = 0.6f + 0.3f * sinf(t * PI * 2);

  if (_phase == ENTER) {
    // white static: a new random pattern every frame
    uint32_t frame = (millis() - _phaseMs) / Static::FRAME_MS;
    for (int k = 0; k < 25; k++) {
      put(k / 5, k % 5, Static::COLOR, Static::level(frame, k));
    }
  } else if (_phase == WHEEL_LEFT || _phase == WHEEL_RIGHT) {
    // "forward" arrow with brightness running up it; the amber dot shows which wheel
    for (int k = 0; k < 9; k++) {
      float wave = 0.5f + 0.5f * sinf((WHEEL_ARROW[k][0] + t * 4) * 1.6f);
      put(WHEEL_ARROW[k][0], WHEEL_ARROW[k][1], BLUE, 0.35f + 0.55f * wave);
    }
    if (_phase == WHEEL_LEFT) {
      put(4, 0, AMBER, 0.9f);
    } else {
      put(4, 4, AMBER, 0.9f);
    }
  } else if (_phase == POLICY) {
    drawDots(t);
    PotentialField::Policy p = _sources[_current]->policy;
    if (p == PotentialField::PULL) {
      drawRows(PLUS, GREEN, pulse);
    } else if (p == PotentialField::PUSH) {
      drawRows(MINUS, RED, pulse);
    } else {
      drawRows(CROSS, GREY, pulse);
    }
  } else if (_phase == ANGLE) {
    PotentialField::Source* s = _sources[_current];
    drawArrow(stepFromAngle(s->angle), colorForPolicy(s->policy), pulse);
  } else if (_phase == LOW_POINT || _phase == HIGH_POINT) {
    // Icons depend on the kind of sensor: moon/sun for light, small/big square for distance
    drawDots(t);
    Sensor* sensor = _sources[_current]->sensor;
    bool low = (_phase == LOW_POINT);
    if (sensor->typeCode() == 'D') {
      if (low) {
        drawRows(FAR_ROWS, TEAL, pulse * 0.7f);
      } else {
        drawRows(NEAR_ROWS, TEAL, pulse);
      }
    } else {
      if (low) {
        drawRows(MOON_ROWS, MOON, pulse);
      } else {
        drawRows(SUN_ROWS, SUN, pulse);
      }
    }
    drawMeter(sensor->meterLevel());
  } else if (_phase == DONE) {
    drawDots(t);
    drawRows(CHECK, GREEN, 0.9f);
  }
}

// Copy to the LEDs. Intensity is squared so fades look even to the eye.
void FieldSetup::push() {
  for (int n = 0; n < 25; n++) {
    const Px& p = _frame[n];
    if (p.i <= 0.02f) {
      _leds[n] = CRGB::Black;
    } else {
      float s = p.i * p.i;
      _leds[n] = CRGB((uint8_t)(p.r * s), (uint8_t)(p.g * s), (uint8_t)(p.b * s));
    }
  }
  FastLED.show();
}
