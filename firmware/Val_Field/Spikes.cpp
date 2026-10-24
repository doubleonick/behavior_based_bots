#include "Spikes.h"

#include "Palette.h"
#include "Icons.h"

namespace {

// Colours (Palette.h): green for +, red for -, white for the wheel, amber backwards
const float SECONDS_PER_FRAME = FrameTimer::FRAME_MS / 1000.0f;

const float LANE_MIN_RATE = 0.5f;     // spikes per second with nothing to feel
const float LANE_MAX_RATE = 10.0f;    // ... and feeling as much as it can
const float WHEEL_MAX_RATE = 10.0f;   // wheel spikes per second at full speed
const float TAIL = 2.5f;              // a spike's tail, in rows
const int   LANE_MAX_AGE = 6;         // frames: 3 rows to travel, plus the tail
const float WHEEL_FADE = 3.0f;        // frames for a wheel spike to fade
const int   WHEEL_MAX_AGE = 6;        // frames: spreads 2 LEDs each way, plus the fade

float clamp01(float x) {
  if (x < 0) {
    return 0;
  }
  if (x > 1) {
    return 1;
  }
  return x;
}

}  // namespace

Spikes::Spikes(CRGB* leds) : _leds(leds) {
  start();
}

void Spikes::start() {
  for (int j = 0; j < MAX_LANES; j++) {
    _lane[j].phase = 0.3f * j;          // lanes start out of step with each other
    clearAges(_lane[j].ages);
  }
  for (int w = 0; w < 2; w++) {
    _wheel[w].phase = 0.5f + 0.25f * w;
    clearAges(_wheel[w].ages);
  }
  _timer.restart();
}

void Spikes::setLanes(int count) {
  if (count < 0) {
    count = 0;
  }
  if (count > MAX_LANES) {
    count = MAX_LANES;
  }
  _count = count;
}

void Spikes::setLane(int lane, float level, float weight) {
  if (lane < 0 || lane >= MAX_LANES) {
    return;
  }
  _lane[lane].level = clamp01(level);
  _lane[lane].weight = weight;
}

void Spikes::setWheel(float speed) {
  _twoWheels = false;
  _wheel[0].speed = clamp01(speed);
}

void Spikes::setWheels(float left, float right) {
  _twoWheels = true;
  _wheel[0].speed = max(-1.0f, min(1.0f, left));
  _wheel[1].speed = max(-1.0f, min(1.0f, right));
}

// Move a wheel's spikes along, and send a new one when it is due (faster = more often)
void Spikes::fireWheel(Wheel& wheel) {
  ageAll(wheel.ages, WHEEL_MAX_AGE);
  float speed = fabsf(wheel.speed);
  if (speed <= 0.01f) {
    return;
  }
  float before = wheel.phase;
  wheel.phase += WHEEL_MAX_RATE * speed * SECONDS_PER_FRAME;
  if ((int)wheel.phase > (int)before) {
    addAge(wheel.ages);
  }
  if (wheel.phase > 1000) {
    wheel.phase -= 1000;
  }
}

// A wheel's spikes start at startCol and spread one LED per frame, within fromCol..toCol
void Spikes::drawWheel(const Wheel& wheel, int fromCol, int toCol, int startCol) {
  const Canvas::Color& rgb = (wheel.speed >= 0) ? Palette::WHITE : Palette::AMBER;
  for (int k = 0; k < MAX_AGES; k++) {
    if (wheel.ages[k] < 0) {
      continue;
    }
    for (int col = fromCol; col <= toCol; col++) {
      int distance = col - startCol;
      if (distance < 0) {
        distance = -distance;
      }
      float behind = wheel.ages[k] - distance;
      if (behind >= 0 && behind < WHEEL_FADE) {
        _canvas.put(4, col, rgb, 1.0f - behind / WHEEL_FADE);
      }
    }
  }
}

void Spikes::clearAges(int* ages) {
  for (int k = 0; k < MAX_AGES; k++) {
    ages[k] = -1;
  }
}

// A new spike leaves: put it in a free slot (or replace the oldest)
void Spikes::addAge(int* ages) {
  int slot = 0;
  for (int k = 0; k < MAX_AGES; k++) {
    if (ages[k] < 0) {
      slot = k;
      break;
    }
    if (ages[k] > ages[slot]) {
      slot = k;
    }
  }
  ages[slot] = 0;
}

void Spikes::ageAll(int* ages, int maxAge) {
  for (int k = 0; k < MAX_AGES; k++) {
    if (ages[k] >= 0) {
      ages[k]++;
      if (ages[k] > maxAge) {
        ages[k] = -1;
      }
    }
  }
}

bool Spikes::update() {
  if (!_timer.due()) {
    return false;
  }
  _canvas.clear();

  // ---- The lanes: one per sensor ----
  for (int j = 0; j < _count; j++) {
    Lane& lane = _lane[j];
    const Canvas::Color& rgb = (lane.weight >= 0) ? Palette::GREEN : Palette::RED;
    int col = Icons::DOT_COLS[_count][j];

    // Move the spikes already on their way, then maybe send a new one
    ageAll(lane.ages, LANE_MAX_AGE);
    float rate = LANE_MIN_RATE + (LANE_MAX_RATE - LANE_MIN_RATE) * lane.level;
    float before = lane.phase;
    lane.phase += rate * SECONDS_PER_FRAME;
    if ((int)lane.phase > (int)before) {
      addAge(lane.ages);
    }
    if (lane.phase > 1000) {
      lane.phase -= 1000;               // keep the number small
    }

    // The sensor dot: brighter the more it feels
    _canvas.put(0, col, rgb, 0.4f + 0.6f * lane.level);

    // Each spike's head is one row further down every frame, with a fading tail
    for (int k = 0; k < MAX_AGES; k++) {
      if (lane.ages[k] < 0) {
        continue;
      }
      for (int row = 1; row <= 3; row++) {
        float behind = lane.ages[k] - (row - 1);   // how far the head has passed this row
        if (behind >= 0 && behind < TAIL) {
          _canvas.put(row, col, rgb, 1.0f - behind / TAIL);
        }
      }
    }
  }

  // ---- The wheels: the bottom row fires at the wheels' speed ----
  if (_twoWheels) {
    fireWheel(_wheel[0]);
    fireWheel(_wheel[1]);
    drawWheel(_wheel[0], 0, 1, 1);     // left wheel: starts next to the middle, spreads left
    drawWheel(_wheel[1], 3, 4, 3);     // right wheel: spreads right
  } else {
    fireWheel(_wheel[0]);
    drawWheel(_wheel[0], 0, 4, 2);     // one wheel: spreads both ways from the middle
  }

  _canvas.push(_leds);
  return true;
}
