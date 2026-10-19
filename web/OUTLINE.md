# Web guide: outline (draft for the owner to edit)

Status: **outline only, nothing built.** Tone and frame are still open (see the end).
Everything here is a proposal to cut, reorder or rewrite.

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

Predict-before-try is the core habit. It is the lab-notebook part, and the
notebook is what gets saved (see *Progress* below).

---

## The progression

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
- **Firmware:** **not built.** Needs PaHub support and fixed arbitration (both in
  `FUTURE_WORK.md`).

### Where state machines and ANNs could fit

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

## Open questions for the owner

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
