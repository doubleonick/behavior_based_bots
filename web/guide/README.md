# The guide (web/guide)

The web guide itself, as far as it is built: the notebook's cover, the inside
cover, the map, Meet Val (stop 0) and Val's button. Started 2026-11-22. Open
`index.html` in a browser; nothing to install (fonts come from Google Fonts).

It follows the design principle in `web/OUTLINE.md`: style, content and art are kept
apart, so any one can change without touching the others.

| layer | where | holds |
|---|---|---|
| style | `style/notebook.css` | every colour, font and layout rule (the notebook look). A new look is a new stylesheet. |
| content | `content/opening.js` | every word, the places on the map, which creatures go where, the stops and the route. `[Square brackets]` mark placeholders. |
| art | `art/notebook/` | the notebook look's drawings, one file each: the Ci and its tile, the wordmark's gear and bulb, creatures (ink), stickers, icons, the cover picture, the map. `make_art.py` writes them. |
| art | `art/val/` | Val's avatar (from `cad/render/`), shared by every look |
| layout | `index.html`, `app.js` | empty frames, and the code that fills them from the content and art. No words, no colours. |

## Pages

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

## Not yet

Saving that travels between machines; the sticker collecting; the Handbook; stop 1
onward; the story text (all placeholders).
