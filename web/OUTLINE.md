# Web guide: outline (draft for the owner to edit)

Status (updated 2026-11-22): **the guide is started** in `web/guide/`: the cover,
the inside cover, the map, Meet Val and Val's button, built to the design principle
below. The rest is mock-ups (`web/mockups/`). The tone is settled (an expedition, see *Decided since*). The path is
eleven stops, still tentative (each robot must work on Val before its lesson is
written). Everything here is a proposal to cut, reorder or rewrite.

This file is the guide's structure: the opening, the shape of every stop, the path,
the map, and saving progress. The reasons, corrections and dropped ideas behind it
are in `FUTURE_WORK.md` (*The web guide*), which also lists recent changes at the top.

---

## The big idea

The guide tells two stories side by side:

1. **Life's story.** How perception and action evolved: from a single cell that
   just speeds up or slows down, to bodies with two sides, to nervous systems that
   balance pulls against each other, to brains built in layers on top of older
   reflexes, to creatures that remember and learn.
2. **Val's story.** Val goes through the same steps, one wire at a time.

The lesson both stories teach: **simple parts, wired simply, give surprisingly rich
behaviour.** And the harder, more grown-up lesson: what we see as *wanting*,
*fear* or *curiosity* may be in the coupling between a body and its world, not in
a hidden plan inside it.

Two ideas from the literature to carry through every chapter:

- **Braitenberg's "uphill analysis, downhill invention."** It is easy to build a
  vehicle and hard to work out, from watching it, how it is built. Kids feel this
  directly: they predict, they are wrong, they look at the wiring, it was simple.
- **Brooks: "the world is its own best model."** Val keeps no map. It reacts to
  what its sensors give it right now, and that is enough for a lot.

Robotics informing cognitive science goes the other way too: each chapter ends with
a question about *us* (Is a moth that flies into a lamp "choosing"? When you flinch,
who decided?).

---

## Design principle: style kept apart from content and art (2026-11-22)

The notebook look should be replaceable later (another notebook, a comic, a field
guide) without rebuilding the guide. So the guide is built in three layers that
change independently:

1. **Style: one shared stylesheet of named values** (colours, fonts, paper, ink,
   borders, the leather) that every page uses. A new style is a new set of values,
   plus new rules only where the layout itself differs. Pages never set a colour or
   font of their own.
2. **Content: kept as data,** apart from the layout: the stops, creatures, places,
   promises, story text and labels. Changing the story does not touch the look, and
   changing the look does not touch the story. Placeholders live here too, marked as
   placeholders.
3. **Art: separate image files** (Val, each creature, each place, the Ci, stickers),
   referred to by name. A new style can swap in new drawings without editing pages.

What carries over and what doesn't: colours and fonts swap in minutes; drawings are
redrawn, not recoloured; the layout (two-page spreads, cards pasted on pages) only
suits styles that are also books. The mock-ups in `web/mockups/` do not follow this
(each is a standalone page with its own copies); the guide itself does.

---

## Design principles from The Hero's Journal (agreed 2026-11-26)

A gamified fantasy journal (91 days; tasks become quests, a hero moves along a
journey, colour-in art). Used as a model for coherence, not for its layout or style.
The owner has a copy; reference photos to come.

1. **One metaphor, used everywhere.** Every element is named in the expedition's
   terms, and the names are consistent (today the guide mixes stop, chapter, lesson,
   page, Handbook, notebook). A glossary to agree: started 2026-11-27 ("waypoint" for a stop; see *Waypoints*).
2. **Progress shown as a place:** the map, with the route drawn in as the child goes.
3. **The same shape every stop,** so it becomes a habit: Watch, Describe, Look inside,
   Try it, and the record.
4. **Theme, structure and free space in balance:** room on each spread for the
   child's own notes and drawings.
5. **The story unfolds with progress:** the prologue sets the quest; each stop adds a
   page of story.
6. **Mechanics, not just mood** (the journal's main criticism is that its theme is
   "vibes"): the game parts must do something. The agent checklist ticks off, the
   route unlocks stops, stickers are earned for something real, and the log holds
   the child's real results.

