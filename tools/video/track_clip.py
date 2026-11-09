"""
track_clip.py - follow Val through a clip and save its path, for the guide's drawings.

    python tools/video/track_clip.py CLIP.mp4 OUT.json [--floor EMPTY_FLOOR.png] [--picture OUT.png]

CLIP.mp4 should come from convert_clip.sh (ordinary colour, 1280 wide, light on the
right). Val is found in each frame by its blue battery, which stands out from a plain
light floor. The path is smoothed a little, because the battery's outline flickers
in dim light.

OUT.json holds, in the coordinates of a 640 x 360 copy of the frame:
    fps      frames per second of the clip
    path     [x, y] for every frame (frames before Val appears copy its first spot)
    pool     [x, y, radius] of the pool of light: the centre of the brightest pixels,
             and how far out the brightness has fallen halfway to the floor's
--floor     a still of the empty floor (best), otherwise the first frame is used
            (fine when Val starts away from the light)
--picture   also save the path drawn on the last frame, with a dot every 0.2 s
            (dots far apart = fast, close together = slow)

The lesson page puts the path into its round lens by
    lens_x = 180 + (x - 320) * 0.56 - 40,   lens_y = 180 + (y - 180) * 0.56
(the -40 moves the pool into the lens) and replays every other frame.

Needs: Python 3 with numpy (and Pillow for --floor / --picture), and ffmpeg.
"""
import argparse
import json
import subprocess
import numpy as np

W, H = 640, 360


def read_frames(clip):
    raw = subprocess.run(['ffmpeg', '-hide_banner', '-loglevel', 'error', '-i', clip,
                          '-vf', f'scale={W}:{H},format=rgb24', '-f', 'rawvideo', '-'],
                         check=True, capture_output=True).stdout
    return np.frombuffer(raw, np.uint8).reshape(-1, H, W, 3)


def clip_fps(clip):
    rate = subprocess.run(['ffprobe', '-v', 'error', '-select_streams', 'v:0', '-show_entries',
                           'stream=r_frame_rate', '-of', 'csv=p=0', clip],
                          check=True, capture_output=True, text=True).stdout.strip()
    num, den = rate.split('/')
    return float(num) / float(den)


def find_battery(frame):
    """Centre of the blue battery, or None if too little of it shows."""
    r, g, b = (frame[..., k].astype(int) for k in range(3))
    blue = (b > r + 45) & (b > g + 15) & (b > 90)
    if blue.sum() < 30:
        return None
    ys, xs = np.nonzero(blue)
    return xs.mean(), ys.mean()


def find_pool(lum):
    """Centre of the brightest pixels, and the radius where brightness is halfway."""
    hi = lum >= np.percentile(lum, 97)
    ys, xs = np.nonzero(hi)
    cx, cy = xs.mean(), ys.mean()
    yy, xx = np.mgrid[0:H, 0:W]
    dist = np.hypot(xx - cx, yy - cy)
    half = (np.percentile(lum, 10) + np.percentile(lum, 99)) / 2
    ring = (lum >= half - 5) & (lum <= half + 5)
    return float(cx), float(cy), float(np.median(dist[ring]))


def smooth(values, k=7):
    padded = np.pad(values, (k // 2, k // 2), mode='edge')
    return np.convolve(padded, np.ones(k) / k, mode='valid')


def draw_picture(frame, path, fps, out):
    from PIL import Image, ImageDraw
    img = Image.fromarray(frame).convert('RGB')
    d = ImageDraw.Draw(img, 'RGBA')
    d.line([tuple(p) for p in path], fill=(224, 122, 46, 230), width=4)
    every = max(1, round(fps * 0.2))
    for i in range(0, len(path), every):
        x, y = path[i]
        d.ellipse((x - 5, y - 5, x + 5, y + 5), fill=(255, 255, 255, 240), outline=(29, 42, 68, 255), width=2)
    x0, y0 = path[0]
    d.ellipse((x0 - 9, y0 - 9, x0 + 9, y0 + 9), fill=(31, 138, 76, 255))
    x1, y1 = path[-1]
    d.rectangle((x1 - 9, y1 - 9, x1 + 9, y1 + 9), fill=(194, 54, 74, 255))
    img.save(out)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('clip')
    ap.add_argument('out')
    ap.add_argument('--floor', help='a still of the empty floor, for the pool of light')
    ap.add_argument('--picture', help='also save the path drawn on the last frame')
    args = ap.parse_args()

    frames = read_frames(args.clip)
    fps = clip_fps(args.clip)
    spots = [find_battery(f) for f in frames]
    found = [s for s in spots if s is not None]
    if not found:
        raise SystemExit('Val (its blue battery) was not found in any frame')
    print(f'{len(frames)} frames, Val found in {len(found)}')

    # Frames before Val appears copy its first spot; frames where it is lost copy the last
    first = found[0]
    last = None
    filled = []
    for s in spots:
        if s is not None:
            last = s
        filled.append(last if last is not None else first)
    a = np.array(filled, float)
    path = np.c_[smooth(a[:, 0]), smooth(a[:, 1])]

    if args.floor:
        from PIL import Image
        floor = np.asarray(Image.open(args.floor).convert('RGB').resize((W, H))).astype(float)
    else:
        floor = frames[0].astype(float)
    pool = find_pool(floor.mean(axis=2))

    json.dump({'fps': fps,
               'path': [[round(x, 1), round(y, 1)] for x, y in path],
               'pool': [round(v, 1) for v in pool]}, open(args.out, 'w'))
    print('wrote', args.out, '; pool', [round(v, 1) for v in pool])
    if args.picture:
        # draw on the last frame where Val was found (where it ended up)
        last_seen = max(i for i, s in enumerate(spots) if s is not None)
        draw_picture(frames[last_seen], path, fps, args.picture)
        print('wrote', args.picture)


if __name__ == '__main__':
    main()
