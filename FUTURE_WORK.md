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
- **Must explain which sensors fit together** (for the adult; too much for matrix
  icons): one robot has three ports and only Port A reads a level, so analog units
  (Light Unit, Reflective IR) share one slot; a Vehicle needs one sensor per robot,
  so this rarely matters there; Field and Brooks can do a little on one robot, but
  multi-sensor work goes through the PaHub (I2C units only). Table in
  `docs/hardware.md`, "What each unit can do in each port".
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
  **Tentative change (2026-11), pending testing: the guide talks to Val over the USB
  cable** (Web Serial, Chrome / Edge on a computer). Reasoning: a computer is in the
  room anyway, because the Arduino IDE is the only way to change firmware today. Uses,
  in the order to try them: (1) Val reports which sensors it found, so the guide only
  offers what is plugged in; (2) the guide sends a subsumption hierarchy (mock-up
  2026-11: plan and predict in the guide, or build it bottom-up on Val's one button;
  the on-robot setup stays as the fallback so rule 5 still holds); (3) one-click
  flashing of ready-built firmware from the guide (e.g. ESP Web Tools), instead of
  installing the IDE; (4) settings that now need a reflash (`PORT_A_UNIT`, `BUMPERS`,
  ...) sent over the link. Bluetooth only if an untethered live view is wanted later.
  Nothing built or run on the robot yet. PAW's Hierarchy Builder (a desktop app, BLE)
  uses the same `verb_target` behaviour names as Val_Brooks: a matching message
  format is possible, not decided. Tablets (no Web Serial, and no Web Bluetooth on
  iPad) are not supported by this route.
- **Tentative path (2026-11): eleven robots,** each to be confirmed by building it on
  Val before its lesson is written ("if the robot cannot perform the behaviour, the
  lesson will not work"): Meet Val; Vehicle 1; Vehicles 2a/2b; Vehicles 3a/3b (ending
  on 3c: several senses on mixed wires); habituation; Vehicle 4 (4a a favourite
  level, 4b a threshold); potential field; Vehicle 5 (holding a state); subsumption;
  Vehicle 7 (learning by association); Vehicle 6 (selection, probably a web game).
  Vehicles 8 to 14 are appendix only. More may follow as a "frontier" that combines
  ideas. Confirmed on the robot by the owner: two joined modules give convincing
  2a/2b and 3a/3b. Creatures, habitats and the map are decided after the robots.
- **Creatures (decided 2026-11):** each stop shows a living animal, from as old a
  lineage as possible, and the Handbook names the ancient ancestor. The robots are a
  coarse sampling of the tree; the appendix gives the deeper context. Body-plan
  spine: no sides (radial, e.g. jellies; Vehicle 1) and then bilateral symmetry
  (Vehicle 2 onward). The owner's source notes and synthesis are kept outside the repo.
- Art: Val drawn as 16-bit pixel art rendered from the chassis STL files
  (`vechicle_one_unibody_v3` / `v3a`, mirrored). Matrix case light grey, Motion base
  dark grey, blue battery at the front, grey wheel with a thin red tyre centred in the
  rear fork. Two-module Val: joined counterweight to counterweight, grey spacer beams
  in the gap. Chassis colours still open (one per side: yellow/purple, orange/blue,
  red/champagne, black/white). The web Val reuses Amazed Kingdoms' voice (browser
  speech, so it varies by device).
- **Art direction chosen (2026-11):** notebook pages (direction B of the art canvas)
  with the specimen lab's round observation window (from C) for the creature side.
  Val's avatar is a 16-bit render from the CAD, from above and just behind ("A+":
  front up the page, face upright), `cad/render/val_v1_above_rear.png`. It follows the
  physical build chapter by chapter (V1 = one module; later builds = new renders).
  Build sheets, LEGO style, in the notebook look, for each kit-to-build step: later.
  *Correction:* earlier art had a grey wheel with a red tyre and the face standing
  at the rear; the built robot has a black wheel, and the Matrix lies face up at the
  rear of the Motion base. The V1 sensor sits in front between two grey 2x4 L beams
  whose feet act as casters.
  For the light vehicles the Color Unit points **up** at the light (see
  docs/hardware.md), so the V1 avatar for the light chapters should show it facing
  up, not forward: still to re-render.
