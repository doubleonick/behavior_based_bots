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
- Face vs spikes (2026-10, built in Val_Vehicle): the face is for interactions (hello,
  setup, surprise at a new sensor); while the wheel turns the matrix shows Type B
  spikes in lanes. When the wheel has been still for 1.5 s the run is over and a face
  comes back.
- Vehicle emotions (2026-10, built in Val_Vehicle): one module can't tell crossed
  from straight wiring, so it reads the emotion from the run itself. + wires: Angry
  for 3 s when it feels a lot at full speed for 1 s (charging, 2b); Scared when the
  wheel stops (it got away, 2a; Vehicle 1 ends the same way, by choice). - wires:
  Curious for 3 s when the feeling fades by 0.3 from a peak of 0.6+ while the wheel
  speeds up by 0.2 (moving on, 3b); Love when the wheel stops (3a). Mixed wires:
  Neutral. New faces: Scared (trembling tall eyes, wobbly zigzag mouth) and Curious
  (eyes glance up and off to the side, smiling). Thresholds are first guesses, to be
  tuned on the robot. Val_Field has spikes too (2026-10): green = pull, red = push, lanes
  ordered left to right by where each sensor points, the bottom row split into the
  two wheels (white forwards, amber backwards); its resting face is Neutral.

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
  Built (2026-10) for the Reflective IR (Port B, analog, calibrated) and the Key
  (Port C, LED shows the wire colour), in all three sketches: assumed when no I2C
  sensor answers there, confirmed or switched off in setup. The U021 Light Unit is
  not done yet.
  Correction (2026-10, from the robot): on Port B the IR read binary and inverted
  (G33 gets the digital output; the analog wire is on G23, no ADC). And assuming
  units that may not be there made setup ask about them every time, which breaks
  "setup only asks about what is plugged in". Proposed instead (waiting for a
  decision): detect the IR by testing its pin (an empty pin follows the ESP32's weak
  pull-up / pull-down, a unit holds it), on any port, analog where possible, digital
  otherwise; the Key joins when it is first pressed.
- **PaHub support**: done in Val_Field (2026-10): a port answering at 0x70 is a hub,
  and a sensor's place is "port + channel" (`Port::channel`, `I2CBus::use`). Still
  capped at 3 sensors, the setup screens' limit. Not yet in Val_Vehicle.
- **Fixed arbitration / subsumption ("Val Brooks")**: behaviour layers in a fixed
  hierarchy; the highest one that wants control drives the wheels. Display (mocked
  up 2026-10, not built): one row per behaviour, top row = top of the hierarchy.
  Left pixel: red = push/avoid, green = pull/approach; a dark gap; then 3 pixels in
  the behaviour's colour: light = yellow, distance = purple, collision = white,
  cruise/wander = blue. Rows not in control are steady; the row in control sends
  spikes left to right at the average wheel rate (|L| and |R| averaged, x 10 per
  second). No wheel information otherwise (decided: not needed).
  Wander: every 200 ms roll a D6 for one primitive: straight forward, straight
  backward, arc forward left, arc forward right, arc back left, arc back right (no
  rotations). Its left pixel is green when the primitive goes forward, red when
  backward, and keeps showing its current roll even when another layer has control.
  Built (2026-10) as `firmware/Val_Brooks/`, a test bench: uncomment one TEST_ line
  for one of six two-behaviour hierarchies (escape left / escape right collision,
  avoid / approach object, avoid / approach light, each over wander) or the full one
  (escape front collision -> avoid object -> approach light -> wander). Collisions
  from bump switches or the IMU (INERTIAL_COLLISION flag). No setup screens yet:
  thresholds and pins are settings in the sketch. Not yet run on the robot.
  The behaviour display can be switched off (long press; SHOW_LAYERS_AT_START for
  how it begins) for an ethology exercise: watch the robot and work out its
  hierarchy from behaviour alone.
- **ANNs**: a later paradigm, after vehicles, fields and subsumption.
- **Saving**: `SAVE_SETUP` (Preferences/NVS) exists but defaults to off.

## Records

- 2026-10: correction. Val_Vehicle (the emotions change) compiled in the host tests but
  not in the Arduino IDE: `enum Wiring` was declared below the first function, and the
  IDE's automatic function declarations go above the first function. Fixed by moving
  it up; `tests/host/run.py` now checks every sketch for this.
- 2026-10: correction. Val_Field arcing right with no sensors looked like mismatched
  servos (trim in code, or a "drive straight" setup step, was proposed). It was the
  wheel position: servos moved from the rear to the middle of the chassis and it drove
  straight. Trim not needed for now.
- 2026-10: spike "branches" (paths merging toward the wheel) dropped for "lanes"
  (one column per sensor): the 5x5 matrix is too coarse for branches to read.
- 2026-10: progress kept only in one browser rejected for the web guide (lost when a
  child moves between machines).
- 2026-10: DFRobot Gravity sensors explored and dropped (see `docs/hardware.md`).
- 2026-10: setup now opens with 0.5 s of white static and tunes back into the face
  afterwards (`Static.h`, `Face::tuneIn`). Mock-ups were made on a design canvas
  before coding.
