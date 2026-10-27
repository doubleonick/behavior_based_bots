# Val

An educational robot with a face: M5Stack Atom Matrix (ESP32, 5x5 LEDs, one
button) on an Atom Motion base (servos), with plug-in Grove sensors. Arduino
firmware now; a web guide with a story for kids and notes for parents is planned
in `web/`.

Audience: kids roughly 8 and up, with a homeschool parent, tutor or club leader
alongside. Everything a child sees (the face, the setup screens, the guide) should
make sense to them; everything an adult needs should be written down for them.

---

## Layout

| path | holds |
|---|---|
| `firmware/shared/` | **the originals** of every class the sketches share |
| `firmware/Val_Vehicle/` | Braitenberg vehicle: sensors wired +/−/off to one servo (S1) |
| `firmware/Val_Field/` | potential field: sensors push/pull by direction; two wheels (S1 left, S2 right) |
| `firmware/Val_Brooks/` | subsumption test bench: behaviours in a fixed hierarchy, picked by uncommenting one `TEST_` line; two wheels, sensors on a PaHub |
| `firmware/Unit_Tester/` | diagnostics: every port pin's reading and "is anything driving it?" on the matrix and Serial; a key LED test. Uses no shared files |
| `firmware/IR_Test/` | just the Reflective IR Unit on Port A or B: digital and analog readings on the matrix and Serial. Uses no shared files |
| `firmware/Light_Test/` | just the Light Unit on Port A, B or C: both pins' readings (level or on/off) on the matrix and Serial. Uses no shared files |
| `firmware/sync_shared.py` | copies `shared/` into the sketches |
| `tests/host/` | runs the real sketches against fake hardware |
| `docs/hardware.md` | pins, Motion registers, sensor chip IDs, port limits, quirks |
| `docs/code-survey.md` | where the firmware's lines go, duplication, and a proposed tidy-up plan |
| `web/` | the planned web guide |
| `FUTURE_WORK.md` | plans, open questions, and records of things dropped or corrected |

Shared classes: `Canvas` (the 5x5 picture every display draws on, and `FrameTimer`),
`Palette` (every colour), `Icons` (every setup picture), `Face` (expressions, talking,
`tuneIn`), `Spikes` (the wiring's activity while driving), `Static` (white static),
`MotionServo`, `Port`, `I2CBus`, `Sensor` (parent; 0..1 stimulus from two calibration
points), `ColorSensor`, `DistanceSensor`, `ReflectiveIRSensor` (on/off, Port B),
`KeySensor` (button + LED, Port C), `Button` (short / long / hold).

---

## Rules that are load-bearing

### 1. `firmware/shared/` is the source of truth

Arduino needs every `.cpp` inside the sketch's own folder, so shared classes are
copied into each sketch. **Edit `firmware/shared/<file>`, then sync. Never edit a
sketch's copy: the next sync overwrites it.**

```
python firmware/sync_shared.py --check   # exit 1 if any copy has drifted
python firmware/sync_shared.py           # propagate
```

A sketch only receives a shared file it already has. To give a sketch a new shared
file, copy it in once by hand; the sync keeps it current after that. New sketches go
in `SKETCHES` in `sync_shared.py`.

### 2. Code style: braces on every conditional body

In all C/C++/Arduino code, every `if` / `else` / `for` / `while` body goes in curly
brackets, never the one-line form. Students read this code; mixed styles confuse
them. Prefer plain, commented code over clever code for the same reason.

### 3. Servos go through the Motion's angle register

The ESP32 can't drive the servo pins; the Motion's STM32 does, over I2C at 0x38.
Use the angle registers (0x00–0x03). The pulse registers did not move the servo on
real hardware. Continuous servos: 90 = stop. See `docs/hardware.md`.

### 4. Sensors are found, not configured

Each sketch looks in every port for every sensor type, tries both wire orders, and
tells sensors sharing address 0x29 apart by chip ID. A new I2C sensor type is a new
`Sensor` child with its own `begin()` that checks a chip ID. Port C has no ADC, and
Port B is wired the other way round (a unit's analog wire lands on G23, no ADC), so
analog units only give a level on Port A.

Units that can't identify themselves (the Reflective IR on Port B, the Key on Port C)
are the one exception: a sketch *assumes* them when no I2C sensor answers in that
port (`ASSUME_IR_ON_PORT_B` / `ASSUME_KEY_ON_PORT_C`; in Val_Vehicle both default to
0, so you switch one on and reflash), setup lets the child switch one off, and an I2C
sensor plugged in later takes over. How the sketches should recognise these units is
still undecided. Real I2C sensors always get
the setup screens' slots first.

### 5. The robot is set up on the robot

One button: short press changes, long press confirms, hold 2 s starts setup. No
computer is needed after flashing. New features should keep that true.

---

## Working with the owner

- **Ask before code on design questions.** When behaviour, UI, or the look of an
  animation is undecided, ask clarifying questions or make a mock-up first and wait
  for a choice. When the request is clear, just do it.
- **Preserve what exists.** Change only what's asked; don't restructure or strip
  working projects without agreement.
- **Give confidence ratings** on technical claims that can't be checked here
  (hardware behaviour, electrical safety, untested library behaviour).
- **Say what is tested vs assumed.** Firmware can't be flashed from a sandbox. The
  host tests check logic only. Report hardware-dependent changes as unverified until
  the owner has run them.
- Record dropped ideas and corrections in `FUTURE_WORK.md`, not just new plans.

---

## Tests

```
python tests/host/run.py            # compile both sketches with fake hardware and compare output
python tests/host/run.py --update   # accept new output after an intended behaviour change
```

Each test is a scripted story (`tests/host/<test>/main.cpp`: button presses and
sensor readings over time) run against stub Arduino libraries (`stub/`). A failing
test writes `actual.txt` beside `expected.txt`. If a change is meant to alter
behaviour, check the diff makes sense before running `--update`, and mention it.
The stubs only model what the sketches use; extend them when a sketch needs more.
Each test's fake FastLED also checksums every frame the matrix shows and prints it at
the end (`LED frames shown: ..., checksum ...`), so a change to what the LEDs show
fails the test too, not just a change to Serial.

The tests compile with g++, not the Arduino IDE, which differs in one known way: the IDE
writes a declaration for every function in a `.ino` above the first function. So a
type (`enum` / `struct` / `class`) that any function's return type or parameters use
must be declared above every function. `run.py` checks this before each test. Other
IDE-only problems are not caught here; a real build for the robot is still the final check.

Building for the robot: Arduino IDE or arduino-cli, M5Stack board package (M5Atom),
libraries FastLED and VL53L0X (Pololu).
