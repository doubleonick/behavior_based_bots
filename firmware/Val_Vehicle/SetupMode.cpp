#include "SetupMode.h"
#include "Static.h"
#include "Palette.h"
#include "Icons.h"

namespace {

// The colour that stands for a Color Unit's channel
const Canvas::Color& colorForChannel(ColorSensor::Channel c) {
  if (c == ColorSensor::MAGENTA) {
    return Palette::MAGENTA;
  }
  if (c == ColorSensor::YELLOW) {
    return Palette::YELLOW;
  }
  if (c == ColorSensor::CYAN) {
    return Palette::CYAN;
  }
  return Palette::WHITE;
}

// The ring round the middle 3x3, anticlockwise from its top-left corner
// (down the left side, along the bottom, up the right side, back along the top)
const int8_t RING[8][2] = {{1, 1}, {2, 1}, {3, 1}, {3, 2}, {3, 3}, {2, 3}, {1, 3}, {1, 2}};

// How bright each of the five strength / self levels is drawn
const float LEVEL_SHADE[5] = {0.2f, 0.4f, 0.6f, 0.8f, 1.0f};

const uint32_t ENTER_MS     = 500;    // half a second of static
const uint32_t DONE_MS      = 1600;
const uint32_t NO_SENSOR_MS = 2000;   // how long "no sensor found" shows
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

const float SetupMode::GAINS[SetupMode::NEURON_LEVELS] = {1, 2, 5, 10, 20};
const float SetupMode::SELF_WEIGHTS[SetupMode::NEURON_LEVELS] = {0, 0.375f, 0.75f, 1.125f, 1.5f};

SetupMode::SetupMode(CRGB* leds, MotionServo& servo) : _leds(leds), _servo(servo) {}

// Which of the five levels is nearest to a value
int SetupMode::levelOf(float value, const float* levels) {
  int best = 0;
  for (int k = 1; k < NEURON_LEVELS; k++) {
    if (fabsf(levels[k] - value) < fabsf(levels[best] - value)) {
      best = k;
    }
  }
  return best;
}

// After a wire's calibration: its nerve cell, if the screens are on
void SetupMode::afterCalibration() {
  if (_neuronScreens) {
    goTo(CELL_PLACE);
  } else {
    nextWire();
  }
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
  _timer.restart();
  if (p == MOUNT) {
    _servo.drive(MOUNT_SPEED);           // turn slowly "forward" so you can check it
  } else {
    _servo.stopNow();
  }
}

// The wire being set up, if its sensor is a Color Unit (the only sensor of type 'C')
ColorSensor* SetupMode::colorUnit() {
  Sensor* s = _wires[_wire]->sensor;
  if (s != nullptr && s->typeCode() == 'C') {
    return static_cast<ColorSensor*>(s);
  }
  return nullptr;
}

void SetupMode::startWire() {
  if (colorUnit() != nullptr) {
    goTo(CHANNEL);
  } else {
    goTo(SIGN);
  }
}

void SetupMode::nextWire() {
  _wire++;
  if (_wire < _count) {
    startWire();
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
        startWire();
      } else {
        goTo(NO_SENSOR);
      }
    }
  } else if (_phase == CHANNEL) {
    ColorSensor* c = colorUnit();
    if (e == Button::SHORT_PRESS) {
      // cycle light -> magenta -> yellow -> cyan -> light
      int next = ((int)c->channel() + 1) % ColorSensor::CHANNEL_COUNT;
      c->setChannel((ColorSensor::Channel)next);
    } else if (e == Button::LONG_PRESS) {
      goTo(SIGN);
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
      } else if (!w->sensor->needsCalibration()) {
        nextWire();                      // an on/off sensor (a key): nothing to calibrate
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
      afterCalibration();
    }
  } else if (_phase == CELL_PLACE) {
    Connection* w = _wires[_wire];
    if (e == Button::SHORT_PRESS) {
      // cycle no cell -> on the wire -> next to the wire -> no cell
      if (w->place == Connection::NO_NEURON) {
        w->place = Connection::IN_THE_WIRE;
      } else if (w->place == Connection::IN_THE_WIRE) {
        w->place = Connection::AGAINST_THE_WIRE;
      } else {
        w->place = Connection::NO_NEURON;
      }
      w->neuron.reset();
    } else if (e == Button::LONG_PRESS) {
      if (w->place == Connection::NO_NEURON) {
        nextWire();                      // no cell: nothing more to set
      } else {
        goTo(CELL_THRESHOLD);
      }
    }
  } else if (_phase == CELL_THRESHOLD) {
    if (e == Button::LONG_PRESS) {
      // the cell wakes at what the sensor feels now (anything from 0 to 1)
      _wires[_wire]->neuron.threshold = _wires[_wire]->sensor->value();
      goTo(CELL_TRY);
    }
  } else if (_phase == CELL_TRY) {
    if (e == Button::SHORT_PRESS) {
      goTo(CELL_THRESHOLD);            // set it again
    } else if (e == Button::LONG_PRESS) {
      goTo(CELL_STRENGTH);
    }
  } else if (_phase == CELL_STRENGTH) {
    Neuron& n = _wires[_wire]->neuron;
    if (e == Button::SHORT_PRESS) {
      int next = (levelOf(n.gain, GAINS) + 1) % NEURON_LEVELS;
      n.gain = GAINS[next];
    } else if (e == Button::LONG_PRESS) {
      goTo(CELL_SELF);
    }
  } else if (_phase == CELL_SELF) {
    Neuron& n = _wires[_wire]->neuron;
    if (e == Button::SHORT_PRESS) {
      int next = (levelOf(n.selfWeight, SELF_WEIGHTS) + 1) % NEURON_LEVELS;
      n.selfWeight = SELF_WEIGHTS[next];
    } else if (e == Button::LONG_PRESS) {
      n.reset();                         // start fresh with the new settings
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
      startWire();
    } else {
      goTo(NO_SENSOR);
    }
  } else if (_phase == NO_SENSOR && now - _phaseMs >= NO_SENSOR_MS) {
    goTo(DONE);
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
      // the wire being set up blinks; a Color Unit's in its channel's colour
      const Canvas::Color* colour = &Palette::WHITE;
      Sensor* s = _wires[j]->sensor;
      if (s != nullptr && s->typeCode() == 'C') {
        colour = &colorForChannel(static_cast<ColorSensor*>(s)->channel());
      }
      _canvas.put(0, col, *colour, level);
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

// Column 4 for the threshold screens: the live meter in the wire's colour, its top
// pixel cyan (where the threshold would go), or, once set, a cyan mark at the threshold
void SetupMode::drawWireMeter(float level, const Canvas::Color& rgb, float threshold, bool showMark) {
  for (int k = 0; k < 4; k++) {
    float fill = constrain(level * 4 - k, 0.0f, 1.0f);
    _canvas.put(4 - k, 4, rgb, 0.12f + 0.78f * fill);
  }
  int top = (int)ceilf(level * 4) - 1;    // the meter's top pixel (-1 = empty)
  int mark = top;
  if (showMark) {
    mark = (int)ceilf(threshold * 4) - 1;
  }
  if (mark < 0) {
    mark = 0;                             // a threshold of 0 sits at the bottom
  }
  if (mark > 3) {
    mark = 3;
  }
  _canvas.put(4 - mark, 4, Palette::CYAN, 0.95f);
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
  } else if (_phase == NO_SENSOR) {
    _canvas.putRows(Icons::NO_SENSOR, '#', Palette::GREY, pulse);
  } else if (_phase == CHANNEL) {
    drawDots(t);
    ColorSensor::Channel c = colorUnit()->channel();
    if (c == ColorSensor::LIGHT) {
      _canvas.putRows(Icons::LETTER_L, '#', Palette::WHITE, pulse);
    } else {
      _canvas.putRows(Icons::BLOCK, '#', colorForChannel(c), pulse);
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
    // Icons depend on the kind of sensor: moon/sun for light, small/big square for
    // distance and reflective IR (both are "how close")
    drawDots(t);
    Sensor* s = _wires[_wire]->sensor;
    bool low = (_phase == LOW_POINT);
    if (s->typeCode() == 'D' || s->typeCode() == 'R') {
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
  } else if (_phase == CELL_PLACE) {
    drawDots(t);
    Connection* w = _wires[_wire];
    const Canvas::Color& wire = colorForWeight(w->weight);
    for (int row = 1; row <= 3; row++) {
      _canvas.put(row, 2, wire, 0.45f);               // the wire, running down the middle
    }
    for (int col = 1; col <= 3; col++) {
      _canvas.put(4, col, Palette::WHITE, 0.3f);       // the wheel
    }
    float blink = 0.12f;
    if (sinf(t * PI * 3) > -0.3f) {
      blink = 1.0f;
    }
    if (w->place == Connection::IN_THE_WIRE) {
      _canvas.put(2, 2, Palette::CYAN, blink);         // on the wire
    } else if (w->place == Connection::AGAINST_THE_WIRE) {
      _canvas.put(2, 3, Palette::CYAN, blink);         // next to the wire
    }
  } else if (_phase == CELL_THRESHOLD || _phase == CELL_TRY) {
    drawDots(t);
    Connection* w = _wires[_wire];
    float level = w->sensor->value();
    bool setting = (_phase == CELL_THRESHOLD);
    // the cell: asleep (slowly pulsing) while setting; on trying, awake when the
    // meter reaches the mark
    float cell = 0.1f + 0.2f * (0.5f + 0.5f * sinf(t * PI));
    if (!setting && level >= w->neuron.threshold) {
      cell = 1.0f;
    }
    for (int row = 1; row <= 3; row++) {
      for (int col = 0; col <= 2; col++) {
        _canvas.put(row, col, Palette::CYAN, cell);
      }
    }
    drawWireMeter(level, colorForWeight(w->weight), w->neuron.threshold, !setting);
  } else if (_phase == CELL_STRENGTH) {
    drawDots(t);
    float shade = LEVEL_SHADE[levelOf(_wires[_wire]->neuron.gain, GAINS)];
    int firing = (int)(t * 6) % 4;                     // rows 0, 1, 2 in turn, then a pause
    for (int r = 0; r < 3; r++) {
      for (int c = 0; c < 3; c++) {
        float b = 0.07f;
        if (r == firing) {
          b = shade;
        }
        _canvas.put(1 + r, 1 + c, Palette::CYAN, b);
      }
    }
  } else if (_phase == CELL_SELF) {
    drawDots(t);
    int level = levelOf(_wires[_wire]->neuron.selfWeight, SELF_WEIGHTS);
    int head = (int)(t * 8) % 8;                       // where the signal is on its way round
    for (int k = 0; k < 8; k++) {
      float b = 0.07f;
      if (level > 0) {
        int behind = (head - k + 8) % 8;
        if (behind < 3) {
          b = LEVEL_SHADE[level] * (1.0f - behind / 3.5f);
        }
      }
      _canvas.put(RING[k][0], RING[k][1], Palette::CYAN, b);
    }
    _canvas.put(2, 2, Palette::CYAN, 0.07f);           // the middle of the cell
  } else if (_phase == DONE) {
    drawDots(t);
    _canvas.putRows(Icons::CHECK, '#', Palette::GREEN, 0.9f);
  }
}

