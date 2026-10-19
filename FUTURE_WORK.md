# Future work

Ideas and plans, roughly in the order they are likely to happen. When an idea is
dropped or a finding turns out wrong, record that here too.

## The web guide (`web/`)

A web interface that carries the instructions, with a game/story layer:

- **For kids:** a story that leads them through the things to explore with Val,
  one discovery at a time (wire a sensor, predict, try, watch the face react).
- **For parents, tutors and club leaders:** notes alongside each step on what to
  set up, what to ask, what the child is learning, and what to do when it doesn't
  work.
- Served as a static site (GitHub Pages) from `web/` so it versions with the firmware
  it describes.
- Open questions: the story's frame and characters; how progress is tracked (none,
  per-device, or accounts); whether the site ever talks to the robot (Web Serial /
  BLE) or stays instructions-only.

## Firmware

- **Two-wheel Braitenberg vehicle.** Val_Vehicle wires every sensor to one servo.
  For the classic vehicles (fear, aggression, love, exploration), each wire needs a
  "which wheel" step (left / right / crossed). The face could name the vehicle's
  "emotion".
- **More M5 units** (each Matrix + Motion has three Grove ports):
  - U021 Light (photoresistor): analog on the white wire, on/off on the yellow
  - U175 Reflective IR: analog on the white wire, on/off on the yellow
  - U144 Key: button on the white wire, one SK6812 LED on the yellow
  None of these are I2C, so they can't be detected; a fixed port layout
  (A = light, B = IR, C = key) avoids a setup step. Port B's analog needs the white
  wire on G33: check on the hardware.
- **PaHub support**: treat a port answering at 0x70 as a hub; a sensor's location
  becomes "port + channel". Mostly `Port` and `I2CBus`. The setup screens' dots
  only fit about 5 sensors.
- **Fixed arbitration** (behaviour layers that suppress each other) and **ANNs**:
  the later paradigms in the program, after vehicles and fields.
- **Saving**: `SAVE_SETUP` (Preferences/NVS) exists but defaults to off.

## Records

- 2026-10: DFRobot Gravity sensors explored and dropped (see `docs/hardware.md`).
- 2026-10: setup now opens with 0.5 s of white static and tunes back into the face
  afterwards (`Static.h`, `Face::tuneIn`). Mock-ups were made on a design canvas
  before coding.
