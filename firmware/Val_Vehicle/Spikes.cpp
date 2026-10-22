#include "Spikes.h"

namespace {

// The setup screens' colours: green for +, red for -, white for the wheel
const float GREEN[3] = {0, 255, 60};
const float RED[3]   = {255, 0, 0};
const float WHITE[3] = {180, 180, 200};
const float AMBER[3] = {255, 110, 0};

const uint32_t FRAME_MS = 40;         // 25 frames per second, like Face
const float    SECONDS_PER_FRAME = FRAME_MS / 1000.0f;

// Where the sensor dots go on the top row, for 1, 2 or 3 sensors (as on the setup screens)
const int DOT_COLS[4][3] = {{0, 0, 0}, {2, 0, 0}, {1, 3, 0}, {0, 2, 4}};

const float LANE_MIN_RATE = 0.5f;     // spikes per second with nothing to feel
const float LANE_MAX_RATE = 10.0f;    // ... and feeling as much as it can
const float WHEEL_MAX_RATE = 10.0f;   // wheel spikes per second at full speed
const float TAIL = 2.5f;              // a spike's tail, in rows
const int   LANE_MAX_AGE = 6;         // frames: 3 rows to travel, plus the tail
const float WHEEL_FADE = 3.0f;        // frames for a wheel spike to fade
const int   WHEEL_MAX_AGE = 6;        // frames: spreads 2 LEDs each way, plus the fade

// One frame being built: colour (0..255) times intensity (0..1), brightest wins
float frame[25][3];

void clearFrame() {
  for (int n = 0; n < 25; n++) {
    frame[n][0] = 0;
    frame[n][1] = 0;
    frame[n][2] = 0;
  }
}

void add(int row, int col, const float* rgb, float intensity) {
  if (intensity <= 0) {
    return;
  }
  if (intensity > 1) {
    intensity = 1;
  }
  // Squared, like Face::push(), so dim and bright look as different as they should
  float s = intensity * intensity;
  float* p = frame[row * 5 + col];
  for (int k = 0; k < 3; k++) {
    float v = rgb[k] * s;
    if (v > p[k]) {
      p[k] = v;
    }
  }
}

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
  _lastDrawMs = 0;
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
  const float* rgb = (wheel.speed >= 0) ? WHITE : AMBER;
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
        add(4, col, rgb, 1.0f - behind / WHEEL_FADE);
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
  uint32_t now = millis();
  if (_lastDrawMs != 0 && now - _lastDrawMs < FRAME_MS) {
    return false;
  }
  _lastDrawMs = now ? now : 1;

  clearFrame();

  // ---- The lanes: one per sensor ----
  for (int j = 0; j < _count; j++) {
    Lane& lane = _lane[j];
    const float* rgb = (lane.weight >= 0) ? GREEN : RED;
    int col = DOT_COLS[_count][j];

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
    add(0, col, rgb, 0.4f + 0.6f * lane.level);

    // Each spike's head is one row further down every frame, with a fading tail
    for (int k = 0; k < MAX_AGES; k++) {
      if (lane.ages[k] < 0) {
        continue;
      }
      for (int row = 1; row <= 3; row++) {
        float behind = lane.ages[k] - (row - 1);   // how far the head has passed this row
        if (behind >= 0 && behind < TAIL) {
          add(row, col, rgb, 1.0f - behind / TAIL);
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

  for (int n = 0; n < 25; n++) {
    _leds[n] = CRGB((uint8_t)frame[n][0], (uint8_t)frame[n][1], (uint8_t)frame[n][2]);
  }
  FastLED.show();
  return true;
}
