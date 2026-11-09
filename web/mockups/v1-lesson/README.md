# Vehicle 1 lesson: design mock-ups

The source of the design canvas "Val V1 Lesson Style" (a Claude design artifact,
private to the owner). Each `.dc.html` file is one board of that canvas; `canvas.json`
lays them out. They are **mock-ups, not the guide itself**: images and clips in them
point at the artifact's own stored copies (`/_blob/...` addresses), which only load
inside the artifact. The clips are also kept here in `web/media/v1/`, and Val's
avatar in `cad/render/val_v1_above_rear.png`.

| board | what it is |
|---|---|
| `Lesson.dc.html` | **the working Vehicle 1 lesson**: Watch, What are they doing?, Look inside, Try it (predict, run, check, notebook log). A pinned observation deck shows Val, redrawn from its real tracked run, beside a simulated woodlouse under the same pool of light, both painting where they spent their time |
| `Cartoon.dc.html` | "Same rule, same trail": the + and − runs side by side with the woodlouse, and a "show the real clip" switch |
| `Expedition.dc.html` | **the expedition map** (Indiana Jones style): the prologue and a short story scene per stop, the red route, Val's position, "My expedition journal". All story text is placeholders |
| `Handbook.dc.html` | **the appendix** for helpers and advanced explorers: per stop, what you're finding out, setting up, questions, if something goes wrong, going further. Stop 1 drafted from the lesson's notes; the rest placeholders |
| `StyleB.dc.html` | the chosen look: notebook pages, round observation window, inks and type |
| `Filming.dc.html` | how to film Val for tracking |
| `ValRenders.dc.html` | Val V1 rendered from the CAD (A+ is the avatar) |
| `ValViews.dc.html` | hand-drawn views, before the CAD renders |
| `Main.dc.html`, `Layout.dc.html` | the first, plainer look (superseded) |

Decisions behind the lesson are recorded in `FUTURE_WORK.md` (the web guide section).

How a new clip becomes part of a lesson: `tools/video/README.md`.
