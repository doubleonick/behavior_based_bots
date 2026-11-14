# Setup and display mock-ups (the 5x5 matrix)

Single web pages that pretend to be Val's matrix and one button, made to choose a
design before any firmware is written. Open a file in a browser; nothing else is
needed. **They are mock-ups, not firmware:** the maths in them copies the sketches',
but nothing here runs on the robot.

| file | what it tries out | state |
|---|---|---|
| `neuron_setup_mockup.html` | setting up a wire's nerve cell on Val (Vehicles 4 and 5): Place, Threshold (show Val, then "Try it"), Strength, Self, then driving | **agreed (2026-11), not built** |
| `neuron_display_mockup.html` | two views of a cell while driving: the wiring view (where the cell sits) and a zoomed-in cell (nine squares for how hard it fires, a loop for memory) | an idea on the table; the driving view is not decided |
| `subsumption_setup_mockup.html` | choosing Val_Brooks' layers: (A) on Val, bottom-up with the one button; (B) planned in the guide, with a prediction | A is workable; B waits on the guide talking to Val (tentative, over USB) |

Decisions and open questions are recorded in `FUTURE_WORK.md`.
