/*
  KeySensor - the M5 Key Unit: one button, and one colour LED (SK6812) in it.

  Not an I2C sensor, so it cannot say what it is. Val assumes one is in Port C when no
  I2C sensor answers there, and the setup screens let you switch it off.

  Raw reading: 1 while the key is pressed, 0 when it isn't. That is already the 0..1
  stimulus, so setup skips calibration (needsCalibration() is false).

  Which of the port's two pins is the button and which is the LED is found the first
  time the key is pressed: the button pin is the one that goes LOW. Until then the key
  reads as not pressed, and the LED stays off.

  The LED shows a colour chosen by the sketch (for example, its wire's colour), dim
  normally and bright while the key is pressed. It is shown with the matrix (every
  FastLED.show()).

  Needs: FastLED.
*/
#pragma once
#include <Arduino.h>
#include <FastLED.h>
#include "Sensor.h"
#include "Canvas.h"

class KeySensor : public Sensor {
public:
  explicit KeySensor(const Port& port) : Sensor(port) {}

  bool begin() override;
  char typeCode() const override { return 'K'; }
  const char* typeName() const override { return "Key"; }
  float meterLevel() const override { return (float)raw(); }
  bool needsCalibration() const override { return false; }

  bool pressed() const { return isConnected() && raw() == 1; }
  bool pinsKnown() const { return _buttonPin >= 0; }   // pressed at least once

  void reclaimPins();                 // after an I2C search used the port: get the pins back

  // The LED: colour (nullptr = off). Dim normally, bright while pressed.
  void showColour(const Canvas::Color* colour);

protected:
  bool readRaw(long& out) override;
  uint32_t readIntervalMs() const override { return 10; }
  long fullScale() const override { return 1; }
  long defaultLow() const override { return 0; }
  long defaultHigh() const override { return 1; }

private:
  int  _buttonPin = -1;               // found on the first press
  int  _ledPin = -1;
  CRGB _led[1];
  bool _ledAdded = false;

  void addLed();
};