Not borrowed: its customisation (colouring in). The notebook already has its own:
the cover colour (superficial), and stickers that mark progress like Pokémon badges
(owner, 2026-11-26; not fully thought out yet).

---

## Visual consistency (2026-11-26, in progress)

The guide as built speaks eight visual languages (`web/mockups/guide/visual_audit.png`).
Decided by the owner:
- **Earned colour:** the notebook's own drawings stay ink (and pale wash, if chosen);
  full, bright colour belongs only to what the explorer collects or adds: stickers,
  the cover, the route.
- **Controls are notebook objects:** tabs, stamps, paper tags, pencilled boxes to tick;
  no web-app buttons.
Open: the one drawing technique for everything in the world (Val, creatures, places,
the map, diagrams, the observation windows): ink line + soft wash, or pencil sketch in
the owner's style (`web/mockups/guide/val_two_styles.html`). Either way Val's 16-bit
pixel art leaves the pages (it can stay on Val's own 5x5 face).

**Drawing conventions (agreed 2026-11-27)**, so the owner's hand-drawn art and the
code-drawn art (Val, creatures, the map) read as one hand; the owner is redrawing
the prologue sketches with these:
- **Line:** one pen for everything (a 0.3 or 0.5 mm fineliner); a second, heavier pen
  only for outer outlines. One confident stroke per edge, no going over it (re-tracing
  is what makes a drawing sketchy). Close the shapes, so washes and hatching fit.
- **Shading:** left out of the line drawing; the chosen style adds it (washes, or
  hatching). If hatching by hand: one direction (45°, top right to bottom left) and
  one spacing.
- **Shapes and labels:** a few repeated forms (circles for worlds and cells, rounded
  rectangles for Val's parts); drawings at a similar size, so line weights match;
  short labels in the same handwriting, beside the drawing, not inside it.
- **Paper and capture:** plain or dot-grid paper; a scan or flat photo in even light.
  The cleaning step (trace to vector, remove specks) and either style can then be
  added automatically.

---

## The opening (updated 2026-11-27)

Rebuilt to the design principles (owner, 2026-11-27), in `web/guide/`: the cover was too
much a video game's box, so it is now a field journal (a printed label with a small
ink-and-wash picture, the Ci pressed in; bright colour earned later as stickers). The
map follows the owner's own drawing (and will follow the redraw). The prologue now sits
between the inside cover and the map: cover → inside cover → prologue (3 spreads) → map
→ Meet Val. Val is drawn from above in ink and wash until the owner redraws it from the
full assembly CAD (its wheel is now under the body).

### Earlier notes on the opening (2026-11-22)

In the order a child meets it; mock-up `web/mockups/guide/opening_game.html`.

1. **The cover promises a game,** like a video game's box. Two options: an adventure
   picture pasted on the notebook's leather (Val in front, the creatures, a light to
   chase), or a cover covered in stickers of Val and the creatures. The notebook
   itself is modelled on the owner's real one (dot-grid pages; six cover colours the
   child may choose; the Ci mark).
2. **The inside cover expands the promise:** "On this journey you will…" (build a
   robot that grows, meet the creatures, find out how a brain can be made of a few
   wires, a challenge at every stop, a sticker for every place, write what you
   discover), and "This notebook belongs to".
3. **The map is the first spread,** as in a fantasy book: the places and their
   creatures, with no path yet; the route is drawn in as the child travels. The
   owner is drawing the geography (see *The map* below).
4. **Meet Val** (stop 0), then on to Vehicle 1.

Decided 2026-11-22 and built in `web/guide/`: the cover is a **mix, leaning on the
stickers** (the adventure picture as a postcard among them; both pure options kept,
`?cover=stickers` / `?cover=picture`); the map is drawn from the owner's river
sketch until the redraw; **the setup cards teach the button** (Meet Val hands over
to a "Val's button" spread with the touch glow); **Meet Val leads back to the map**,
where the route to stop 1 is drawn in. Still open: the promises' wording, and all
story text. The full
list is under *The opening sequence* in `FUTURE_WORK.md`.