- **Vehicle 1 lesson mock-up (2026-11), `web/mockups/v1-lesson/`:** four steps
  (Watch, What are they doing?, Look inside, Try it). Decisions:
  - **One stimulus for both:** the woodlouse senses light too (it avoids light to
    avoid drying out), so "What are they doing?" has one best fit. *Dropped:* a
    woodlouse sensing dryness beside Val sensing light (two right answers).
  - **Best fit opens the reveal at once;** other picks get a nudge and one more try.
  - **The reveal asks with the child's own word:** "Could Val *love the dark* with
    just one light sensor joined to one wheel?"
  - **Try it is predict, run, check:** tap a prediction, 20 s run, verdict with the
    measured share, a row in "My notebook". *Dropped:* a free-text prediction box
    (no feedback, disconnected from the play).
  - **A pinned observation deck** keeps Val and the woodlouse in view through every
    step (a top strip on phones).
  - **Val is redrawn from its real tracked run** (the A+ sprite on a drawn floor,
    same pool of light and same orange time-trail as the woodlouse), with a "show
    the real clip" switch as proof. *Dropped:* the plain video beside a vector
    window, and pixelating the footage (it looked like a poor camera, not a
    cartoon).
  - Still to do: saving the notebook (save file + code); the story text (prologue
    and each stop's scene, placeholders for the owner to write); moving the lesson's
    "For the adult" slips into the Handbook, since the pages are now all written to
    the child.
- **Tone settled (2026-11): expedition**, Indiana Jones map style, game feel carrying
  the narrative, the notebook as the player's part. Everything is child-directed;
  helpers and advanced students get an appendix, *The Expedition Handbook*. Mock-ups:
  `Expedition.dc.html` (map, prologue, per-stop scenes, journal) and
  `Handbook.dc.html`.
- **Filming V1 for the tracked cartoon (2026-11):** overhead phone, plain light floor,
  a bright flashlight fixed above as the light, exposure locked, two seconds of
  empty floor first, whole run in frame. + wire: Val waits in the dark until the
  light comes on. - wire: set Val down by hand in the dim. Dead time is trimmed
  afterwards. First takes (IMG_2169 +, IMG_2171 -) tracked well by the blue battery.
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

- **A safety reflex from the IMU: "not level, motors off" (2026-11, exploring).** Wanted:
  a safety feature with no extra sensor (so not a downward IR, and not a stop on the
  path), and a reason to explore the Matrix's IMU. `firmware/IMU_Test/` (branch
  `expanded_vehicles`, **not yet run on the robot**) is the first step: a bubble level
  on the matrix, tilt / total g / jolt on Serial with one word (LEVEL, TILTED,
  FALLING, JOLT), the button sets "this is level", and an optional trial that stops
  S1 while not level and restarts after a second of level. To find out with it:
  which axes are Val's front and left; what the numbers are with the nose over a
  table edge, picked up, on its side, falling; with the wheel crawling, how far the
  nose drops before Val is past saving. Expectation, not measured: it can stop the
  wheels when picked up, tipped or falling; it only saves Val from an edge if
  something other than the drive wheel leads and Val is slow. Then choose the rule
  and put it in the shared code for every sketch. The thresholds in the test (12
  degrees, 0.4 g, 0.6 g) are first guesses.
- **Vehicles 4 and 5: one nerve cell on the wire (2026-11, branch `expanded_vehicles`,
  a test bench).** One part, `Neuron` (Val_Vehicle/Neuron.h), is meant to carry
  Vehicles 4, 5 and 7, so each vehicle is a small change to the last and the lessons
  need one explanation. It adds its inputs, is silent below a threshold, fires harder
  above it up to a ceiling, and follows its input over a short time:
  `target = clamp(gain x (input + self x output - threshold), 0, 1)`, output moving
  towards target over `timeMs`. Gain 1 is graded (the biological default); gain 20 is
  all-or-none (Braitenberg's abrupt 4b and his Vehicle 5 threshold devices). A
  neuron that listens to itself (`self` at least threshold + 1 / gain) stays on once
  switched on: memory.
  In Val_Vehicle a wire can have one neuron (`Connection::place`): IN_THE_WIRE
  (4b: sensor -> neuron -> motor) or AGAINST_THE_WIRE (4a: the wire goes straight to
  the motor and a side branch through the neuron holds it back twice as hard, which
  gives a favourite level at the threshold; lopsided, peak speed = the threshold).
  Set per port in the sketch (`PORT_A_NEURON`, `PORT_A_THRESHOLD`, `NEURON_GAIN`,
  `NEURON_TIME_MS`, `NEURON_SELF`, `AGAINST_STRENGTH`), not on the robot yet: setup
  screens come only if the behaviour proves itself. With no neuron nothing changes.
  Host tests `vehicle_neuron_step`, `vehicle_neuron_favourite`, `vehicle_neuron_memory`
  pass. **Not yet run on the robot.** To confirm there: 4b, Val sits still and then
  goes; 4a on one module, fastest at the favourite level (with a - wire: slowest
  there, so it lingers); 4a on two joined modules, whether Val circles a light at the
  distance it likes (the least certain); memory, a flash of light sets Val going and
  it keeps going. Still to design for Vehicle 5: what switches a memory neuron off
  (an inhibiting input from another sensor, which needs wires between ports), and
  whether habituation is the same neuron with a slow negative echo. The key ignores
  neurons. The faces (Angry, Curious, Scared, Love) still read what the sensor feels,
  not what reaches the motor: to revisit. The motor's own sum (weights, and a bias of
  1 when every wire is inhibitory) is the same arithmetic as a neuron's but has not
  been rewritten as one.
  First run on the robot (owner, 2026-11, one module, not extensive): recognisable
  4a, 4b and memory behaviours.
