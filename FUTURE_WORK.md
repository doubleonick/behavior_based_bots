# Future work

Ideas and plans, roughly in the order they are likely to happen. When an idea is
dropped or a finding turns out wrong, record that here too.

## The web guide (`web/`)

- **Ideas for play (owner, 2026-11):** each stop could end in a **challenge** to do
  with that stop's robot (a fun goal, not just an experiment), and/or **open-ended
  play**: Val exploring a home-made version of where that stop's creatures live,
  with stand-in stimulus sources (a lamp for the sun, coloured patches on the floor
  for food or scent, ...). Not designed yet.
- **The opening sequence (2026-11, mock-up in progress):** a landing page like a lab
  notebook's cover, with pictures that hint at what will be explored; one button
  opens it into the introduction to the adventure; from there to meeting Val (stop
  0). Establishing this sequence comes before the real content.
- **The 5x5 matrix is near its limit (2026-11).** Each setup screen is close to what
  25 LEDs can carry (five shades, coloured dots, moving spikes), and with everything
  on one wire has up to nine screens. **Decided: favour the guide explaining and the
  matrix confirming (A):** each lesson shows pictures of that stop's screens and
  what each press does; this also builds out the lessons. The path-stage setting
  (each stop's firmware shows only that stop's screens) is a heuristic to use where
  it helps. Later, if needed: setup from the guide over USB (B, with on-robot setup
  kept), or a screen with more pixels (C, e.g. the Atom S3: a small colour LCD in the
  Atom's footprint, probably fits the Motion base; not checked). A proposed rule of
  thumb ("one question per screen, at most two colours with meaning, a screen that
  needs a key belongs in the guide") was judged too strict: kept only as a warning
  sign. Screens it flags: the five shades (strength, self), the "switched off by"
  screen, the field's colour wires.
- **Memory's off switch (decided 2026-11, mock-up `web/mockups/setup/memory_off_mockup.html`;
  not built):** another wire on the same module, whatever its sensor, can switch a memory
  cell off, at a fixed strong strength (twice what it feels); it still moves the wheel
  as an ordinary wire (a new stimulus takes over and the old behaviour is dropped,
  as down a priority list). Screen "What switches this memory off?" after Self: only
  the cell's own wire (cyan dot) and the chosen one (red dot, red spikes into the
  cell) show; the cell is an empty outline when the memory is off and a filled block
  when it is on. Biological fit: the roundworm's lasting dwelling / roaming states,
  ended mostly by a change in what it senses, with fading as the backstop (already
  built: the lower self shades). Later: two cells that hold themselves and silence
  each other.
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

- **DLight Unit in Val_Vehicle and Val_Field (2026-11, branch `expanded_vehicles`, not
  yet run on the robot in these sketches).** `DLightSensor` (shared, typeCode 'L'):
  found by itself in any port or PaHub channel, at 0x23 or 0x5C (the BH1750 has no ID
  register; no other unit Val uses answers there). Raw = lux; low / high set in setup
  with the moon and sun; the meter is logarithmic; the stimulus is linear in lux
  between low and high (a log stimulus might suit the wide range better: to try).
  Val_Field: pulls by default, like the other light sensors. Host test
  `vehicle_dlight` (Port B: room light about a third, flashlight full, dark stopped).
  Ex7_DLight worked on the robot in Port A (owner); B, C and a PaHub not tried.
  Not added to Val_Brooks.
