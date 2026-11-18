/*
  FieldSetup - the one-button setup screens for the potential-field robot.

    0. Half a second of white static (see Static.h).
    1. Wheels. S1 is the LEFT wheel, S2 the RIGHT wheel.
         S1 turns slowly "forward" (amber dot bottom-left).
           Wrong way? Short press flips it. Long press confirms.
         Then the same for S2 (amber dot bottom-right).
    2. For each sensor that is plugged in:
         policy: + (green) pull / attract,  - (red) push / repel,  x (grey) off.
                 Short press changes, long press confirms.
         angle:  an arrow shows which way the sensor points on the robot.
                 Short press turns it 45 degrees anticlockwise, long press confirms.
         low:    no stimulus, long press.    light: moon (cover it)   distance: small square (far)
         high:   full stimulus, long press.  light: sun (light it)    distance: big square (near)
         (angle, low and high are skipped when the policy is "off"; reflective IR uses
          the distance squares; a key skips low and high: it is only pressed or not)
       A Color Unit is one sensor with up to four wires: its light wire first (as
       above, except that angle, low and high are always asked, because the colour
       wires share them), then a magenta, a yellow and a cyan wire. Each colour wire
       only asks pull / push / off (it starts on off); while it is asked, the dot on
       the top row blinks in its colour and the meter on the right shows how much of
       that colour the sensor feels. The display has 3 lanes, so a colour wire is
       only offered while fewer than 3 wires are on (keeping a place for each sensor
       still to come); the rest are skipped and stay off.
       The top row has one dot per sensor; confirmed sensors show their colour.
    3. A check mark, then setup ends (the sketch then tunes the face back in).

  When only a newly plugged sensor needs setting up, the wheel steps are skipped.
  While setup is running it owns the LEDs and both servos.
*/
#pragma once
#include <Arduino.h>
#include <FastLED.h>
#include "Canvas.h"
#include "Button.h"
#include "MotionServo.h"
#include "PotentialField.h"

class FieldSetup {
public:
  FieldSetup(CRGB* leds, MotionServo& leftWheel, MotionServo& rightWheel);

  // sources = the field sources to set up: up to 3 sensors, each followed by its
  // colour wires if it is a Color Unit (channel 1-3).
  // withWheels = true for a full setup; false when only new sensors need setting up.
  // othersOn = wires already on that are not being set up (they take lanes too).
  static constexpr int MAX_ENTRIES = 12;
  static constexpr int MAX_LANES = 3;
  void start(PotentialField::Source** sources, int count, bool withWheels, int othersOn = 0);
  void handle(Button::Event e);
  void update();                       // call every loop while active()

  bool active() const { return _phase != OFF; }
  bool takeFinished();                 // true once, right after setup ends

  // Angle steps: 0..7, each 45 degrees anticlockwise from forward
  static float angleFromStep(int step);
  static int   stepFromAngle(float angle);

private:
  enum Phase : uint8_t { OFF, ENTER, WHEEL_LEFT, WHEEL_RIGHT, POLICY, ANGLE, LOW_POINT, HIGH_POINT, DONE };

  CRGB*        _leds;
  MotionServo& _left;
  MotionServo& _right;
  Phase        _phase = OFF;
  uint32_t     _phaseMs = 0;
  FrameTimer   _timer;
  PotentialField::Source* _sources[MAX_ENTRIES];
  int          _count = 0;
  int          _dotOf[MAX_ENTRIES];    // which top-row dot (which sensor) each entry belongs to
  int          _dots = 0;              // how many sensors
  int          _othersOn = 0;
  int          _current = 0;
  bool         _finished = false;
  bool         _withWheels = true;
  Canvas       _canvas;

  void goTo(Phase p);
  void nextSource();
  bool isColour(int entry) const { return _sources[entry]->channel != 0; }
  bool roomForColour() const;          // may the current colour wire be offered?
  void enterSource();                  // go to the current entry, skipping colours with no room
  void flip(MotionServo& servo);

  void drawDots(float t);
  void drawMeter(float level, const Canvas::Color& rgb);
  void drawArrow(int step, const Canvas::Color& rgb, float intensity);
  void draw();
};
