/*
  ColorSensor - the M5 Color Unit (TCS3472).

  It measures light through four filters: red, green, blue and "clear" (all of it).
  It can be used for one of four CHANNELS:

    LIGHT     how bright it is, any colour (the "clear" filter). The default.
    MAGENTA   how much magenta it sees
    YELLOW    how much yellow
    CYAN      how much cyan

  Raw reading: LIGHT uses the clear filter, 0 to 65535. A colour channel uses the
  brightest of red, green and blue, so a coloured light counts as bright as a white
  one of the same strength. Either way the low and high points are set on this
  brightness, as for any light sensor.

  Stimulus (value): LIGHT is the brightness between the low and high points.
  A colour channel is that, times how much the colour seen is like the channel's:

    how close its hue is   1 at the channel's own hue, falling to 0 at `width`
                           degrees away (fuzzy: red light counts a little as magenta
                           and a little as yellow)
    how saturated it is    white or grey light has no colour, so 0

  The hues and the width can be tuned with setHues(). Hue: red 0, yellow 60,
  green 120, cyan 180, blue 240, magenta 300 (degrees round the colour wheel).

  Setup icons: moon (dark) and sun (bright), as for any light sensor.
*/
#pragma once
#include <Arduino.h>
#include "Sensor.h"

class ColorSensor : public Sensor {
public:
  enum Channel : uint8_t { LIGHT, MAGENTA, YELLOW, CYAN };
  static const int CHANNEL_COUNT = 4;

  explicit ColorSensor(const Port& port) : Sensor(port) {}

  bool begin() override;
  char typeCode() const override { return 'C'; }
  const char* typeName() const override { return channelName(_channel); }
  float meterLevel() const override;
  float value() const override;

  void setChannel(Channel c) { _channel = c; }
  Channel channel() const { return _channel; }
  static const char* channelName(Channel c);

  // Where each colour channel sits on the colour wheel, and how many degrees away
  // still count (shared by every Color Unit)
  static void setHues(float magenta, float yellow, float cyan, float width);

  // How much what it sees now is like a colour channel's colour: 0..1 (1 for LIGHT)
  float colourMatch() const;

protected:
  bool readRaw(long& out) override;
  uint32_t readIntervalMs() const override { return 160; }   // one measurement every ~154 ms
  long fullScale() const override { return 65535; }
  long defaultLow() const override { return 0; }
  long defaultHigh() const override { return 65535; }

private:
  int _sda = -1;
  int _scl = -1;
  Channel _channel = LIGHT;
  long _red = 0;
  long _green = 0;
  long _blue = 0;

  static float s_hue[CHANNEL_COUNT];
  static float s_width;

  void writeRegister(uint8_t reg, uint8_t value);
};