- **The Color Unit as light or colour (2026-11, branch `expanded_vehicles`).** Agreed
  design (mock-up `web/mockups/setup/colour_setup_mockup.html`): a Color Unit has four
  channels, LIGHT (any colour, the clear filter, as before), MAGENTA, YELLOW, CYAN.
  Low and high are always set on brightness; a colour channel's stimulus is that
  brightness stimulus x how close the hue is (1 at the channel's hue, 0 at
  `HUE_WIDTH` degrees away: fuzzy, so red counts a third as magenta and as yellow) x
  saturation (white light has no colour). A colour channel's brightness is the
  brightest of red, green and blue, so a coloured beam counts as bright as white.
  Hues and width are settings (`MAGENTA_HUE` 300, `YELLOW_HUE` 60, `CYAN_HUE` 180,
  `HUE_WIDTH` 90). **Vehicles: one sensor, one stimulus** (built): setup asks the
  channel first (a white L, or a block in the colour; the wire's dot takes the
  colour), then sign, low, high as before. Starts on light, so a light vehicle needs
  one more long press than before and nothing else changes. Host test
  `vehicle_color` (magenta: full under magenta, a third under red or blue, nothing
  under white or cyan); `vehicle`, `vehicle_emotions` and `vehicle_units` gained the
  extra press (Serial unchanged, LED checksums changed by the new screen).
  **Not yet run on the robot.** **Fields (built 2026-11, not yet run on the
  robot):** one sensor, many stimuli. A Color Unit in a place has its light wire
  (`field.source[i]`) and three colour wires (`field.source[colourSlot(i, k)]`,
  channel 1-3) that share its sensor, angle and low / high; each colour's stimulus
  is the clear filter's brightness stimulus x the hue match (`valueFor`). Setup:
  light's pull / push / off (default pull); then angle, low, high, always asked for
  a Color Unit even if light is off; then magenta, yellow, cyan (default off), each
  with its dot blinking in its colour and a meter of that colour. 3 lanes: a colour
  wire is only offered while fewer than 3 wires are on, keeping a lane for each
  sensor still to come; the rest are skipped (Serial says so). Host test
  `field_color` (one Color Unit on the left side, tasting the floor: magenta pulls
  left, yellow pushes right, cyan off, red cancels); `field_hub` gained the colour
  presses (outcome unchanged). Note: Val_Field's colour brightness is the clear
  filter (calibrated once for all four wires), Val_Vehicle's is the brightest of
  red, green and blue: for floor patches set high over the white floor or a patch. **Hierarchies (later):** colour behaviours, `approach_magenta` style.
  *Dropped:* a separate "light or colour?" screen (light became one of the channels);
  a "?" show-me colour (fixed colours for now); strict colour matching (red would
  count as no colour at all); low and high per colour channel.
  First run (owner, 2026-11): **colour only works at close range,** so it suits
  patches or strips of colour on the floor, with the Color Unit pointing down.
