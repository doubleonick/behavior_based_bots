# Hardware notes

What the firmware relies on, and what was learned the hard way. Facts marked
*(tested)* were confirmed on the real robot; the rest come from M5Stack's
documentation.

## Atom Matrix (ESP32-PICO)

| what | pin |
|---|---|
| 5x5 WS2812 LEDs (GRB, index = row * 5 + col, row 0 at the top) | G27 |
| Button (press the screen; LOW when pressed) | G39 |
| Internal I2C to the Motion base and the IMU: SDA / SCL | G25 / G21 |
| Grove Port A (on the Atom itself) | G26, G32 |

- Keep `FastLED.setBrightness` low (the sketches use 20): the matrix is very bright
  and gets warm.
- An old FastLED (the GitHub "FastLED-master" download) fails to compile for current
  ESP32 cores with `RMTMEM` / `gpio_matrix_out` errors. Remove it and install FastLED
  from the Library Manager. *(tested)*

## Atom Motion v1.2

The Motion's own STM32 microcontroller drives the servo and motor ports. The ESP32
only talks to it over the internal I2C bus (address **0x38**). The ESP32 has no
direct connection to the servo signal pins.

| register | meaning |
|---|---|
| 0x00–0x03 | servo S1–S4 **angle**, 1 byte, 0–180. This is what `MotionServo` uses. *(tested)* |
| 0x10, 0x12, 0x14, 0x16 | servo S1–S4 pulse width, 2 bytes, 500–2500 µs. Did not move our servo. *(tested)* |
| 0x20–0x21 | DC motor ports |

- Continuous-rotation servos: 90 = stop. `MotionServo` maps speed −1..1 to an angle,
  with a "mount" setting so "forward" is right for a wheel on either side
  (LEFT: forward is toward 180; RIGHT: forward is toward 0).
- **The servo headers can't be used as inputs.** Their signal pins belong to the STM32,
  and the register map has nothing to read back.
- Grove Port B: G23, G33. Grove Port C: G19, G22.
- Two-wheel body: with the servos at the rear of the chassis, Val_Field arced right
  when driving straight with no sensors; moving them to the middle of the chassis
  fixed it, with no change to the code. *(tested)*
- Power quirk: after the Motion has been off for a long time, its power switch
  sometimes has to be toggled twice before it starts. There is no software fix.
  *(tested)*

## Grove ports and what they can do

All three ports supply **5 V** on the red wire. ESP32 pins are **3.3 V only**.

| port | pins | read a voltage (ADC)? | notes |
|---|---|---|---|
| A (Atom) | G26, G32 | both | G26 is ADC2: it stops working while Wi-Fi is on. G32 (ADC1) always works. |
| B (Motion) | G23, G33 | G33 only | |
| C (Motion) | G19, G22 | neither | on/off signals only |

The sketches use a second I2C bus (`Wire1`) for the plug-in sensors and point it at a
port's pins just before each read (`I2CBus::use`). That lets one sketch serve three
ports, and lets two sensors with the same address work at once, one per port. It also
tries both wire orders, so SDA/SCL orientation never matters.

## Sensors the firmware recognises

Both answer at I2C address **0x29**, so the firmware asks each chip for its ID:

| sensor | chip | ID check | raw reading |
|---|---|---|---|
| M5 Color Unit | TCS3472 | register 0x92 (0x80 \| 0x12) reads 0x44, 0x4D or 0x10 | clear channel, 0–65535 (gain 4x, ~154 ms) |
| M5 ToF Unit | VL53L0X | registers 0xC0 / 0xC1 read 0xEE / 0xAA | distance in mm, capped at 2000 |

DFRobot's SEN0245 is the same VL53L0X chip, so it is recognised too.

Every sensor turns its raw reading into a 0..1 stimulus, a straight line between
two calibration points taken during setup: "low" (no stimulus) and "high" (full
stimulus). For distance, "high" is the *smaller* number (near).

## Considered and not used

- **DFRobot Gravity 3-pin sensors** (DFR0029 button, SEN0138 crash, DFR0034 sound).
  They need a voltage divider on Grove's 5 V, can't be told apart by the robot, and
  sound needs an ADC pin. Too fiddly for the audience. A tester sketch was written
  and then removed.
## M5 Unit PaHub (Val_Field only, for now)

A PCA9548A at address **0x70** with six channels. It connects one channel at a time
to its input (write one byte: bit n = channel n; 0 = all off), so sensors on
different channels can share an address: two distance sensors, or two light
sensors, on one port.

- Val_Field looks for a hub on each port (both wire orders) before looking for
  sensors. A port with a hub holds only the hub; its six channels are searched
  instead and named "Hub 0" to "Hub 5" by channel number.
- Every read switches the hub to that sensor's channel first (`I2CBus::use`).
- The setup screens have room for 3 sensors, so only the first 3 found are used.
- Channel numbering 0-5 matches the code; check it against the labels on the unit.
- Tested only against fake hardware so far.
