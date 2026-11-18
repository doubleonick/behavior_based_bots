/*
  SetupMode - the one-button setup screens on the 5x5 matrix.

    0. Half a second of white static (see Static.h).
    1. Mount: an arrow shows "forward" and the wheel turns slowly forward.
       Short press flips left/right (the wheel reverses). Long press confirms.
    2. For each sensor that is plugged in:
         channel (a Color Unit only): what it feels. A white L = light (any colour),
                 a block in magenta, yellow or cyan = that colour. Short press
                 changes, long press confirms. The wire's dot on the top row takes
                 the colour too.
         sign:   + (green) excitatory, - (red) inhibitory, x (grey) not connected.
                 Short press changes, long press confirms.
         low:    no stimulus, long press.     light: moon, cover the sensor  } skipped
                                              distance: small square, far   } when the
         high:   full stimulus, long press.   light: sun, shine light       } wire
                                              distance: big square, near    } is "x"
         (reflective IR uses the distance squares; a key skips low and high: it is
          only pressed or not)
       The top row has one dot per wire; confirmed wires show their colour.
    (No sensor found: a grey box with a cross for 2 seconds, so it is clear that
     nothing was there to set up. Check the port, or the PORT_A_UNIT / 
     ASSUME_KEY_ON_PORT_C settings for units that can't say what they are.)
    3. A check mark, then setup ends (the sketch then tunes the face back in).
  The right-hand column is a live meter of the sensor's stimulus.
  When only a newly plugged sensor needs wiring, step 1 (mount) is skipped
  and only the new sensor's wire is shown.

  While setup is running it owns the LEDs; the Face is not drawn.
*/
#pragma once
#include <Arduino.h>
#include <FastLED.h>
#include "Canvas.h"
#include "Button.h"
#include "MotionServo.h"
#include "Connection.h"
#include "ColorSensor.h"

class SetupMode {
public:
  SetupMode(CRGB* leds, MotionServo& servo);

  // wires = the connections to set up (up to 3).
  // withMount = true for a full setup; false when only new sensors need wiring.
  void start(Connection** wires, int count, bool withMount);
  void handle(Button::Event e);
  void update();                       // call every loop while active()

  bool active() const { return _phase != OFF; }
  bool takeFinished();                 // true once, right after setup ends

private:
  enum Phase : uint8_t { OFF, ENTER, MOUNT, NO_SENSOR, CHANNEL, SIGN, LOW_POINT, HIGH_POINT, DONE };

  CRGB*        _leds;
  MotionServo& _servo;
  Phase        _phase = OFF;
  uint32_t     _phaseMs = 0;
  FrameTimer   _timer;
  Connection*  _wires[3];
  int          _count = 0;
  int          _wire = 0;
  bool         _finished = false;
  bool         _withMount = true;
  Canvas       _canvas;

  void goTo(Phase p);
  void nextWire();
  void startWire();                    // a Color Unit's channel first, else straight to the sign
  ColorSensor* colorUnit();            // this wire's Color Unit, or nullptr

  void drawDots(float t);
  void drawMeter(float level);
  void draw();
};
