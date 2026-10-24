/*
  Layers - which behaviour is in control, shown on the 5x5 matrix.

  One row per behaviour, top row = top of the hierarchy (it wins when it wants control).
    column 0      red = push / avoid, green = pull / approach
    column 1      dark (a gap)
    columns 2-4   the behaviour's colour:
                    collision = white, distance = purple, light = yellow, wander = blue
  A row that is NOT in control stays steady. The row that IS in control sends spikes
  along it, left to right, as often as the wheels' average speed says.

  Usage:
    CRGB leds[25];
    Layers layers(leds);
    layers.setRows(2);
    layers.setRow(0, &Palette::RED, &Palette::WHITE);   // escape collision
    layers.setRow(1, &Palette::GREEN, &Palette::BLUE);  // wander, going forward
    layers.setInControl(1);
    layers.setWheels(0.5f, 0.5f);
    void loop() { layers.update(); }                // call as often as possible

  LED layout expected: index = row * 5 + col, row 0 at the top (same as Face).
  update() calls FastLED.show() itself whenever it draws a new frame.
*/
#pragma once
#include <Arduino.h>
#include <FastLED.h>
#include "Canvas.h"
#include "Palette.h"

class Layers {
public:
  static const int MAX_ROWS = 5;

  explicit Layers(CRGB* leds);

  void start();                                   // forget old spikes and start drawing afresh
  void setRows(int count);                        // 0..5 behaviours
  void setRow(int row, const Canvas::Color* sign, const Canvas::Color* colour);   // sign may be nullptr
  void setInControl(int row);                     // -1 = none
  void setWheels(float left, float right);        // -1..1 each; sets the spike rate

  bool update();                                  // returns true when a new frame was drawn

private:
  static const int MAX_AGES = 4;                  // spikes in flight along the row

  CRGB*          _leds;
  int            _count = 0;
  const Canvas::Color* _sign[MAX_ROWS];
  const Canvas::Color* _colour[MAX_ROWS];
  int            _inControl = -1;
  float          _rate = 0;                        // spikes per second
  float          _phase = 0;
  int            _ages[MAX_AGES];                  // frames since each spike left (-1 = none)
  FrameTimer     _timer;
  Canvas         _canvas;
};