---

## Every chapter has the same shape

| part | for | what it is |
|---|---|---|
| **Story** | child | A short scene: where we are in life's history and what Val gets next. |
| **Build** | child + adult | What to plug in or move. One picture per step. |
| **Predict** | child | "What will Val do?" Written or drawn in the notebook *before* trying. |
| **Try** | child | Run it. The face reacts to what the sensors notice. |
| **Notice** | child | Prompts: was the prediction right? What surprised you? |
| **Name it** | child | The scientific word, after they've seen it (kinesis, taxis, inhibition...). |
| **Wonder** | both | A question about animals or people. No answer given. |
| **Grown-up notes** | adult | Setup, what's being learned, questions to ask, what usually goes wrong, going further. |

Since then (2026-11): each stop may also end in a **challenge** with that stop's
robot, and **open-ended play** in a home-made version of where its creatures live;
the child collects a **sticker** per place, like passport stamps; **setup cards**
show each screen Val will show (Vehicle 1 drafted). Grown-up notes become the
stop's page in *The Expedition Handbook*.

Predict-before-try is the core habit. It is the lab-notebook part, and the
notebook is what gets saved (see *Progress* below).

---

## The progression

### The path (tentative, 2026-11-12; updated here 2026-11-22)

| stop | robot | body | firmware |
|---|---|---|---|
| 0 | Meet Val | one module, no sensors | exists |
| 1 | Vehicle 1 | one module, one wheel, one sensor | `Val_Vehicle`, exists; lesson filmed |
| 2 | Vehicles 2a / 2b | two modules joined, one wheel each | `Val_Vehicle` on each half; confirmed convincing on the robot by the owner |
| 3 | Vehicles 3a / 3b, ending on 3c (several senses, mixed wires) | as 2 | as 2 |
| 4 | Habituation | to decide | not built; maybe the same neuron with a slow negative echo |
| 5 | Vehicle 4 (4a a favourite level, 4b a threshold) | one module; the orbit needs two | `Neuron` in `Val_Vehicle` (branch `expanded_vehicles`); first run on the robot recognisable |
| 6 | Potential field | one module, two wheels, up to 3 sensors | `Val_Field`, exists |
| 7 | Vehicle 5 (holding a state: memory) | one module | `Neuron` listening to itself; first run recognisable; its off switch designed, not built |
| 8 | Subsumption | one module, two wheels, PaHub | `Val_Brooks`, a test bench (hierarchy set in the sketch, not yet on the robot) |
| 9 | Vehicle 7 (learning by association) | to decide | not built |
| 10 | Vehicle 6 (selection) | probably a web game | not built |

Vehicles 8 to 14 go in the appendix; a "frontier" of explorations that combine
ideas may follow stop 10.

**Creatures (decided 2026-11):** each stop shows a living animal from as old a
lineage as possible; the Handbook names the ancient ancestor. Body plans: no sides
(radial: jellies, Vehicle 1), then bilateral from Vehicle 2. Decided so far: the
jellies. Candidates from the owner's notes: the roundworm (dwelling and roaming),
ants (following a trail), the woodlouse. Floor colour stands for taste and smell
(chemicals on the ground), not colour vision.

### Depth at each waypoint (principle, owner, 2026-11-28; details not decided)

The waypoints felt too shallow. Depth should come from investigating, not from more
explaining: not a game woven into a textbook. Explanations arrive after the child has
seen something and has a question; the text stays short. Ideas to draw on (proposals,
not decided):
- a mystery that opens the waypoint, too big for one run;
- several field trials, each changing one thing (wire, lamp, drive, start, floor),
  predicted, run and recorded, each adding a clue;
- the child's own measurements (tallies, timings, hand-drawn charts);
- a real animal where possible (the woodlouse choice chamber, a classic school
  experiment), alongside Val;
- a challenge that needs the understanding to solve (park Val in the shade);
- optional deeper side trails for keen explorers; later waypoints look back at earlier.