- **Setting up a wire's nerve cell on Val (agreed 2026-11, not built).** Mock-up:
  `web/mockups/setup/neuron_setup_mockup.html`. For each wire, after today's sign,
  low point and high point:
  1. *Place:* no cell, a cell on the wire, or a cell next to the wire. The wire is
     drawn in its sign's colour (green +, red -) and the cell blinks **cyan** (the
     colour for a nerve cell everywhere). Where the cell sits is the only difference
     between "in the wire" and "against the wire". No cell skips the rest.
  2. *Threshold:* shown to Val like the low and high points. The cell (a cyan 3x3)
     sleeps on the left, the live bar is on the right with a cyan top; long press
     sets the threshold at what the sensor feels now. Then "Try it": the mark stays,
     the cell lights whenever the bar reaches it; long press keeps, short press sets
     again. Any level from 0 to 1 is allowed, on purpose: 0 (always awake; with gain
     1 in the wire that is a plain wire, so Vehicles 1 to 3 are the same part at its
     plainest; against the wire it is a dead wire) and 1 (hardly ever wakes) are
     discoveries, not errors. Threshold comes second, not last: it belongs with the
     sensor while the lamp is still in hand, and last would muddle "Try it" with the
     strength and self choices (and could look like Val just running).
  3. *Strength:* the middle 3x3 fires top to bottom (input to output); five evenly
     spaced shades choose the gain (proposed 1, 2, 5, 10, 20).
  4. *Self:* the 3x3's ring fires counter-clockwise; five choices starting at zero
     (0 = no self connection; proposed 0, 0.375, 0.75, 1.125, 1.5).
  So gain and self become per wire (they are one setting for all neurons in the test
  bench). Open: whether five shades can be told apart on the real LEDs at Val's low
  brightness (try with a small sketch first); what the driving view shows for a cell
  (the wiring view with a blinking cell, or a zoomed 3x3 whose lit squares count how
  hard it fires and whose right-hand loop shows memory:
  `neuron_display_mockup.html`); whether the display stays on while a cell holds Val
  still (today the face returns when the wheel stops, so a waiting cell is never
  seen); how a memory cell is switched off. *Dropped:* showing strength by the
  cell's brightness in the wiring view (too subtle), blue for the cell (cyan reads
  better), and a memory cell drawn as just "stays lit" (no feedback visible).
- **Subsumption setup (mock-up 2026-11, not built):**
  `web/mockups/setup/subsumption_setup_mockup.html`. On Val: the bottom layer first
  (wander or cruise), then one layer up at a time from the behaviours whose sensors
  were found, "finished" flashes the stack. In the guide: stack the cards, see the
  matrix Val will show, read a prediction. Only offering behaviours for sensors
  that are plugged in is wanted; a web builder can only know that if Val tells it,
  hence the tentative USB link (see the web guide section).
  *Dropped (2026-11):* the first version bent the signal with formulas on the wire
  (a tent "HUMP" and an on/off "STEP" with a 0.05 gap). Faithful to Braitenberg's
  text, but an abstraction with no biological part to point at; replaced before it
  was run on the robot. Not a Gaussian either way.
- **Two-wheel Braitenberg vehicle.** Val_Vehicle wires every sensor to one servo.
  For the classic vehicles (fear, aggression, love, exploration), each wire needs a
  "which wheel" step (left / right / crossed). The face could name the vehicle's
  "emotion".
- **More M5 units** (each Matrix + Motion has three Grove ports):
  - U021 Light (photoresistor): analog on the white wire, on/off on the yellow
  - U175 Reflective IR: analog on the white wire, on/off on the yellow
  - U144 Key: button on the white wire, one SK6812 LED on the yellow
  - **PbHub (U041) to look into:** an I2C hub (STM32, 0x61) with six ports that reads
    on/off and analog pins for the ESP32. If it works as described, the Light Unit,
    Reflective IR and Key could join many sensors on one robot (it could even sit on
    a PaHub channel). Unverified: which channels read analog, and whether it can drive
    the Key's SK6812 LED. Would need its own class.
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

- **Port layouts settled (2026-10):** Val_Vehicle: Color or ToF in B, IR or Light Unit
  in A (a level, `PORT_A_UNIT`), Key in C. Val_Field: the same, no Key (off by
  default). Val_Brooks: ToF / Color pairs on a PaHub in B, left Key in A, right Key in
  C (`BUMPERS 3`). **Dropped** with this: the Reflective IR on Port B in Val_Vehicle
  and Val_Field, and the IR vehicle's "cruise" wiring (0.25 forwards, +/-0.5 when it
  sees something, so a - wire reverses). The IR on Port A is a plain wire, like the
  ToF. `ReflectiveIRSensor` still reads on/off on Port B, for Val_Brooks.
- **Val_Brooks with one Color Unit** (2026-10): the light behaviours compare its
  reading now with 0.2 s ago (a change of 15% or more counts). Getting brighter =
  "brighter on the sensor's side", darker = "the other side"; with the sensor on the
  left, approach turns left as it gets brighter. With a Color Unit on each side, the
  left/right pair takes over by itself. Untested on the robot.
- **Val_Brooks approach / avoid** now turn on the spot (0.2 s, toward / away from the
  stronger side) instead of steering while driving; escapes arc backwards to the hit
  side for 1 s (no straight part), or turn on the spot for 1 s when both sides are hit.

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
