# The guide (web/guide)

The web guide itself, as far as it is built: the notebook's cover, the inside
cover, the map, Meet Val (stop 0) and Val's button. Started 2026-11-22. Open
`index.html` in a browser; nothing to install (fonts come from Google Fonts).

It follows the design principle in `web/OUTLINE.md`: style, content and art are kept
apart, so any one can change without touching the others.

| layer | where | holds |
|---|---|---|
| style | `style/notebook.css` | every colour, font and layout rule (the notebook look). A new look is a new stylesheet. |
| content | `content/opening.js`, `content/stop1.js` | every word, the places on the map, which creatures go where, the stops and the route. `[Square brackets]` mark placeholders. |
| art | `art/notebook/` | the notebook look's drawings, one file each: the Ci and its tile, the wordmark's gear and bulb, creatures (ink), stickers, icons, the cover picture, the map. `make_art.py` writes them. |
| art | `art/val/` | Val's avatar (from `cad/render/`), shared by every look |
| layout | `index.html`, `app.js`, `stop1.js`, `tray.js` | empty frames, and the code that fills them from the content and art (`app.js` the opening; `stop1.js` stop 1's spreads; `tray.js` the round observation windows). No words, no colours. |

## Pages

Since 2026-11-27: `#cover` → `#inside` → `#prologue1` → `#prologue2` → `#prologue3` →
`#map` → … The cover is a field journal (a printed label with the Earth-and-Moon picture
from the prologue, the Ci pressed into the leather; stickers are earned later); the
earlier covers stay available with `?cover=mix`, `stickers` or `picture`. The map is the
owner's own drawing (`art/notebook/map-owner.jpg`, cleaned: ruled lines removed, pen and
pencil kept), with places set on it in its own pixels; more places come with the owner's
redraw. Val everywhere is the ink-and-wash drawing from above (`art/val/val-top.svg`),
until the owner's redraw from the full assembly CAD.

Stop 1 (added 2026-11-25): `#stop1` (the jellies, and what to build) → `#watch` → `#describe` → `#look` → `#try`, carried over from the Vehicle 1 lesson mock-up. Val in the windows is simulated with the pulse drive, until real runs filmed from above replace it; the woodlouse is the living cousin. Stop 1 opens from the map once Val has been met.


`#cover` → `#inside` → `#map` → `#meet-val` → `#button` → back to `#map`, where the
route to stop 1 is now drawn in. The view is kept in the address, so any page can be
linked to.

- **The cover:** the mix chosen by the owner (2026-11-22): stickers lead, with the
  adventure picture as a postcard among them. Both pure options are kept: add
  `?cover=stickers` or `?cover=picture` to the address to see them. The explorer
  chooses the cover's colour (six).
- **The map:** drawn from the owner's sketch (sea, shore, ridge, grass, a river from
  the mountains through a lake to the sea) until the owner's redraw replaces
  `art/notebook/map.svg`. Places and creatures come from the content; only Val's
  Workshop opens yet.
- **Remembered in this browser only:** the explorer's name, the day it was begun, the
  cover colour, the prediction on Meet Val, and whether Val has been met. Add
  `?reset` to the address to start again.

## Checking it

`node tests/web/clickthrough.js` (and `... 390` for a phone) clicks from the cover to
the map and back, as a reader would, and fails if any step can't be clicked.

## Not yet

Saving that travels between machines; the sticker collecting; the Handbook; stop 2
onward; Vehicle 1's setup cards inside the guide; Val's real runs in the windows; the
story text (all placeholders).
