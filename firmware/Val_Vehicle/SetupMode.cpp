#include "SetupMode.h"
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
const char* const MOON_ROWS[5]  = {".....", ".##..", "#....", "#....", ".##.."};
const char* const SUN_ROWS[5]   = {".....", "#..#.", ".##..", ".##..", "#..#."};
const char* const FAR_ROWS[5]   = {".....", ".....", ".##..", ".##..", "....."};   // small = far away
const char* const NEAR_ROWS[5]  = {".....", "####.", "#..#.", "#..#.", "####."};   // big = close
const char* const CHECK[5] = {".....", "....#", "...#.", "#.#..", ".#..."};

// The "forward" arrow (row, col)
const int8_t ARROW[9][2] = {{0, 2}, {1, 1}, {1, 2}, {1, 3}, {2, 0}, {2, 2}, {2, 4}, {3, 2}, {4, 2}};

// Where the wire dots go on the top row, for 1, 2 or 3 wires
const int8_t DOT_COLS[4][3] = {{0, 0, 0}, {2, 0, 0}, {1, 3, 0}, {0, 2, 4}};

const uint32_t FRAME_MS     = 40;
const uint32_t ENTER_MS     = 500;    // half a second of static
const uint32_t DONE_MS      = 1600;
const float    MOUNT_SPEED  = 0.3f;   // how fast the wheel turns while choosing the mount

const uint8_t* colorForWeight(float w) {
  if (w > 0) {
    return GREEN;
  }
  if (w < 0) {
    return RED;
  }
  return GREY;
}
}

SetupMode::SetupMode(CRGB* leds, MotionServo& servo) : _leds(leds), _servo(servo) {
  clear();
}

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
  _lastDrawMs = 0;
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
void SetupMode::clear() {
  for (int n = 0; n < 25; n++) {
    _frame[n] = {0, 0, 0, 0};
  }
}

void SetupMode::put(int row, int col, const uint8_t* rgb, float intensity) {
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

void SetupMode::drawRows(const char* const rows[5], const uint8_t* rgb, float intensity) {
  for (int r = 0; r < 5; r++) {
    for (int c = 0; c < 5; c++) {
      if (rows[r][c] == '#') {
        put(r, c, rgb, intensity);
      }
    }
  }
}

// Top row: one dot per wire
void SetupMode::drawDots(float t) {
  for (int j = 0; j < _count; j++) {
    int col = DOT_COLS[_count][j];
    if (_phase == DONE || j < _wire) {
      float w = _wires[j]->weight;
      float level = 0.7f;
      if (w == 0) {
        level = 0.35f;
      }
      put(0, col, colorForWeight(w), level);
    } else if (j == _wire) {
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

// Column 4: live meter (stimulus), filling from the bottom
void SetupMode::drawMeter(float level) {
  for (int k = 0; k < 4; k++) {
    float fill = constrain(level * 4 - k, 0.0f, 1.0f);
    put(4 - k, 4, WHITE, 0.15f + 0.75f * fill);
  }
}

void SetupMode::draw() {
  clear();
  float t = (millis() - _phaseMs) / 1000.0f;
  float pulse = 0.6f + 0.3f * sinf(t * PI * 2);

  if (_phase == ENTER) {
    // white static: a new random pattern every frame
    uint32_t frame = (millis() - _phaseMs) / Static::FRAME_MS;
    for (int k = 0; k < 25; k++) {
      put(k / 5, k % 5, Static::COLOR, Static::level(frame, k));
    }
  } else if (_phase == MOUNT) {
    // "forward" arrow with brightness running up it, plus a corner dot for the side
    for (int k = 0; k < 9; k++) {
      float wave = 0.5f + 0.5f * sinf((ARROW[k][0] + t * 4) * 1.6f);
      put(ARROW[k][0], ARROW[k][1], BLUE, 0.35f + 0.55f * wave);
    }
    if (_servo.mount() == MotionServo::LEFT) {
      put(4, 0, AMBER, 0.9f);
    } else {
      put(4, 4, AMBER, 0.9f);
    }
  } else if (_phase == SIGN) {
    drawDots(t);
    float w = _wires[_wire]->weight;
    if (w > 0) {
      drawRows(PLUS, GREEN, pulse);
    } else if (w < 0) {
      drawRows(MINUS, RED, pulse);
    } else {
      drawRows(CROSS, GREY, pulse);
    }
  } else if (_phase == LOW_POINT || _phase == HIGH_POINT) {
    // Icons depend on the kind of sensor: moon/sun for light, small/big square for distance
    drawDots(t);
    Sensor* s = _wires[_wire]->sensor;
    bool low = (_phase == LOW_POINT);
    if (s->typeCode() == 'D') {
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
    drawMeter(s->meterLevel());
  } else if (_phase == DONE) {
    drawDots(t);
    drawRows(CHECK, GREEN, 0.9f);
  }
}

// Copy to the LEDs. Intensity is squared so fades look even to the eye.
void SetupMode::push() {
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