**Each waypoint ends with a Creature Report** (owner, 2026-11-28; after the Octonauts'
creature reports): what the explorer observed, with options to choose from, and room
and encouragement for free writing. Then the expedition's question: **"Is Val an
agent?"** And the same question asked of the creatures Val imitates. The answers can
differ: a jelly regulates its own life (Di Paolo's sense of agency, see the prologue
notes) where Val, acting with no needs of its own, may not; and from waypoint to
waypoint the answers can change, which is the expedition's thread (the checklist:
navigate, communicate, manipulate).

### Waypoints: robots paired with creatures (2026-11-27)

**"Waypoint" is the guide's word for a stop on the expedition** (agreed with the owner,
2026-11-27; the first entry of the expedition glossary). Older text says "stop".
As far as the current proposals go; firmness as in the section below.

| # | waypoint | robot (what Val is) | creature(s) | how firm |
|---|---|---|---|---|
| 0 | Meet Val | a face only: it can't feel or move | none: the prologue's cell ("alive, but not agentic") | place decided |
| 1 | Vehicle 1 | one sensor, one wheel, no sides; pulse drive | jellies (pulse drive), the ancient stage; woodlouse (smooth drive), the living cousin; roundworm on a food patch, a floor-colour variant | jellies decided; pulse drive accepted 2026-11-24; pairing of drives decided 2026-11-25 |
| 2 | Vehicles 2a / 2b | two joined modules, uncrossed / crossed + wires: turns away or towards | flatworm (planarian, the "coward"); ants on a scent trail; crickets to song, if Val gets microphones | proposed / recorded idea / proposed |
| 3 | Vehicles 3a / 3b / 3c | − wires: slows near and stays (love) or turns away (explorer); 3c mixes senses | ants (trail following); turtle and bee superseded | open |
| 4 | Habituation | a wire whose response fades with repetition (to design) | sea hare (*Aplysia*) | placeholder |
| 5 | Vehicle 4 | a nerve cell on the wire: threshold (4b), favourite level (4a) | none yet | open |
| 6 | Potential field | two wheels, pushes and pulls summed; perhaps a literal hill via the IMU | desert ants (Wehner: a weighted sum of cues) | proposed, best sourced |
| 7 | Vehicle 5 (memory) | a nerve cell that holds its state; another wire switches it off | roundworm: lasting dwell / roam states | recorded idea |
| 8 | Subsumption | behaviours in layers (`Val_Brooks`) | an early jawless fish (Long's fish robot) | proposed |
| 9 | Vehicle 7 (association) | learns to link two sensors | bees learning colours | placeholder; no source |
| 10 | Vehicle 6 (selection) | probably a web game: wirings copied with mistakes | none yet | open |
| beyond | the frontier | combined ideas; Vehicles 8 to 14 in the appendix | | |

Gaps: waypoints 3, 5 and 10 have no creature; 4 and 9 only placeholders. The cover
label's bottom path (cell, jelly, flatworm, ant, fish) follows waypoints 0, 1, 2, 6, 8.

### Creatures: where the decisions stand (2026-11-23)

Rebuilt from the repo, the session's history (mostly 2026-11-10 to 2026-11-12) and the
readthrough of the owner's sources (2026-11-10; the summaries, synthesis and gaps
files are kept outside the repo). Creature choices have waited on purpose: the robots
come first (owner, 2026-11-12).

