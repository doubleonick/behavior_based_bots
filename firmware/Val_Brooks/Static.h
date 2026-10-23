/*
  Static - white "TV static" for the 5x5 matrix, shared by the setup screens and Face.

  Going into setup, the screen shows half a second of static. Coming out, Face's
  tuneIn() starts with static that settles LED by LED into the face.

  The static is "random" but repeatable: frame 7 always looks the same. That keeps
  it cheap (no random generator to look after) and lets the tune-in decide in
  advance when each LED settles.
*/
#pragma once
#include <Arduino.h>

namespace Static {

const uint32_t FRAME_MS = 40;          // a new pattern every 40 ms
const float DENSITY = 0.55f;           // about how many LEDs are lit in each frame (0..1)
const uint8_t COLOR[3] = {200, 200, 220};   // white, a touch blue like the setup screens

// A number from 0 to 1 that looks random but is always the same for the same a and b
inline float random01(uint32_t a, uint32_t b) {
  uint32_t h = a * 374761393UL + b * 668265263UL;
  h = (h ^ (h >> 13)) * 1274126177UL;
  h = h ^ (h >> 16);
  return (h & 0xFFFF) / 65535.0f;
}

// Brightness of LED k (0..24) in static frame number `frame`: 0 = off, else 0.2 to 0.9
inline float level(uint32_t frame, int k) {
  if (random01(frame, k) >= DENSITY) {
    return 0;
  }
  return 0.2f + 0.7f * random01(frame + 7919, k);
}

// Tune-in: the moment LED k settles, as a fraction of the tune-in (0.3 to 0.9)
inline float settleAt(int k) {
  return 0.3f + 0.6f * random01(4242, k);
}

}
