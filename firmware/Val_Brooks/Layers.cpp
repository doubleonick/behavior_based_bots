#include "Layers.h"

namespace {

const float    SECONDS_PER_FRAME = FrameTimer::FRAME_MS / 1000.0f;
const float    MAX_RATE = 10.0f;      // spikes per second at full speed
const float    STEADY = 0.6f;         // a row not in control (the face's normal brightness)
const float    UNDER_SPIKES = 0.35f;  // the row in control, between spikes
const float    TAIL = 2.5f;           // a spike's tail, in columns
const int      MAX_AGE = 8;           // frames: 5 columns to cross, plus the tail

}  // namespace

Layers::Layers(CRGB* leds) : _leds(leds) {
  for (int r = 0; r < MAX_ROWS; r++) {
    _sign[r] = nullptr;
    _colour[r] = nullptr;
  }
  start();
}

void Layers::start() {
  for (int k = 0; k < MAX_AGES; k++) {
    _ages[k] = -1;
  }
  _phase = 0.5f;
  _timer.restart();
}

void Layers::setRows(int count) {
  if (count < 0) {
    count = 0;
  }
  if (count > MAX_ROWS) {
    count = MAX_ROWS;
  }
  _count = count;
}

void Layers::setRow(int row, const Canvas::Color* sign, const Canvas::Color* colour) {
  if (row < 0 || row >= MAX_ROWS) {
    return;
  }
  _sign[row] = sign;
  _colour[row] = colour;
}

void Layers::setInControl(int row) {
  _inControl = row;
}

// The spikes come as often as the wheels' average speed (forwards or backwards)
void Layers::setWheels(float left, float right) {
  float average = (fabsf(left) + fabsf(right)) / 2;
  if (average > 1) {
    average = 1;
  }
  _rate = MAX_RATE * average;
}

bool Layers::update() {
  if (!_timer.due()) {
    return false;
  }

  // Move the spikes along, and send a new one when it is due
  for (int k = 0; k < MAX_AGES; k++) {
    if (_ages[k] >= 0) {
      _ages[k]++;
      if (_ages[k] > MAX_AGE) {
        _ages[k] = -1;
      }
    }
  }
  float before = _phase;
  _phase += _rate * SECONDS_PER_FRAME;
  if ((int)_phase > (int)before) {
    int slot = 0;
    for (int k = 0; k < MAX_AGES; k++) {
      if (_ages[k] < 0) {
        slot = k;
        break;
      }
      if (_ages[k] > _ages[slot]) {
        slot = k;
      }
    }
    _ages[slot] = 0;
  }
  if (_phase > 1000) {
    _phase -= 1000;
  }

  _canvas.clear();
  for (int row = 0; row < _count; row++) {
    for (int col = 0; col < 5; col++) {
      const Canvas::Color* rgb = nullptr;
      if (col == 0) {
        rgb = _sign[row];
      } else if (col >= 2) {
        rgb = _colour[row];
      }
      if (rgb == nullptr) {
        continue;                      // the gap, or a row with no sign
      }
      float intensity = STEADY;
      if (row == _inControl) {
        // The head of each spike is one column further right every frame, with a fading tail
        intensity = UNDER_SPIKES;
        for (int k = 0; k < MAX_AGES; k++) {
          if (_ages[k] < 0) {
            continue;
          }
          float behind = _ages[k] - col;
          if (behind >= 0 && behind < TAIL) {
            float spike = 1.0f - behind / TAIL * 0.65f;
            if (spike > intensity) {
              intensity = spike;
            }
          }
        }
      }
      _canvas.put(row, col, *rgb, intensity);
    }
  }
  _canvas.push(_leds);
  return true;
}
