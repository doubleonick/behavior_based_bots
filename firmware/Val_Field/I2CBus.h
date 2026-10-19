/*
  I2CBus - the Atom's second I2C bus (Wire1), shared by every plug-in sensor.

  Sensors on different ports take turns: just before a sensor is used, the bus
  is pointed at that sensor's two pins. This also lets two sensors with the same
  address (for example two Color Units) be used at once, one per port.

  It also tells apart the two sensors that share address 0x29, by reading the ID
  each chip reports about itself.
*/
#pragma once
#include <Arduino.h>
#include <Wire.h>

namespace I2CBus {

const uint8_t SHARED_ADDRESS = 0x29;   // both the Color Unit and the ToF (distance) Unit use this

void use(int sda, int scl);            // point Wire1 at these pins
void release();                        // let go of the pins (so a 3-pin sensor can use them)
bool answers(uint8_t address);         // is anything at this address on the current pins?
bool readByte(uint8_t address, uint8_t command, uint8_t& value);

bool isDistanceChip();                 // VL53L0X: reports 0xEE and 0xAA at registers 0xC0 and 0xC1
bool isColorChip();                    // TCS3472: reports 0x44, 0x4D or 0x10 at its ID register

}
