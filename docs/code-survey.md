# Firmware code survey (2026-10)

A survey of `firmware/` before any merging or restructuring: what the sketches have
in common, what could be simplified, and a layout that would keep things tidy
whatever comes next. Nothing here has been changed yet; this is a plan to discuss.

Snapshot: Val_Vehicle, Val_Field and Val_Brooks as of commit `37cc34d`.

---

## Where the lines go

About 4,800 lines in all, counting each shared file once and including comments:

| what it does | lines | share | where |
|---|---|---|---|
| **Matrix: faces, spikes, layers, setup screens** | ~2,290 | **~48%** | `Face` (584), `Spikes` (316), `Layers` (205), `Static` (41), `SetupMode` (365), `FieldSetup` (439), plus each sketch's face/spikes switching and emotion reading (~340) |
| **Hardware: finding sensors, I2C, PaHub, servos** | ~965 | ~20% | `Sensor`, `ColorSensor`, `DistanceSensor`, `I2CBus`, `Port`, `MotionServo` (~650), plus detection inside the sketches (~230) and Brooks' collision / distance / light detection (~90) |
| **Control: sensor -> motor** | ~310 | **~7%** | `sumWires` + `Connection` (Vehicle, ~60), `PotentialField` (~110), Brooks' behaviours and arbitration (~145) |
| Setup flow, saving, Serial, `setup()` / `loop()`, `Button` | ~600 | ~13% | mostly in the three sketches |
| Header comments, settings, globals | ~700 | ~14% | the tops of the sketches (worth keeping: students read them) |

The part children should study, how the wiring makes behaviour, is the smallest
part, and it is tangled with everything else.

---

## What the sketches have in common

1. **Two copies of the setup screens.** `SetupMode` (Vehicle) and `FieldSetup`
   (Field) have the same drawing helpers (`clear`, `put`, `drawRows`, `drawDots`,
   `drawMeter`, `push`), the same icons (+, -, x, moon, sun, near/far squares, check,
   forward arrow) and colours, and the same flow (static, then one sensor at a time,
   then a check mark). Only about 60 of their ~290 drawing lines differ.
2. **Four separate drawing engines.** `Face`, `Spikes`, `Layers` and the setup
   screens each have their own 25-pixel frame, "brightest wins" `put`, 40 ms frame
   timer, squared brightness, and `FastLED.show()`. Four places to fix if brightness
   ever needs tuning.
3. **The same colours defined four or more times** (green, red, white, amber in
   `SetupMode`, `FieldSetup`, `Spikes`, `Layers`).
4. **Vehicle and Field share about 140 distinct lines:** the per-port record and
   detection (`detectPort`, `isPlugged`, `isSetUp`, `anyWireNeedsSetup`,
   `searchForSensors`), the new-sensor surprise -> setup, save / load,
   `noteActivity` / `updateIdle` (word for word), the greeting, and the
   spikes-vs-face switching.
5. **PaHub code is in two sketches.** `findHub` is in Field and Brooks; Vehicle has
   no hub support at all. That is an accident of history, not a decision.
6. **Spike mechanics twice.** `Spikes` (vertical lanes) and `Layers` (horizontal
   rows) both implement "spikes leave at a rate, move one LED per frame, fading tail".
7. **Three ways to tie a sensor to a behaviour:** `Connection` (sensor + weight),
   `PotentialField::Source` (sensor + policy + angle), and Brooks' raw sensors with
   thresholds. Each fits its scheme, but they repeat the sensor's place, calibration
   and "plugged in and set up?".
8. **Small things:** `ColorSensor::begin()` and `DistanceSensor::begin()` repeat the
   try-both-wire-orders loop. Brooks uses snake_case function names (as asked for)
   while everything else is camelCase; one convention would be easier for students.

---

## Class simplifications

- **`Canvas`**: one 25-pixel frame with the frame timer, `put` (brightest wins),
  `putSoft`, `addRows` (`'#'` pictures), `blend`, and the squared-brightness push.
  `Face`, `Spikes`, `Layers` and the setup screens all draw on it. ~150 lines saved,
  and every display gets the same brightness.
- **`Palette` + `Icons`**: every colour and every 5x5 picture defined once.
- **`SetupScreens`**: one setup engine that runs a list of steps (mount / wheels,
  sign / policy, angle, low, high); each control scheme declares its steps. A
  "choose a control scheme" menu or a "forget everything?" confirmation becomes just
  another step. Replaces ~800 lines with roughly 450.
- **`SensorRack`**: the three ports plus PaHub channels, detection (hub included),
  hot-plugging, and per-place records. All three sketches use it; Vehicle gets hub
  support for free.
- **`Settings`**: save / load with one section per control scheme. The natural home
  for "saving on by default" and "forget everything".
- **`Mood`**: activity / idle timers, the greeting and the surprise, shared instead
  of copied.
- **`SpikeTrain`**: the shared spike timing, used by `Spikes` and `Layers`.
- **Each control scheme then holds only its idea**: Vehicle = wires + emotion
  reading; Field = `PotentialField`; Brooks = behaviours + arbitration. Each sketch
  file would shrink from ~700 lines to roughly 150-250, mostly the part worth
  reading.

---

## A layout that works either way

```
firmware/shared/
  hardware/   Port, I2CBus, Sensor, ColorSensor, DistanceSensor, MotionServo, Button, (Imu)
  matrix/     Canvas, Palette, Icons, Static, Face, Spikes, Layers, SetupScreens
  robot/      SensorRack, Wheels, Settings, Mood
  schemes/    Vehicle, Field, Brooks   (the control ideas only)
```

- **If the firmware is merged later**, it becomes one sketch with these folders
  under `src/`. The Arduino IDE compiles a sketch's `src/` folder recursively (high
  confidence), so `sync_shared.py` and the copies go away.
- **If the sketches stay separate**, the same folders can be synced into each, or
  `shared/` can become an Arduino library installed once (no syncing, but one more
  thing to install).

Either way the four-way split is worth doing now and doesn't commit to merging.

---

## Doing it safely

The host tests run the real sketches and compare their Serial output, so a
refactor that leaves the output identical hasn't changed behaviour (for the logic;
how the LEDs look still needs checking on the robot). Suggested order, keeping the
tests unchanged at each step:

1. `Canvas` + `Palette` + `Icons` (display only, lowest risk). **Done:** every LED
   frame of every host story is identical before and after (the tests now checksum
   the LED frames, so later steps are checked the same way).
2. `SetupScreens`
3. `SensorRack` (hub support for everything)
4. `Settings` (and saving on by default)
5. `Mood`
6. The folder layout, and the merge decision

Expected result: roughly 500-600 fewer lines (~12%), and each idea in one place.