- **Floor colour as taste and smell, not vision (idea for the path, 2026-11).** A
  sensor reading only what is right under it is a contact chemical sense, older
  than eyes, not colour vision (early eyespots sensed light and shade and its
  direction, at a distance, not colour). So on the path: light from above stays
  "light"; colour on the floor is "a chemical on the ground", and magenta, yellow
  and cyan can stand for different chemicals (food, danger, a mate: an analogy).
  Flies and butterflies taste with their feet, which is nearly what Val does
  (high confidence). Fits the early sea and seabed stops: the first moving animals
  grazed on algal mats (owner's source 02). Revisit the light-based creature
  choices with this in mind.
  *Vehicles:*
  - Vehicle 1 (one module, sensor down): a patch is a food patch. A - wire on its
    colour slows Val on the patch and speeds it off, so it lingers there: the
    roundworm C. elegans slowing on its bacterial food (kinesis; high confidence).
    A + wire hurries it across. Bacteria do the oldest version: they only know the
    chemistry where they are.
  - Vehicles 2 and 3 (two modules, one sensor each side): a strip is a scent trail.
    Each side tastes the ground under it; with the right wiring Val steers to keep
    the strip between its sensors, as an ant follows a trail with its two antennae
    (trail following: high confidence; that the left / right comparison is the
    whole story: medium). Uncrossed + turns off the trail, crossed + turns onto it.
    Needs a Color Unit per module (owner has one so far).
  - With one sensor, a trail can only be followed by swinging and comparing over
    time, as maggots and worms do: that needs memory, so it belongs at or after
    Vehicle 5.
  *Fields:*
  - One Color Unit, many stimuli: approach one colour, avoid another (approach
    yellow food, avoid magenta danger), each colour its own wire.
  - A floor sensor has no direction of its own, so its push or pull acts in the
    direction it is MOUNTED (left, right, front of the chassis), not where it points.
    One unit mounted to one side gives a one-sided pull at a patch's edge; two
    (left and right) give a proper turn towards or away.
  - Patches have edges, not gradients. For a real field, print a strip that fades
    from white to full colour: the stimulus already multiplies by saturation, so
    the fade reads as a concentration gradient to climb or flee. Untested.
  - Long-range smell (a wolf scenting the air) is the field idea proper: a
    gradient followed from far away. A dog with its nose on a trail is the close
    version.
- **`firmware/tools/Color_Test/` (2026-11, branch `expanded_vehicles`, not yet run on the
  robot):** the whole matrix shows the colour the Color Unit sees. The strongest of
  red, green and blue is stretched to full, so the colour shows even in dim light
  (`SHOW_BRIGHTNESS` true dims it with the light instead); below `TOO_DARK` it goes
  dark. Pressing the screen over something white takes a white balance. To find out
  on the robot: how true the colours look on the matrix's LEDs, and whether white
  balance is needed for the colours a lesson would use.
- **A safety reflex from the IMU: "not level, motors off" (2026-11, exploring).** Wanted:
  a safety feature with no extra sensor (so not a downward IR, and not a stop on the
  path), and a reason to explore the Matrix's IMU. `firmware/tools/IMU_Test/` (branch
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
  First run (owner, 2026-11): with the axes set, the bubble moves the right way. Not
  expected to save Val from driving off an edge (agreed). So the reflex is for being
  picked up, tipped or falling, and the IMU's better use may be as a sense: gravity
  is among the oldest senses (jellyfish and comb jellies steer by it), so it belongs
  early on the path as one more sense Val has, not as a stop. Ideas, none built: the
  slope under Val as a literal gradient at the potential-field stop (uphill pulls or
  downhill pulls; one IMU gives forward and sideways, like a left / right pair with
  no port); tilt as a sensor on a vehicle's wire; "picked up or tipped: stop" as
  subsumption's bottom layer. Frontier: noticing "I am driving but not moving"
  (command against what is felt). Unknown and to test first: whether Val can climb a
  slope without slipping, and whether a few degrees reads clearly while driving.
  IMU_Test now has a slow drive for that: short presses step through `SPEED_STEPS`,
  a long press sets level, `S1_DIRECTION` / `S2_DIRECTION` say which servos, and
  Serial adds a half-second average of the tilt ("slope").
  Second run (owner, 2026-11, one module, IMU_Test's slow drive): **Val climbs up
  and down an incline,** fairly steep (not measured). **A collision registered as a
  JOLT at the top speed** (22 degrees from stop); at lower speeds the bubble moved
  but no JOLT was called, so 0.6 g is too high for slow bumps (or they are too soft
  to feel: the Serial numbers would say). No false alarms seen, not tested hard.
  Left / right not tested (needs two modules). **Decided: do not put any IMU logic
  into the other sketches yet.** Still to measure before anything is built: the
  slope reading while driving on a ramp of known angle; the jolt size at each speed
  against the jolt of the wheels starting and stopping.
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
- **Setting up a wire's nerve cell on Val (agreed 2026-11; built 2026-11 in Val_Vehicle behind
  `NEURONS_IN_SETUP`, host test `vehicle_neuron_setup`, not yet run on the robot).** Mock-up:
  `web/mockups/setup/neuron_setup_mockup.html`. For each wire, after today's sign,
  low point and high point (when: decided 2026-11, see below):
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
  seen); how a memory cell is switched off.
  *When the neuron screens appear (decided 2026-11):* for now, a sketch setting
  (`NEURONS_IN_SETUP`, off by default), so Vehicles 1 to 3 never see them; Val_Vehicle
  only (Val_Field and Val_Brooks get none until a lesson needs them). Long term:
  one path stage setting (e.g. `PATH_STOP`) that switches features on as the path
  goes (neurons from Vehicle 4, memory from Vehicle 5, ...), set when a stop's
  firmware is flashed (fits flashing from the guide, the tentative USB plan).
  As built: the screens follow a wire's high point (skipped for an x wire and the
  key); the threshold is what `value()` reads when long-pressed; gains 1, 2, 5, 10,
  20 and self 0, 0.375, 0.75, 1.125, 1.5 (`SetupMode::GAINS` / `SELF_WEIGHTS`); the
  `PORT_x_NEURON` / `NEURON_GAIN` / ... settings are where each wire starts; the
  cell is remembered with the wire (and saved with `SAVE_SETUP`). Still to check on
  the LEDs: whether the five shades can be told apart.
  *Rejected:* always asking (a screen about cells before the lesson has them);
  unlocking on the robot by a hidden gesture or an existing cell (hard to discover). *Dropped:* showing strength by the
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
