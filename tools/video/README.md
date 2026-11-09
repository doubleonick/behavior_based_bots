# Video tools: from a phone clip of Val to the guide

1. **Film** as in `web/mockups/v1-lesson/Filming.dc.html`: phone straight overhead and
   still, plain light floor, a light fixed above (one brightness setting), exposure
   locked, a couple of seconds of empty floor first, the whole run in frame. Dead
   time at either end is fine; it gets trimmed.
2. **Convert and trim** (HDR to ordinary colour, 1280 wide, no sound, optional
   quarter turn so the light ends up on the right):

       tools/video/convert_clip.sh IMG_2173.MOV web/media/v1/v1_minus.mp4 13.5 18.0
       tools/video/convert_clip.sh IMG_2172.MOV web/media/v1/v1_plus.mp4 5.2 8.73
       tools/video/convert_clip.sh V1_neg_wire.mov web/media/v1/v1_minus_tail.mp4 8.6 13.9 cw

3. **Track** Val (by its blue battery) and measure the pool of light:

       python tools/video/track_clip.py web/media/v1/v1_minus.mp4 minus.json --picture minus.png

   `--floor` takes a still of the empty floor for the pool (better when Val starts
   in the light). `--picture` draws the path with a dot every 0.2 s.
4. The lesson page replays every other point of the path in its round lens; the
   mapping is in `track_clip.py`'s header.

| clip | from | wire | what Val does |
|---|---|---|---|
| `v1_plus.mp4` | IMG_2172, 5.2–8.7 s | + | comes out of the light fast, stops at the edge of the pool |
| `v1_minus.mp4` | IMG_2173, 13.5–18.0 s | − | crosses the dim floor, stops in the light |
| `v1_minus_tail.mp4` | V1_neg_wire, 8.6–13.9 s, turned clockwise | − | slows in the light but drives on out of frame (a second take) |

Needs ffmpeg (with the zscale filter) and Python 3 with numpy and Pillow.