**The rules (firm, the owner's):**
1. A living animal per stop, from as old a lineage as possible; the ancient ancestor
   in the Handbook. Some choices serve both (2026-11-10).
2. Body plans: radial first (jellies), then bilateral from Vehicle 2, which "opens
   doors" for more complex nervous systems (2026-11-10).
3. The robots are a coarse sampling of the tree; the appendix gives the depth
   (2026-11-11).
4. Creatures, habitats and the map are chosen after the robots (2026-11-12).
5. The Seminal Sea is the start. The owner names the places. If the woodlouse is the
   first creature, woods come down to the sea (2026-11-10).
6. Floor colour is taste and smell, not vision; the light-based creature ideas are
   to be revisited with that in mind (2026-11-18).

**Firmness:** *decided* (the owner said so or agreed); *recorded idea* (in
`FUTURE_WORK.md`, not confirmed); *proposed* (suggested, never answered);
*superseded*; *placeholder* (only on a mock-up or the guide's map).

| stop | creature(s) | firmness | notes |
|---|---|---|---|
| 0 Meet Val | none: chemistry in the early sea; energy before sensing (Lane); a living thing regulating what it lets in (Di Paolo) | place decided (Seminal Sea); no creature proposed | |
| 1 Vehicle 1 | jellies, as the stage and body plan | decided | Owner: "I don't have a jelly-analog in my robots." Proposed: three roles (jellies the stage; Val V1 the robot, "no sides yet"; the woodlouse the living animal to test). Not confirmed. |
| 1 | woodlouse | built into the V1 lesson mock-ups; as the stop's creature, unsettled (it is bilateral) | Early mock-ups called stop 1 "The Tide Pools". |
| 1 | roundworm (*C. elegans*) slowing on food (a − wire on a floor colour); bacteria as the oldest version | recorded idea | biology: high confidence |
| 2 Vehicles 2a/2b | flatworm (planarian): old bilateral lineage, two eyespots, turns from light ("coward") | proposed (2026-11-10) | |
| 2 | cockroach (2a), moth (2b) | superseded | |
| 2–3 | ants following a scent trail with two antennae (floor strip, a Color Unit per module) | recorded idea | trail following high confidence; left/right comparison the whole story: medium |
| 2 | Kinsbourne (source 04): uncrossed is the invertebrate plan, crossed the chordate, the crossing perhaps a by-product | a framing, not a creature | |
| 2–3 | crickets turning to song (Webb's robot crickets), if Val gets microphones | proposed (2026-11-23) | |
| 3 Vehicles 3a/3b/3c | basking turtle or lizard ("love"), bee ("explorer") | proposed, then effectively superseded by the radial/bilateral rework | |
| 4 Habituation | sea hare (*Aplysia*) | placeholder (map only) | sources: habituation is ancient, even in single cells |
| 5 Vehicle 4 | none | | |
| 6 Potential field | desert ants (Wehner): a weighted sum of cues, no central decider | proposed as the best sourced match (2026-11-10) | replaced salmon (a metaphor only); ideas: a literal hill via the IMU, a wolf's long-range scent |
| 7 Vehicle 5 (memory) | roundworm: lasting dwell / roam states, ended by a change in what it senses | recorded idea (memory's off switch) | rat in a maze, in caves: superseded when memory moved earlier |
| 8 Subsumption | an early jawless fish (Long's fish robot, source 15: forage underneath, escape on top) | proposed (2026-11-10) | mountain goat earlier (unsourced); Gaveau's two reaching pathways for the Wonder; no Brooks source in the collection |
| 9 Vehicle 7 (association) | bees learning colours | placeholder (map only) | no source on associative learning |
| 10 Vehicle 6 (selection) | none | | probably a web game |

Not a stop, decided: gravity is a sense Val has from early on; jellies and comb jellies
(source 05) steer by gravity organs.

**Holding the rest up:** the corrected synthesis from the owner's terminal session
(asked for 2026-11-10: four errors, and the stages rebuilt as a real timeline); the
unbuilt robots (habituation, Vehicles 5, 6, 7); gaps in the sources (kinesis in real
animals, jellyfish themselves, Brooks, potential fields, associative learning).

In short: the rules are firm; the only creature fixed to a stop is the jellies at
Vehicle 1, with an open question of how the robot stands in for them.

**Vehicle 1 as the jellies (owner, 2026-11-23):** the jellies are the imperfect
ancient version; Vehicle 1 is the jelly principle with steering taken out (one
feeling, one push, no sides; the world picks the heading). Val gets both a regular
(smooth) drive and a pulse drive, in which the light sets how often it pushes; which
one a lesson uses is **not set yet**. Mock-up: `web/mockups/v1-lesson/pulse_drive.html`.
**Decided 2026-11-24:** stop 1 uses the pulse drive (push and glide), tested on Val.
**Built 2026-11-25** in `web/guide/` (owner's choices): the woodlouse is the living
cousin a child can watch; the stop is the lesson's four steps as spreads (Watch, What
are they doing?, Look inside, Try it), after an opening spread with the jellies and
what to build; Val in the windows is simulated (pulse drive) until real runs, filmed
from above with Val running straight, replace it.

The chapters below were written for the first six stops (2026-10) and are kept as
they were; stops 4, 5, 7, 9 and 10 have no chapter yet.

Hardware needed changes along the way, so each chapter lists it.

### 0. Meet Val

- **Life:** before there was anything that sensed, there was only chemistry.
- **Val:** Val wakes up. It has a face and nothing else: it can't feel or move yet.
- **Build/try:** power on, hear "Hello, my name is, Val", learn the button
  (short = change, long = confirm, hold 2 s = setup). Look at every face.
- **Grown-up notes:** flashing the firmware (once, by an adult), charging, what
  the face's expressions mean, safety around servos and small parts.
- **Hardware:** one Matrix + Motion. **Firmware:** exists.

### 1. Vehicle 1: Getting around (one sensor, one motor)

- **Life:** the first single cells. A bacterium doesn't steer; it just goes
  faster or slower depending on what it senses. That's **kinesis**.
- **Val:** gets one sense (light or distance) wired to one wheel.
- **Try:** + wire (more light → faster), then − wire (more light → slower). Where
  does Val end up spending its time?
- **Name it:** sensor, motor, excitatory, inhibitory, kinesis.
- **Wonder:** Val has no idea where the light is. So how does it end up in the dark?
- **Grown-up notes:** calibration (low/high points in setup); why a straight
  line of behaviour can still look purposeful; friction and wobble make V1 look
  "alive" (Braitenberg's own point).
- **Hardware:** one Matrix + Motion, one servo (S1), one sensor.
  **Firmware:** `Val_Vehicle`, exists.

### 2. Vehicles 2a and 2b: Fear and aggression (two sides, + wires)

- **Life:** bodies with a left and a right (bilateral animals, early worms). Two
  eyes, two sides, and now you can *turn*. That's **taxis**.
- **Val:** two sensors, two wheels. Same + wires as before, but now it matters
  *which* side goes to *which* wheel.
- **2a, straight (uncrossed):** turns away from the light, and speeds off. "Coward."
- **2b, crossed:** turns toward it and charges. "Aggressor."
- **Try:** predict both before trying. Swap the cables and watch the personality
  change.
- **Name it:** taxis, phototaxis, ipsilateral/contralateral (optional, for keen ones).
- **Wonder:** we called one "scared" and one "angry". Did we build fear?
- **Hardware:** as currently conceived, **two Matrix + Motion units**, one per
  side, each running `Val_Vehicle` with one sensor and one wheel. Crossed vs
  straight = which sensor is plugged into which half.
  **Firmware:** exists for each half (my reading; see open questions).

### 3. Vehicles 3a and 3b: Love and the explorer (− wires)

- **Life:** neurons that *calm* other neurons. Inhibition is how nervous systems
  settle, stay, and stop.
- **Val:** same two halves, now with − wires (Val starts at full speed and
  slows as the stimulus grows).
- **3a, straight:** approaches, slows, stops facing the source. "Love."
- **3b, crossed:** approaches, slows, then turns away to wander off. "Explorer."
- **Try:** mix them: light on +, distance on −. Now there are values in conflict.
- **Name it:** inhibition, attraction vs. avoidance, "emergent" behaviour.
- **Wonder:** Val 3a stays by the lamp. Does it like the lamp?
- **Hardware/firmware:** as chapter 2.

### 4. Potential fields: many pulls at once

- **Life:** animals with many senses at once, each pulling a little: smell this
  way, danger that way. The body goes where the pulls add up.
- **Val:** back to **one** Val with two wheels. Each sensor pushes or pulls in
  the direction it points, and the arrows add.
- **Try:** a light that pulls and a wall that pushes: where does Val go? Draw the
  arrows first.
- **Name it:** vector, field, sum, gradient.
- **Wonder:** if you're pulled toward cake and away from the dog next to it, what
  do you do? What does Val do?
- **Hardware:** one Matrix + Motion, servos S1 (left) and S2 (right), up to three
  sensors. **Firmware:** `Val_Field`, exists.

### 5. Subsumption: layers on layers

- **Life:** evolution doesn't redesign brains; it adds new layers on top of old
  ones. The old reflexes are still there underneath (the blink, the flinch), and
  newer layers can override them, but only sometimes.
- **Val:** behaviours stacked in layers: "don't hit things" at the bottom,
  "wander" above it, "go to the light" on top. A higher layer can quiet a
  lower one; a lower one can still take over when it matters.
- **Try:** build one layer at a time, each tested before the next. Predict what
  happens when two layers disagree.
- **Name it:** layer, suppression, inhibition (again, at a bigger scale),
  subsumption, arbitration.
- **Wonder:** when you pull your hand off a hot pan before you feel the pain,
  which "you" decided?
- **Hardware:** **one** Matrix + Motion (pick one of the two from chapters 2–3),
  with a PaHub for more sensors than three ports allow.
- **Firmware:** ~~not built~~ *(stale since 2026-10-23)*: `Val_Brooks` is a test
  bench with PaHub support and a fixed hierarchy, chosen by editing one line and
  reflashing. Setting the hierarchy on the robot is mocked up, not built.

### Where state machines and ANNs could fit

*Settled by the path (2026-11-12):* state enters as Vehicle 5 (a neuron that holds
its state), before subsumption; learning enters as Vehicle 7 (association), and
evolution as Vehicle 6 (selection), last. The options below are kept as the
reasoning.

Two candidate placements for each, to choose between.

**State machines (memory: Val is in a mood, a mode)**

- **Option A, before subsumption (as "chapter 4½").** Val gets an internal state
  that changes which wiring is active, e.g. *hungry* (3b, explores) vs. *fed* (3a,
  stays). Life parallel: motivation, sleep/wake. It's a small step from the
  vehicles and makes subsumption easier, because...
- **Option B, inside subsumption.** Brooks built his layers out of small state
  machines (his "augmented finite state machines"). So state machines could be
  introduced as *what a layer is made of*.
- Either way, Val's face is already a state machine kids have been watching all
  along (asleep → surprised → happy), which is a nice "you've seen this already"
  moment.
- Braitenberg's own Vehicle 5 (threshold units that hold a state) is the bridge
  from the vehicles to memory, if a book link is wanted.

**ANNs (learning: the wiring changes itself)**

- The vehicles already **are** tiny neural networks: each wire is a weight of +1,
  −1 or 0, set by hand. So ANNs can be a thread that runs through the guide
  ("Look, it's a network!") and a final chapter where the weights stop being set
  by hand and start being *learned*.
- **Option A, last chapter: learning.** Val learns which wiring works (e.g. a
  simple Hebbian or perceptron rule, so that wires that fire together strengthen).
  Life parallel: learning within one lifetime, vs. evolution across many.
- **Option B, a parallel track.** An "under the hood" sidebar in every chapter
  redraws that chapter's wiring as a network diagram, so the ANN chapter is
  familiar when it arrives.
- Braitenberg's Vehicles 6 (selection / evolution) and 7 (association) map onto
  this too.

My suggestion, for discussion only: A then A (state machines just before
subsumption, learning last), **plus** the network sidebar from the ANN Option B,
since it costs one drawing per chapter.

---

## Progress that travels between machines

Your point from Amazed Kingdoms: a child moves between a parent's laptop, another
parent's tablet, a grandparent's, a club's. Progress on one browser is lost on the
others. Options, which can be combined:

| option | how | carries notebook text? | needs accounts/server? | works on GitHub Pages? |
|---|---|---|---|---|
| **Save file** | "Save to file" downloads `val-progress.json`; "Load" opens it. Thumb drive, email, cloud folder. | yes | no | yes |
| **Save code** | A short code (like old game passwords) or QR that encodes chapters done. Write it on paper, type it in elsewhere. | no (too long) | no | yes |
| **Browser memory** | Auto-saves on that browser, as a convenience on top of the above. | yes | no | yes |
| **Accounts / sync** | Sign in; progress on a server. | yes | yes | no, needs a backend |

Suggested start: **browser memory + save file + save code**, no accounts. The save
file holds everything (chapters, predictions, notes); the code is the fallback
when there's no thumb drive to hand. Accounts can come later if it's worth running
a server. A printed notebook page per chapter also works offline and for
homeschool records.

---

## The map (2026-11-22, being drawn)

The first spread. A first mock-up put the sea on the left page and the land on the
right (oldest life to newest), with no path. The owner's sketch (2026-11-22) adds a
river from the mountains, through a lake, to the sea. Notes given on it:

- **The river is the path without being drawn:** the journey goes upstream, from the
  sea to the source, which is also oldest life to newest.
- **Bands of country order the stops:** sea, shore, a ridge, grassland, the lake,
  forest, mountains. The ridge can be a gate (onto land, or from vehicles to fields
  and layers); the lake a midway hub (fits the potential field); the mountains the
  end, with "Uncharted" beyond.
- **Watch:** room for eleven places along the river (bends, a ford, a waterfall,
  reed beds); Val's Workshop at the river mouth, so the way upstream is clear;
  nothing important in the spread's fold (the coast or ridge could run along it);
  the sea's share should match the number of water stops.

The owner will redraw it with these notes.

---

## Decided since this outline (2026-11)

- **Tone: expedition.** An old travel map (Indiana Jones style) with a red route;
  each chapter is a stop with a short story scene, and the guide opens with a
  prologue. The game feel carries the narrative; the notebook experiments are the
  player's part in the story (they fill "My expedition journal").
- **Everything is written to the child.** Adult material moves to an appendix, *The
  Expedition Handbook*, for helpers and for older or more advanced explorers, which
  the player can open any time. (So the "Grown-up notes" part of each chapter
  becomes that chapter's Handbook page.)
- Mock-ups: `web/mockups/v1-lesson/` (`Expedition.dc.html`, `Handbook.dc.html`,
  `Lesson.dc.html`).

## Open questions for the owner

*Status (2026-11-22):* 1 is settled (expedition, with the notebook as the player's
part); 2 is confirmed (two joined modules give convincing 2a/2b and 3a/3b); 3 is
answered by `Val_Brooks` (one module, two wheels, PaHub); 4 is settled by the path;
5 is settled (real living animals, with the ancestor in the Handbook); 6 is open.
The opening's own open decisions are under *The opening*.

1. **Tone and frame.** Three you like, and they may combine rather than compete:
   - *Stranger in a strange land*: Val is newly arrived and gets its senses in
     life's order. Gives the **story**.
   - *Lab notebook*: predict / try / notice in every chapter. Gives the **habit**.
   - *Expedition*: the chapter map is a "tree of life" or timeline that lights up
     as chapters are done. Gives the **game**.
   A next step could be a mock-up of one chapter (chapter 1) in two or three tones
   to choose from.
2. **Two-controller vehicles (chapters 2–3).** Is my reading right: each half runs
   `Val_Vehicle` with one sensor and one wheel, and crossing = swapping which
   sensor goes on which half? (`FUTURE_WORK.md` also lists a one-controller
   two-wheel vehicle with a "which wheel" step; is that dropped, or a later
   alternative for families with one kit?)
3. **Subsumption on one controller with a PaHub:** confirm that's the plan, and
   whether the two-wheel Val from chapter 4 is the body.
4. **Placement of state machines and ANNs** (above).
5. **How much biology to name.** Real organisms (E. coli, planaria, moths, frogs)
   or keep it general ("the first cells", "the first worms")?
6. **Reading level.** Same text for an 8-year-old and a 12-year-old, or a short and
   a longer version of each story?
