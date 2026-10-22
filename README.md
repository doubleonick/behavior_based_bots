# Val

A small educational robot with a face. Val runs on an **M5Stack Atom Matrix** sitting
on an **Atom Motion** base: the 5x5 LED matrix is the face, the Motion drives
continuous-rotation servos as wheels, and plug-in Grove sensors are how Val
perceives the world.

The firmware is a framework for exploring how simple wiring produces behaviour,
for kids (roughly 8 and up) with a parent, tutor or club leader alongside:

| sketch | idea |
|---|---|
| `firmware/Val_Vehicle/` | **Braitenberg vehicles.** Every sensor is wired (+ excitatory, − inhibitory, or off) to one wheel. |
| `firmware/Val_Field/` | **Potential fields.** Every sensor pushes or pulls in the direction it points; the sum steers two wheels. |

Both share one set of classes (the face, sensors, servos, button) in `firmware/shared/`.
Setup happens on the robot itself with its one button, so no computer is needed once
it's flashed.

A web guide, with a story that leads kids through the things to explore and notes
for the grown-ups, is planned in `web/`. See `FUTURE_WORK.md`.

## Hardware

- M5Stack Atom Matrix (ESP32, 5x5 RGB LEDs, one button under the screen)
- M5Stack Atom Motion v1.2 (servo ports S1–S4, Grove Ports B and C)
- 1–2 continuous-rotation servos in S1 (left wheel) and S2 (right wheel)
- Sensors, one per Grove port (Atom Port A, Motion Ports B and C), found automatically:
  - M5 Color Unit (TCS3472), used as a light sensor
  - M5 ToF Unit (VL53L0X), used as a distance sensor
- Optional, Val_Field only: an M5 PaHub in one port, so two sensors of the same kind
  can share it (up to 3 sensors in all)

Pins, registers and known quirks: `docs/hardware.md`.

## Building

Arduino IDE (or arduino-cli) with the **M5Stack** board package, board *M5Atom*.
Libraries from the Library Manager: **FastLED** and **VL53L0X by Pololu**.
Open `firmware/Val_Vehicle/Val_Vehicle.ino` or `firmware/Val_Field/Val_Field.ino`.

## Using it

1. Power on. Val says "Hello, my name is, Val" (the mouth animates).
2. The first time, setup starts by itself; later, **hold the button for 2 seconds**.
   Short press changes a choice, long press confirms it.
3. After setup, Val drives. While the wheel turns, the matrix shows **spikes**: one lane
   per sensor (green for a + wire, red for a −), faster the more it feels, and the wheel
   firing along the bottom row. Val_Vehicle shows the "emotion" a run plays out:
   Angry for a few seconds while a + wire charges at what it feels, Curious while a
   − wire moves on from it; when the wheel stops, Scared (+ wires: it got away) or
   the heart (− wires: it stays by what it loves). In Val_Field, green lanes pull and red
   lanes push, the lanes are placed left to right by where each sensor points, and the
   bottom row is split into the left and right wheel (white forwards, amber backwards).

Plugging in a new sensor while Val is running makes it look surprised and then
set up just that sensor.

## For developers

```
python firmware/sync_shared.py --check    # are the sketches' copies of shared/ in step?
python firmware/sync_shared.py            # copy shared/ into the sketches
python tests/host/run.py                  # run both sketches against fake hardware
```

Edit classes in `firmware/shared/`, never a sketch's copy. `CLAUDE.md` has the
rest of the working rules.
