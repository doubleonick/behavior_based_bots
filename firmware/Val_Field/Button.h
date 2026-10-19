/*
  Button - the Atom Matrix's one programmable button (press the screen, G39).

  update() reports:
    SHORT_PRESS  released after less than LONG_MS
    LONG_PRESS   released after LONG_MS or more
    HOLD         still held after HOLD_MS (only when hold is enabled);
                 the release that follows reports nothing
*/
#pragma once
#include <Arduino.h>

class Button {
public:
  enum Event : uint8_t { NONE, SHORT_PRESS, LONG_PRESS, HOLD };

  explicit Button(int pin) : _pin(pin) {}

  void begin() {
    pinMode(_pin, INPUT);
  }

  Event update(bool holdEnabled) {
    const uint32_t DEBOUNCE_MS = 30;
    const uint32_t LONG_MS = 700;
    const uint32_t HOLD_MS = 2000;

    bool down = digitalRead(_pin) == LOW;
    uint32_t now = millis();

    if (down != _down && now - _changedMs > DEBOUNCE_MS) {
      _down = down;
      _changedMs = now;
      if (_down) {
        _pressedMs = now;
        _holdFired = false;
      } else {
        if (_holdFired) {
          return NONE;
        }
        if (now - _pressedMs < LONG_MS) {
          return SHORT_PRESS;
        }
        return LONG_PRESS;
      }
    }

    if (_down && holdEnabled && !_holdFired && now - _pressedMs >= HOLD_MS) {
      _holdFired = true;
      return HOLD;
    }
    return NONE;
  }

private:
  int      _pin;
  bool     _down = false;
  bool     _holdFired = false;
  uint32_t _changedMs = 0;
  uint32_t _pressedMs = 0;
};
