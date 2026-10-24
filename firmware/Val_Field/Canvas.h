/*
  Canvas - one picture for the 5x5 matrix, and how it is sent to the LEDs.

  Everything that draws on the matrix (Face, Spikes, the setup screens, ...) draws
  on a Canvas, so they all look the same: the same "brightest wins" rule, the same
  brightness curve, the same 25 frames a second.

  A pixel is a colour (0..255 for red, green, blue) and an intensity (0..1, 0 = off).
  When two things are drawn on the same pixel, the brighter one wins.
  push() squares the intensity on the way to the LEDs, so the difference between
  normal (0.6) and full (1.0) is easy to see on real LEDs.

  Usage:
    CRGB leds[25];
    Canvas canvas;
    FrameTimer timer;
    if (timer.due()) {                       // 25 times a second
      canvas.clear();
      canvas.put(2, 2, Palette::GREEN, 0.6f);
      canvas.push(leds);
    }

  LED layout: index = row * 5 + col, row 0 at the top.
*/
#pragma once
#include <Arduino.h>
#include <FastLED.h>

class Canvas {
public:
  struct Color { float r, g, b; };          // 0..255 each
  struct Pixel { Color c; float i; };       // i = intensity 0..1 (0 = off)

  Pixel px[25];

  Canvas() { clear(); }

  void clear();

  // Light one pixel (brightest wins). Off the edge, or intensity 0: nothing happens.
  void put(int row, int col, const Color& c, float intensity);

  // Light a "pixel" at a fractional position by sharing it between the neighbouring
  // LEDs. This is what makes eyes slide instead of jump.
  void putSoft(float row, float col, const Color& c, float intensity);

  // A 5-row picture: every `mark` character is lit, e.g. {".....", "..#..", ...}
  void putRows(const char* const rows[5], char mark, const Color& c, float intensity);

  // Cross-fade between two pictures: k = 0 gives a, k = 1 gives b
  void blend(const Canvas& a, const Canvas& b, float k);

  // Send the picture to the LEDs (intensity squared) and show it
  void push(CRGB* leds) const;
};

// "Is it time for the next frame?" 25 frames a second, like every matrix display.
class FrameTimer {
public:
  static const uint32_t FRAME_MS = 40;

  bool due();                               // true at most once every FRAME_MS
  void restart() { _lastMs = 0; }           // the next due() is true straight away

private:
  uint32_t _lastMs = 0;
};
