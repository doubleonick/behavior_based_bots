#include "FieldSetup.h"
#include "Static.h"
#include "Palette.h"
#include "Icons.h"

namespace {

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

const uint32_t ENTER_MS    = 500;    // half a second of static
const uint32_t DONE_MS     = 1600;
const float    WHEEL_SPEED = 0.3f;    // how fast a wheel turns while checking its direction

const Canvas::Color& colorForPolicy(PotentialField::Policy p) {
  if (p == PotentialField::PULL) {
    return Palette::GREEN;
  }
  if (p == PotentialField::PUSH) {
    return Palette::RED;
  }
  return Palette::GREY;
}
}

FieldSetup::FieldSetup(CRGB* leds, MotionServo& leftWheel, MotionServo& rightWheel)
    : _leds(leds), _left(leftWheel), _right(rightWheel) {}

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
  _timer.restart();
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
      if (s->sensor->needsCalibration()) {
        goTo(LOW_POINT);
      } else {
        nextSource();                      // an on/off sensor (a key): nothing to calibrate
      }
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
  if (!_timer.due()) {
    return;
  }
  draw();
  _canvas.push(_leds);
}

// ---------------------------------------------------------------------
//  Drawing
// ---------------------------------------------------------------------
// Top row: one dot per sensor
void FieldSetup::drawDots(float t) {
  for (int j = 0; j < _count; j++) {
    int col = Icons::DOT_COLS[_count][j];
    if (_phase == DONE || j < _current) {
      PotentialField::Policy p = _sources[j]->policy;
      float level = 0.7f;
      if (p == PotentialField::OFF) {
        level = 0.35f;
      }
      _canvas.put(0, col, colorForPolicy(p), level);
    } else if (j == _current) {
      float level = 0.15f;
      if (sinf(t * PI * 3) > -0.3f) {
        level = 0.9f;
      }
      _canvas.put(0, col, Palette::WHITE, level);
    } else {
      _canvas.put(0, col, Palette::WHITE, 0.22f);
    }
  }
}

// Column 4: live meter of the sensor's stimulus, filling from the bottom
void FieldSetup::drawMeter(float level) {
  for (int k = 0; k < 4; k++) {
    float fill = constrain(level * 4 - k, 0.0f, 1.0f);
    _canvas.put(4 - k, 4, Palette::WHITE, 0.15f + 0.75f * fill);
  }
}

// An arrow from the centre of the matrix toward the edge, in one of 8 directions
void FieldSetup::drawArrow(int step, const Canvas::Color& rgb, float intensity) {
  int dr = DIRECTION[step][0];
  int dc = DIRECTION[step][1];
  _canvas.put(2, 2, Palette::WHITE, 0.3f);                              // the robot
  _canvas.put(2 + dr, 2 + dc, rgb, intensity);                 // shaft
  int tipR = 2 + 2 * dr;
  int tipC = 2 + 2 * dc;
  _canvas.put(tipR, tipC, rgb, intensity);                     // tip
  // the two barbs point back from the tip, 45 degrees either side of the shaft
  int back = (step + 4) % 8;
  int barb1 = (back + 1) % 8;
  int barb2 = (back + 7) % 8;
  _canvas.put(tipR + DIRECTION[barb1][0], tipC + DIRECTION[barb1][1], rgb, intensity);
  _canvas.put(tipR + DIRECTION[barb2][0], tipC + DIRECTION[barb2][1], rgb, intensity);
}

void FieldSetup::draw() {
  _canvas.clear();
  float t = (millis() - _phaseMs) / 1000.0f;
  float pulse = 0.6f + 0.3f * sinf(t * PI * 2);

  if (_phase == ENTER) {
    // white static: a new random pattern every frame
    uint32_t frame = (millis() - _phaseMs) / Static::FRAME_MS;
    for (int k = 0; k < 25; k++) {
      _canvas.put(k / 5, k % 5, Palette::STATIC, Static::level(frame, k));
    }
  } else if (_phase == WHEEL_LEFT || _phase == WHEEL_RIGHT) {
    // "forward" arrow with brightness running up it; the amber dot shows which wheel
    for (int k = 0; k < 9; k++) {
      float wave = 0.5f + 0.5f * sinf((Icons::ARROW[k][0] + t * 4) * 1.6f);
      _canvas.put(Icons::ARROW[k][0], Icons::ARROW[k][1], Palette::BLUE, 0.35f + 0.55f * wave);
    }
    if (_phase == WHEEL_LEFT) {
      _canvas.put(4, 0, Palette::AMBER, 0.9f);
    } else {
      _canvas.put(4, 4, Palette::AMBER, 0.9f);
    }
  } else if (_phase == POLICY) {
    drawDots(t);
    PotentialField::Policy p = _sources[_current]->policy;
    if (p == PotentialField::PULL) {
      _canvas.putRows(Icons::PLUS, '#', Palette::GREEN, pulse);
    } else if (p == PotentialField::PUSH) {
      _canvas.putRows(Icons::MINUS, '#', Palette::RED, pulse);
    } else {
      _canvas.putRows(Icons::CROSS, '#', Palette::GREY, pulse);
    }
  } else if (_phase == ANGLE) {
    PotentialField::Source* s = _sources[_current];
    drawArrow(stepFromAngle(s->angle), colorForPolicy(s->policy), pulse);
  } else if (_phase == LOW_POINT || _phase == HIGH_POINT) {
    // Icons depend on the kind of sensor: moon/sun for light, small/big square for
    // distance and reflective IR (both are "how close")
    drawDots(t);
    Sensor* sensor = _sources[_current]->sensor;
    bool low = (_phase == LOW_POINT);
    if (sensor->typeCode() == 'D' || sensor->typeCode() == 'R') {
      if (low) {
        _canvas.putRows(Icons::FAR, '#', Palette::TEAL, pulse * 0.7f);
      } else {
        _canvas.putRows(Icons::NEAR, '#', Palette::TEAL, pulse);
      }
    } else {
      if (low) {
        _canvas.putRows(Icons::MOON, '#', Palette::MOON, pulse);
      } else {
        _canvas.putRows(Icons::SUN, '#', Palette::SUN, pulse);
      }
    }
    drawMeter(sensor->meterLevel());
  } else if (_phase == DONE) {
    drawDots(t);
    _canvas.putRows(Icons::CHECK, '#', Palette::GREEN, 0.9f);
  }
}

