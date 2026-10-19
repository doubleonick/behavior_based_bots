/*
  MotionServo - one continuous-rotation servo on the M5 Atom Motion base.

  The Atom talks to the Motion base over I2C (address 0x38). The Motion's own chip
  makes the servo pulses, so the sketch only sends it an angle (0-180).
  On a continuous-rotation servo the angle means speed:
     90 = stopped, one end = full speed one way, the other end = full speed the other way.

  Mounting: a wheel mounted on the LEFT side of a vehicle turns forward toward 180.
  A wheel mounted on the RIGHT side faces the other way, so forward is toward 0.

  Usage:
    MotionServo servo(1);          // servo plugged into S1
    servo.begin();
    servo.drive(0.5);              // half speed forward (0 = stop, 1 = full forward)
    servo.update();                // call every loop
*/
#pragma once
#include <Arduino.h>
#include <Wire.h>

class MotionServo {
public:
  enum Mount : uint8_t { LEFT, RIGHT };

  explicit MotionServo(uint8_t channel = 1);   // 1 to 4 = S1 to S4

  bool begin();                       // find the Motion base; true if found

  void setMount(Mount m);
  Mount mount() const { return _mount; }
  static const char* mountName(Mount m);

  void setAllowReverse(bool allow);   // off: negative drive means "stopped"
  void setStopAngle(int degrees);     // the angle where the wheel holds still (normally 90)

  void drive(float amount);           // -1..1, positive = forward
  void stopNow();                     // stop immediately, without easing
  void update();                      // call every loop: eases toward the drive and sends it

  int angle() const { return _lastAngle; }

private:
  uint8_t  _channel;
  Mount    _mount = LEFT;
  bool     _allowReverse = false;
  int      _stopAngle = 90;
  float    _target = 0;               // requested drive
  float    _smooth = 0;               // eased drive actually sent
  int      _lastAngle = -1;
  uint32_t _lastUpdateMs = 0;

  int  angleFor(float amount) const;
  void sendAngle(int degrees);
};
