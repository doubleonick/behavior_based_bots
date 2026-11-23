/*
  PulseDrive - moves the wheels in beats instead of at a steady speed. A TRIAL.

  The regular drive: the wiring asks for a speed, and the wheel turns at that speed.
  The pulse drive: the wiring sets how OFTEN the wheels push, like a jellyfish's bell
  pulsing faster or slower with what it feels. Each push is the same. More felt, more
  pushes a second; nothing felt, no pushes.

  It sits between the wiring and the wheels, so it works the same in every sketch,
  with one wheel or two:
    - The beat rate follows the STRONGEST wheel's amount (0..1, either direction):
      full strength gives BEATS_AT_FULL beats a second.
    - During a beat every wheel gets its own amount, scaled up so the strongest
      wheel pushes at full. So with two wheels Val still heads the same way as with
      the regular drive (the same turn), only in beats.
    - With the ON_OFF shape the average speed equals the regular drive's. PUSH_GLIDE
      (a quick push, then easing off) is gentler but slower on average (about 0.4 of
      the regular drive), because a wheel can't push harder than full.
  MotionServo's own easing softens every beat a little (about a tenth of a second),
  so ON_OFF already looks like a push and a short glide on the robot.

  Usage, once per loop, before the wheels' drive():
    PulseDrive pulse;
    pulse.setEnabled(true);
    pulse.step(strongest);              // strongest = the largest |amount| of any wheel
    servo.drive(pulse.apply(amount));   // for each wheel

  With the pulse drive off, apply() hands the amount back unchanged.
*/
#pragma once
#include <Arduino.h>

class PulseDrive {
public:
  enum Shape : uint8_t { ON_OFF, PUSH_GLIDE };

  void setEnabled(bool on) { _enabled = on; }
  bool enabled() const { return _enabled; }
  void setBeatsAtFull(float beats);   // beats a second when the wiring asks for full speed
  void setShape(Shape s) { _shape = s; }
  static const char* shapeName(Shape s);

  void  step(float strongest);        // call once per loop
  float apply(float amount) const;    // the amount to send to a wheel right now
  bool  inBeat() const { return _beatAge >= 0; }

private:
  bool     _enabled = false;
  float    _beatsAtFull = 1.6f;
  Shape    _shape = ON_OFF;
  float    _phase = 0.999f;           // just short of a beat, so the first push comes at once
  float    _beatAge = -1;             // seconds since this beat began; -1 = between beats
  float    _gain = 1;                 // what apply() multiplies by now
  uint32_t _lastMs = 0;
  bool     _started = false;

  float shapeAt(float share) const;   // a beat's strength, share = 0..1 through the beat
};
