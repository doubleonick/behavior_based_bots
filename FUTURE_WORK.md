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
- Progression: V1, V2a/b, V3a/b, potential field, subsumption. State machines and
  ANNs to be placed. Draft chapter outline in `web/OUTLINE.md`.
- Frame: the evolution of perception and action in living things, told alongside
  Val's; simple design giving rich behaviour. Tone (stranger in a strange land /
  lab notebook / expedition) still open.
- Progress must travel between machines (child moves between households, tutors,
  clubs), so per-browser storage alone is not enough. Proposed: save file + short
  save code, no accounts at first.
- Instructions only: the site does not talk to the robot (no Web Serial / BLE) for
  now.
- Art: Val drawn as 16-bit pixel art rendered from the chassis STL files
  (`vechicle_one_unibody_v3` / `v3a`, mirrored). Matrix case light grey, Motion base
  dark grey, blue battery at the front, grey wheel with a thin red tyre centred in the
  rear fork. Two-module Val: joined counterweight to counterweight, grey spacer beams
  in the gap. Chassis colours still open (one per side: yellow/purple, orange/blue,
  red/champagne, black/white). The web Val reuses Amazed Kingdoms' voice (browser
  speech, so it varies by device).
- Two-module Val, which one has the face: chosen on the robot (both Matrices ask,
  the child presses one). The other Matrix shows its sensor's signal instead; five
  displays mocked up (level, spikes, trace, ripple, wire), none chosen yet. The web
  Val has to match: since the site doesn't talk to the robot, the guide asks which
  colour has the face and saves the answer with the child's progress.

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

- 2026-10: progress kept only in one browser rejected for the web guide (lost when a
  child moves between machines).
- 2026-10: DFRobot Gravity sensors explored and dropped (see `docs/hardware.md`).
- 2026-10: setup now opens with 0.5 s of white static and tunes back into the face
  afterwards (`Static.h`, `Face::tuneIn`). Mock-ups were made on a design canvas
  before coding.
