# Examples: one device at a time

Small, standalone sketches for the Atom Matrix (+ Atom Motion). Each shows the bare
minimum to use one device, and draws what it reads on the 5x5 matrix. None of them
uses the Val sketches' shared files: copy one folder, open its `.ino`, upload.

| sketch | device | needs |
|---|---|---|
| `Ex1_Servo` | a servo on the Motion (S1..S4) | FastLED |
| `Ex2_ToF` | 90° ToF Unit (distance) | FastLED, VL53L0X (Pololu) |
| `Ex3_Reflective_IR` | Reflective IR Unit | FastLED |
| `Ex4_Light_Unit` | Light Unit (U021) | FastLED |
| `Ex5_Key` | Key Unit (button + LED) | FastLED |
| `Ex6_Color` | Color Unit (R, G, B and clear light) | FastLED |

Board: M5Stack board package, **M5Atom**. Serial Monitor at 115200.

## Which port gives what

The Atom Matrix has Port A; the Motion has Ports B and C. A Grove cable's **yellow**
wire lands on the first pin listed, its **white** wire on the second.

| unit | Port A (G26 / G32) | Port B (G33 / G23) | Port C (G19 / G22) |
|---|---|---|---|
| ToF, Color (I2C) | works | works | works |
| Reflective IR, Light Unit | **a level** (white on G32) | on/off only | on/off only (not tried) |
| Key | works | works | works |

- **I2C units** (ToF, Color) work in any port, and on a PaHub (set `HUB_CHANNEL`).
- **Analog units** (Reflective IR, Light Unit) only give a **level** in Port A. Only
  G26, G32 and G33 can read a voltage, and in Port B the level wire lands on G23.
  Ports B and C give their on/off output, which flips where the unit's knob is set.
- **Port B is wired the other way round** from A and C (yellow on G33, white on G23).
- **The Key** worked in all three ports on battery; on USB its presses were not seen.
- **Servos** get their power from the Motion's battery: switch the Motion on.

See `docs/hardware.md` for the details and what has been tested.
