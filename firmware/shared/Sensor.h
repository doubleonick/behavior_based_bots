/*
  Sensor - the parent class for every sensor a vehicle can be wired to.

  Every sensor gives the same thing to the rest of the program: value(), a number
  from 0 (no stimulus) to 1 (full stimulus). How a sensor measures is up to the
  child class (ColorSensor, DistanceSensor, ...). Turning the measurement into
  0..1 happens here, using two calibration points taken during setup:
     low  = the raw reading that should count as 0 (no stimulus)
     high = the raw reading that should count as 1 (full stimulus)
  The conversion is a straight line between them, so it works whichever way the
  raw numbers run (for distance, "high stimulus" is the SMALLER number: near).
  Curves belong to the wires, not the sensors.

  A child class must provide:
     begin()          look for its chip on the port, set it up, return true if found
     readRaw(out)     take one reading; return false if the sensor did not answer
     readIntervalMs() how often a new reading is available
     fullScale()      the largest raw reading possible
     defaultLow(), defaultHigh()   calibration points to use before setup
     meterLevel()     0..1 for the setup screen's meter (0 = no stimulus)
     typeCode(), typeName()        e.g. 'C' "light", 'D' "distance"
  and may override:
     needsCalibration()  false for an on/off sensor (setup then skips low/high)
*/
#pragma once
#include <Arduino.h>
#include "Port.h"

class Sensor {
public:
  explicit Sensor(const Port& port) : _port(port) {}
  virtual ~Sensor() {}

  virtual bool begin() = 0;
  virtual char typeCode() const = 0;
  // false for sensors that are only on or off (a key): setup skips their low/high points
  virtual bool needsCalibration() const { return true; }
  virtual const char* typeName() const = 0;
  virtual float meterLevel() const = 0;

  const char* portName() const { return _port.name; }
  void setPort(const Port& port) { _port = port; }   // e.g. once the PaHub has been found

  bool  update();                     // call every loop; true when a new reading arrived
  bool  isConnected() const { return _connected; }
  long  raw() const { return _raw; }
  float value() const;                // 0..1 between the calibration points

  // virtual so a child class can do more when it is calibrated
  virtual void calibrateLow()  { _low = _raw; }
  virtual void calibrateHigh() { _high = _raw; }
  void  setCalibration(long low, long high);
  void  clearCalibration();           // go back to defaultLow() / defaultHigh()
  long  lowPoint() const;
  long  highPoint() const;

  bool  takeChange();                 // true once after a noticeable change (counts as activity)

protected:
  virtual bool readRaw(long& out) = 0;
  virtual uint32_t readIntervalMs() const = 0;
  virtual long fullScale() const = 0;
  virtual long defaultLow() const = 0;
  virtual long defaultHigh() const = 0;

  Port _port;
  bool _connected = false;

private:
  static const long NOT_SET = -1;

  long     _raw = 0;
  long     _low = NOT_SET;
  long     _high = NOT_SET;
  uint32_t _lastReadMs = 0;
  bool     _haveReference = false;
  float    _reference = 0;            // slowly-updated reading, used to spot changes
  bool     _changed = false;
};
