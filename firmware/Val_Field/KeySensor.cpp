#include "KeySensor.h"
#include "I2CBus.h"

namespace {
const float DIM = 0.6f;               // the LED when the key is not pressed (squared, like the matrix)
const float BRIGHT = 1.0f;            // while pressed
}

// Nothing to ask the unit: just get both pins ready to be read
bool KeySensor::begin() {
  reclaimPins();
  _buttonPin = -1;
  _ledPin = -1;
  _connected = true;
  Serial.printf("Key assumed on %s (press it once so Val knows which pin is the button)\n", _port.name);
  return true;
}

void KeySensor::reclaimPins() {
  if (_ledAdded) {
    return;                                   // the LED pin belongs to FastLED now
  }
  I2CBus::releaseIfUsing(_port.pin1);         // the I2C search may have been using them
  I2CBus::releaseIfUsing(_port.pin2);
  pinMode(_port.pin1, INPUT_PULLUP);
  pinMode(_port.pin2, INPUT_PULLUP);
}

bool KeySensor::readRaw(long& out) {
  if (_buttonPin < 0) {
    // Not pressed yet: whichever pin goes LOW first is the button, the other is the LED
    if (digitalRead(_port.pin1) == LOW) {
      _buttonPin = _port.pin1;
      _ledPin = _port.pin2;
    } else if (digitalRead(_port.pin2) == LOW) {
      _buttonPin = _port.pin2;
      _ledPin = _port.pin1;
    } else {
      out = 0;
      return true;
    }
    Serial.printf("Key on %s: button on G%d, LED on G%d\n", _port.name, _buttonPin, _ledPin);
    addLed();
  }
  out = (digitalRead(_buttonPin) == LOW) ? 1 : 0;
  return true;
}

// FastLED needs the pin when it is compiled, so there is one line per possible pin
void KeySensor::addLed() {
  if (_ledAdded) {
    return;
  }
  _led[0] = CRGB::Black;
  if (_ledPin == 19) {
    FastLED.addLeds<SK6812, 19, GRB>(_led, 1);
    _ledAdded = true;
  } else if (_ledPin == 22) {
    FastLED.addLeds<SK6812, 22, GRB>(_led, 1);
    _ledAdded = true;
  } else if (_ledPin == 23) {
    FastLED.addLeds<SK6812, 23, GRB>(_led, 1);
    _ledAdded = true;
  } else if (_ledPin == 33) {
    FastLED.addLeds<SK6812, 33, GRB>(_led, 1);
    _ledAdded = true;
  } else if (_ledPin == 26) {
    FastLED.addLeds<SK6812, 26, GRB>(_led, 1);
    _ledAdded = true;
  } else if (_ledPin == 32) {
    FastLED.addLeds<SK6812, 32, GRB>(_led, 1);
    _ledAdded = true;
  }
}

void KeySensor::showColour(const Canvas::Color* colour) {
  if (!_ledAdded) {
    return;
  }
  if (colour == nullptr) {
    _led[0] = CRGB::Black;
    return;
  }
  float i = pressed() ? BRIGHT : DIM;
  float s = i * i;
  _led[0] = CRGB((uint8_t)(colour->r * s), (uint8_t)(colour->g * s), (uint8_t)(colour->b * s));
}
