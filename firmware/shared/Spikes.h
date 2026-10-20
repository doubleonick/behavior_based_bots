/*
  Spikes - what the wiring is doing, shown on the 5x5 matrix while Val drives.

  Each sensor is a "lane": its dot sits on the top row (in the same spot as on the
  setup screens), and it sends spikes down its own column, faster the more it feels.
  Green spikes are + wires, red spikes are - wires.
  The bottom row is the wheel. It fires its own white spikes, spreading out from the
  middle, at the speed the wheel is actually being driven.

  Like a real nerve, a lane never goes quite silent: even with nothing to feel it
  sends a slow spike now and then.

  Usage:
    CRGB leds[25];
    Spikes spikes(leds);
    spikes.start();                         // clear, when switching to spikes
    spikes.setLanes(2);                     // how many sensors are wired
    spikes.setLane(0, 0.7f, +1);            // lane 0 feels 0.7, + wire
    spikes.setLane(1, 0.2f, -1);            // lane 1 feels 0.2, - wire
    spikes.setWheel(0.5f);                  // wheel driven at half speed
    void loop() { spikes.update(); }        // call as often as possible; redraws ~25x/second

  LED layout expected: index = row * 5 + col, row 0 at the top (same as Face).
  update() calls FastLED.show() itself whenever it draws a new frame.
*/
#pragma once
#include <Arduino.h>
#include <FastLED.h>

class Spikes {
public:
  static const int MAX_LANES = 3;

  explicit Spikes(CRGB* leds);

  void start();                                  // forget old spikes and start drawing afresh
  void setLanes(int count);                      // 0..3 sensors
  void setLane(int lane, float level, float weight);   // level 0..1, weight +1 or -1
  void setWheel(float speed);                    // 0..1 (the size of the wheel's speed)

  bool update();                                 // returns true when a new frame was drawn

private:
  static const int MAX_AGES = 4;                 // spikes in flight per lane

  struct Lane {
    float level = 0;
    float weight = 1;
    float phase = 0;                             // counts up; a spike leaves at each whole number
    int   ages[MAX_AGES];                        // frames since each spike left (-1 = none)
  };

  CRGB*    _leds;
  Lane     _lane[MAX_LANES];
  int      _count = 0;
  float    _wheel = 0;
  float    _wheelPhase = 0;
  int      _wheelAges[MAX_AGES];
  uint32_t _lastDrawMs = 0;

  void clearAges(int* ages);
  void addAge(int* ages);
  void ageAll(int* ages, int maxAge);
};
