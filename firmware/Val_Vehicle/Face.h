/*
  Face - animated expressions for a 5x5 LED matrix (M5 Atom Matrix).

  Usage:
    CRGB leds[25];
    Face face(leds);
    face.sayHello();          // or face.happy(), face.asleep(), face.show(Face::LOVE) ...
    face.tuneIn(Face::HAPPY); // static that settles into the face (used after setup)
    void loop() { face.update(); }   // call as often as possible; it redraws ~25x/second

  LED layout expected: index = row * 5 + col, row 0 at the top.
  Face::update() calls FastLED.show() itself whenever it draws a new frame.
*/
#pragma once
#include <Arduino.h>
#include <FastLED.h>

class Face {
public:
  enum Expression : uint8_t {
    NEUTRAL, HAPPY, SAD, SURPRISED, ANGRY, LAUGHING, WINK, UNSURE, ASLEEP, LOVE, TALKING
  };

  // Mouth shapes used for talking
  enum Mouth : uint8_t {
    MOUTH_CLOSED,     // M, B, P and pauses
    MOUTH_HALF,       // most consonants
    MOUTH_WIDE,       // "ah"
    MOUTH_STRETCHED,  // "ee"
    MOUTH_ROUND,      // "oh"
    MOUTH_SMALL       // "oo", W
  };

  explicit Face(CRGB* leds);

  // ---- Expressions: each call switches to that expression and restarts its animation ----
  void neutral();    // blinks, glances left and right
  void happy();      // smile, occasional blink
  void sad();        // frown, occasional double blink
  void surprised();  // neutral -> "O" mouth, bright pixel laps the O, flash, back to neutral (repeats)
  void angry();      // eyes turn from blue to red, mouth shimmers
  void laughing();   // happy -> laughing with shimmering mouth -> happy (repeats)
  void wink();       // neutral -> one eye closed + smirk -> neutral (repeats)
  void unsure();     // crooked mouth, sideways glance
  void asleep();     // face slowly rises and falls, snores at the top
  void love();       // pulsing heart
  void show(Expression e);

  // ---- Tune in ----
  // 0.6 s of white static that settles LED by LED into expression e, then e plays as usual.
  // expression() already reports e while it tunes in.
  void tuneIn(Expression e);
  bool isTuningIn() const { return _tuning; }

  // ---- Talking ----
  // Play a sequence of mouth shapes, one every frameMs, then switch to `then`.
  // The `shapes` array must stay valid while it plays (make it static/global).
  void say(const Mouth* shapes, uint8_t count, uint16_t frameMs = 150, Expression then = NEUTRAL);
  void sayHello();   // "Hello, my name is, Val"
  bool isTalking() const { return _expr == TALKING; }

  // ---- Call every loop ----
  bool update();     // returns true when a new frame was drawn

  Expression expression() const { return _expr; }
  static const char* name(Expression e);

  struct Color { float r, g, b; };
  struct Pixel { Color c; float i; };   // i = intensity 0..1 (0 = off)

private:
  CRGB*      _leds;
  Expression _expr = NEUTRAL;
  uint32_t   _startMs = 0;      // when the current expression began
  uint32_t   _lastDrawMs = 0;
  Pixel      _frame[25];
  bool       _tuning = false;   // tuning in to _expr

  const Mouth* _sayShapes = nullptr;
  uint8_t      _sayCount = 0;
  uint16_t     _sayFrameMs = 150;
  Expression   _sayThen = NEUTRAL;

  void drawExpression(Expression e, float t, uint32_t elapsedMs);
  void addStatic(float progress, uint32_t elapsedMs);   // the tune-in's static, over the face

  // One renderer per expression. t = seconds since the expression started.
  void drawNeutral(float t);
  void drawHappy(float t);
  void drawSad(float t);
  void drawSurprised(float t);
  void drawAngry(float t);
  void drawLaughing(float t);
  void drawWink(float t);
  void drawUnsure(float t);
  void drawAsleep(float t);
  void drawLove(float t);
  void drawTalking(uint32_t elapsedMs);

  void drawEyesAndMouth(const char* const mouthRows[5], float drift, float eyelids);
  void push();   // copy _frame to the LEDs
};
