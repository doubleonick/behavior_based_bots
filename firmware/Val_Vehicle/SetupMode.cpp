#include "SetupMode.h"
#include "Static.h"
#include "Palette.h"
#include "Icons.h"

namespace {

const uint32_t ENTER_MS     = 500;    // half a second of static
const uint32_t DONE_MS      = 1600;
const float    MOUNT_SPEED  = 0.3f;   // how fast the wheel turns while choosing the mount

const Canvas::Color& colorForWeight(float w) {
  if (w > 0) {
    return Palette::GREEN;
  }
  if (w < 0) {
    return Palette::RED;
  }
  return Palette::GREY;
}
}

SetupMode::SetupMode(CRGB* leds, MotionServo& servo) : _leds(leds), _servo(servo) {}

void SetupMode::start(Connection** wires, int count, bool withMount) {
  _count = min(count, 3);
  for (int n = 0; n < _count; n++) {
    _wires[n] = wires[n];
  }
  _wire = 0;
  _finished = false;
  _withMount = withMount;
  _servo.stopNow();
  goTo(ENTER);
}

void SetupMode::goTo(Phase p) {
  _phase = p;
  _phaseMs = millis();
  _timer.restart();
  if (p == MOUNT) {
    _servo.drive(MOUNT_SPEED);           // turn slowly "forward" so you can check it
  } else {
    _servo.stopNow();
  }
}

void SetupMode::nextWire() {
  _wire++;
  if (_wire < _count) {
    goTo(SIGN);
  } else {
    goTo(DONE);
  }
}

bool SetupMode::takeFinished() {
  bool f = _finished;
  _finished = false;
  return f;
}

void SetupMode::handle(Button::Event e) {
  if (e == Button::NONE) {
    return;
  }
  if (_phase == MOUNT) {
    if (e == Button::SHORT_PRESS) {
      if (_servo.mount() == MotionServo::LEFT) {
        _servo.setMount(MotionServo::RIGHT);
      } else {
        _servo.setMount(MotionServo::LEFT);
      }
    } else if (e == Button::LONG_PRESS) {
      if (_count > 0) {
        _wire = 0;
        goTo(SIGN);
      } else {
        goTo(DONE);
      }
    }
  } else if (_phase == SIGN) {
    Connection* w = _wires[_wire];
    if (e == Button::SHORT_PRESS) {
      // cycle + -> - -> x -> +
      if (w->weight > 0) {
        w->weight = -1;
      } else if (w->weight < 0) {
        w->weight = 0;
      } else {
        w->weight = 1;
      }
    } else if (e == Button::LONG_PRESS) {
      if (w->weight == 0) {
        nextWire();                      // not connected: nothing to calibrate
      } else {
        goTo(LOW_POINT);
      }
    }
  } else if (_phase == LOW_POINT) {
    if (e == Button::LONG_PRESS) {
      _wires[_wire]->sensor->calibrateLow();
      goTo(HIGH_POINT);
    }
  } else if (_phase == HIGH_POINT) {
    if (e == Button::LONG_PRESS) {
      _wires[_wire]->sensor->calibrateHigh();
      nextWire();
    }
  }
}

void SetupMode::update() {
  if (_phase == OFF) {
    return;
  }
  uint32_t now = millis();
  if (_phase == ENTER && now - _phaseMs >= ENTER_MS) {
    if (_withMount) {
      goTo(MOUNT);
    } else if (_count > 0) {
      goTo(SIGN);
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
// Top row: one dot per wire
void SetupMode::drawDots(float t) {
  for (int j = 0; j < _count; j++) {
    int col = Icons::DOT_COLS[_count][j];
    if (_phase == DONE || j < _wire) {
      float w = _wires[j]->weight;
      float level = 0.7f;
      if (w == 0) {
        level = 0.35f;
      }
      _canvas.put(0, col, colorForWeight(w), level);
    } else if (j == _wire) {
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

// Column 4: live meter (stimulus), filling from the bottom
void SetupMode::drawMeter(float level) {
  for (int k = 0; k < 4; k++) {
    float fill = constrain(level * 4 - k, 0.0f, 1.0f);
    _canvas.put(4 - k, 4, Palette::WHITE, 0.15f + 0.75f * fill);
  }
}

void SetupMode::draw() {
  _canvas.clear();
  float t = (millis() - _phaseMs) / 1000.0f;
  float pulse = 0.6f + 0.3f * sinf(t * PI * 2);

  if (_phase == ENTER) {
    // white static: a new random pattern every frame
    uint32_t frame = (millis() - _phaseMs) / Static::FRAME_MS;
    for (int k = 0; k < 25; k++) {
      _canvas.put(k / 5, k % 5, Palette::STATIC, Static::level(frame, k));
    }
  } else if (_phase == MOUNT) {
    // "forward" arrow with brightness running up it, plus a corner dot for the side
    for (int k = 0; k < 9; k++) {
      float wave = 0.5f + 0.5f * sinf((Icons::ARROW[k][0] + t * 4) * 1.6f);
      _canvas.put(Icons::ARROW[k][0], Icons::ARROW[k][1], Palette::BLUE, 0.35f + 0.55f * wave);
    }
    if (_servo.mount() == MotionServo::LEFT) {
      _canvas.put(4, 0, Palette::AMBER, 0.9f);
    } else {
      _canvas.put(4, 4, Palette::AMBER, 0.9f);
    }
  } else if (_phase == SIGN) {
    drawDots(t);
    float w = _wires[_wire]->weight;
    if (w > 0) {
      _canvas.putRows(Icons::PLUS, '#', Palette::GREEN, pulse);
    } else if (w < 0) {
      _canvas.putRows(Icons::MINUS, '#', Palette::RED, pulse);
    } else {
      _canvas.putRows(Icons::CROSS, '#', Palette::GREY, pulse);
    }
  } else if (_phase == LOW_POINT || _phase == HIGH_POINT) {
    // Icons depend on the kind of sensor: moon/sun for light, small/big square for distance
    drawDots(t);
    Sensor* s = _wires[_wire]->sensor;
    bool low = (_phase == LOW_POINT);
    if (s->typeCode() == 'D') {
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
    drawMeter(s->meterLevel());
  } else if (_phase == DONE) {
    drawDots(t);
    _canvas.putRows(Icons::CHECK, '#', Palette::GREEN, 0.9f);
  }
}

