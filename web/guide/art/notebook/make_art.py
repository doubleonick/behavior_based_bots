"""Writes the notebook style's art files (SVG) into this folder.

The notebook look's drawings: the Ci mark and its tile, the wordmark's gear and bulb,
the creatures (in ink, and as stickers), small icons, the cover picture and the map.
Each is a plain SVG file with its colours baked in: a different style would redraw
them, not recolour them. Run:  python make_art.py   (writes beside this file)
"""
import math, os, random

HERE = os.path.dirname(os.path.abspath(__file__))
INK = '#4e3b26'      # brown ink (map, promises)
PEN = '#2f3f8f'      # blue ballpoint (water)
GREEN = '#5f8f62'    # green pencil (grass)
SLATE = '#3b3e45'    # the endpaper's slate
PALE = '#c9c9cf'     # the endpaper's pale grey
ORANGE = '#E07A2E'   # Curious Innovations orange


def write(name, body, w, h, extra=''):
    path = os.path.join(HERE, name)
    with open(path, 'w') as f:
        f.write(f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {w} {h}" width="{w}" height="{h}"{extra}>\n{body}\n</svg>\n')


# ---------- the Ci: a gear for the C, a lightbulb for the i ----------
def gear_c(R, r, tooth, teeth, gap, inset):
    pts = []
    step = 360 / teeth
    P = lambda rad, a: (rad * math.cos(math.radians(a)), -rad * math.sin(math.radians(a)))
    pts.append(P(R, gap))
    end = 360 - gap
    t = math.ceil(gap / step) * step + step * inset * 0.5
    while t + step * 0.5 <= end:
        w = step * inset
        pts += [P(R, t), P(R + tooth, t + w * 0.15), P(R + tooth, t + w * 0.85), P(R, t + w)]
        t += step
    pts += [P(R, end), P(r, end)]
    for k in range(41):
        pts.append(P(r, end - (end - gap) * k / 40))
    return 'M' + ' L'.join(f'{x:.2f} {y:.2f}' for x, y in pts) + ' Z'


GEAR = dict(R=40, r=30, tooth=5.5, teeth=14, gap=44, inset=0.4)
GEAR_BOLD = dict(R=40, r=25, tooth=7, teeth=14, gap=44, inset=0.48)


def bulb(bold, fill, mask_id):
    """Version B's bulb; its threads are cut out with a mask, so they show what is behind."""
    if bold:
        x = 32
        glass = f'M{x} -21.5 a9 9 0 0 1 5.8 15.9 l-1 3.6 h-9.6 l-1 -3.6 a9 9 0 0 1 5.8 -15.9 Z'
        base = (x - 5.5, -0.2, 11, 20.2)
        threads = [(x - 5.5, 4.2), (x - 5.5, 9.2), (x - 5.5, 14.2)]
        tw, dx, dy = 1.8, 11, 2.4
    else:
        x = 31
        glass = f'M{x} -21 a7.5 7.5 0 0 1 4.8 13.2 l-1 3.8 h-7.6 l-1 -3.8 a7.5 7.5 0 0 1 4.8 -13.2 Z'
        base = (x - 4, -1, 8, 21)
        threads = [(x - 4, 4), (x - 4, 8.5), (x - 4, 13)]
        tw, dx, dy = 1.3, 8, 2
    bx, by, bw, bh = base
    cuts = ''.join(f'<path d="M{a} {b} l{dx} {dy}" stroke="#000" stroke-width="{tw}"/>' for a, b in threads)
    return (f'<mask id="{mask_id}" maskUnits="userSpaceOnUse" x="-60" y="-60" width="120" height="120">'
            f'<rect x="-60" y="-60" width="120" height="120" fill="#fff"/>{cuts}</mask>'
            f'<path d="{glass}" fill="{fill}"/>'
            f'<rect x="{bx}" y="{by}" width="{bw}" height="{bh}" fill="{fill}" mask="url(#{mask_id})"/>')


def ci(bold, fill, mid):
    g = GEAR_BOLD if bold else GEAR
    return f'<path d="{gear_c(**g)}" fill="{fill}"/>' + bulb(bold, fill, mid)


write('ci.svg', f'<g transform="translate(56 56)">{ci(False, "#1d2a44", "t")}</g>', 112, 112)
write('ci-bold.svg', f'<g transform="translate(56 56)">{ci(True, "#000", "t")}</g>', 112, 112)
# the endpaper tile: two marks in offset rows (like bricks), pale grey on slate
tile = (f'<rect width="60" height="64" fill="{SLATE}"/>'
        f'<g transform="translate(15 16) scale(.21)">{ci(False, PALE, "a")}</g>'
        f'<g transform="translate(45 48) scale(.21)">{ci(False, PALE, "b")}</g>')
write('ci-tile.svg', tile, 60, 64)
# the wordmark's letters: the gear (as C) and the bulb (as I), orange; the words are type
R = GEAR['R'] + GEAR['tooth']
write('wordmark-gear.svg', f'<g transform="translate({R} {R})"><path d="{gear_c(**GEAR)}" fill="{ORANGE}"/></g>', 2 * R, 2 * R)
write('wordmark-bulb.svg', f'<g transform="translate(-25 21)">{bulb(False, ORANGE, "w")}</g>', 12, 41)


# ---------- creatures, each in a 100 x 100 box, drawn in one colour ----------
CREATURES = {
    'jelly': lambda c: f'''<g fill="none" stroke="{c}" stroke-width="3" stroke-linecap="round">
      <path d="M18 50 Q18 14 50 14 Q82 14 82 50 Q74 45 66 50 Q58 45 50 50 Q42 45 34 50 Q26 45 18 50Z" fill="{c}" fill-opacity=".25"/>
      <path d="M34 26 Q50 20 66 26" opacity=".6"/>
      <path d="M28 52 q-4 10 2 18 t-2 20 M42 52 q4 12 -2 22 t2 18 M58 52 q-4 12 2 22 t-2 18 M72 52 q4 10 -2 18 t2 20" stroke-width="2.4"/></g>''',
    'ant': lambda c: f'''<g fill="{c}" stroke="{c}" stroke-linecap="round">
      <ellipse cx="50" cy="24" rx="9" ry="10"/><ellipse cx="50" cy="44" rx="7" ry="11"/><ellipse cx="50" cy="72" rx="13" ry="18"/>
      <g fill="none" stroke-width="3"><path d="M44 40 L28 30 L22 34 M56 40 L72 30 L78 34 M44 46 L26 50 L20 58 M56 46 L74 50 L80 58 M45 52 L32 66 L30 78 M55 52 L68 66 L70 78 M45 17 L38 6 L30 4 M55 17 L62 6 L70 4"/></g></g>''',
    'woodlouse': lambda c: f'''<g stroke="{c}" stroke-linecap="round" fill="none">
      <ellipse cx="50" cy="52" rx="22" ry="36" fill="{c}" fill-opacity=".3" stroke-width="3"/>
      <path d="M30 32 Q50 26 70 32 M28 42 Q50 36 72 42 M28 52 Q50 46 72 52 M28 62 Q50 56 72 62 M30 72 Q50 66 70 72 M34 80 Q50 76 66 80" stroke-width="2.2"/>
      <path d="M42 18 Q34 8 24 6 M58 18 Q66 8 76 6 M44 88 l-4 8 M56 88 l4 8" stroke-width="2.6"/></g>''',
    'moth': lambda c: f'''<g stroke="{c}" stroke-linecap="round" stroke-linejoin="round">
      <path d="M48 40 L14 22 Q6 40 20 52 L46 50Z M52 40 L86 22 Q94 40 80 52 L54 50Z" fill="{c}" fill-opacity=".55" stroke-width="2.4"/>
      <path d="M46 52 L24 60 Q22 76 38 72 L48 58Z M54 52 L76 60 Q78 76 62 72 L52 58Z" fill="{c}" fill-opacity=".35" stroke-width="2.4"/>
      <ellipse cx="50" cy="52" rx="4.5" ry="18" fill="{c}"/>
      <path d="M48 36 Q40 22 32 18 M52 36 Q60 22 68 18" fill="none" stroke-width="2.2"/></g>''',
    'bee': lambda c: f'''<g stroke="{c}" stroke-linecap="round">
      <ellipse cx="34" cy="34" rx="14" ry="20" fill="{c}" fill-opacity=".2" stroke-width="2.4" transform="rotate(-30 34 34)"/>
      <ellipse cx="66" cy="34" rx="14" ry="20" fill="{c}" fill-opacity=".2" stroke-width="2.4" transform="rotate(30 66 34)"/>
      <ellipse cx="50" cy="62" rx="15" ry="22" fill="none" stroke-width="3"/>
      <path d="M37 56 H63 M36 66 H64 M40 76 H60" stroke-width="5"/>
      <circle cx="50" cy="36" r="8" fill="{c}"/><path d="M47 30 L40 18 M53 30 L60 18" stroke-width="2.4"/></g>''',
    'flatworm': lambda c: f'''<g stroke="{c}" stroke-linecap="round">
      <path d="M50 8 C62 8 70 18 66 26 C76 40 74 70 50 94 C26 70 24 40 34 26 C30 18 38 8 50 8Z" fill="{c}" fill-opacity=".22" stroke-width="3"/>
      <ellipse cx="43" cy="24" rx="3.6" ry="2.6" fill="{c}"/><ellipse cx="57" cy="24" rx="3.6" ry="2.6" fill="{c}"/>
      <path d="M50 40 V78" stroke-width="2" opacity=".6"/></g>''',
    'seahare': lambda c: f'''<g stroke="{c}" stroke-linecap="round" stroke-linejoin="round">
      <path d="M8 74 C8 58 24 48 44 50 C56 44 68 40 78 44 C88 48 92 60 88 70 C70 78 30 80 8 74Z" fill="{c}" fill-opacity=".25" stroke-width="3"/>
      <path d="M30 56 C40 44 56 44 64 52" fill="none" stroke-width="2.4"/>
      <path d="M78 44 L82 28 M72 43 L70 27 M88 62 L96 62 M86 68 L94 72" fill="none" stroke-width="2.8"/></g>''',
    'roundworm': lambda c: f'<path d="M10 60 C22 40 32 74 46 54 S70 36 80 52 S88 66 92 58" fill="none" stroke="{c}" stroke-width="4" stroke-linecap="round"/>',
    'frog': lambda c: f'''<g stroke="{c}" stroke-linecap="round" stroke-linejoin="round">
      <path d="M50 20 C66 20 74 32 72 50 C72 68 62 80 50 80 C38 80 28 68 28 50 C26 32 34 20 50 20Z" fill="{c}" fill-opacity=".25" stroke-width="3"/>
      <circle cx="40" cy="26" r="6" fill="{c}"/><circle cx="60" cy="26" r="6" fill="{c}"/>
      <path d="M30 46 L16 40 L10 46 M70 46 L84 40 L90 46 M32 72 L14 80 L18 92 M68 72 L86 80 L82 92" fill="none" stroke-width="3"/></g>''',
}
for name, draw in CREATURES.items():
    write(f'creatures/{name}.svg', draw(INK), 100, 100)

# stickers: a coloured badge with the creature on it (the white die-cut edge is added by the style)
STICKERS = {  # name: (shape, badge colour, creature colour)
    'jelly': ('circle', '#2c7f8a', '#e9fbfb'), 'ant': ('circle', '#d9a22e', '#2b1d0d'),
    'woodlouse': ('round', '#6f8a4e', '#f1ead6'), 'moth': ('circle', '#6a4c93', '#f6ecd5'),
    'bee': ('circle', '#f2cf4a', '#2b1d0d'), 'flatworm': ('round', '#8a5a44', '#f6e7d2'),
    'seahare': ('circle', '#3f6f9a', '#eaf3fb'), 'frog': ('round', '#4d7d4a', '#eef5e6'),
}
for name, (shape, bg, fg) in STICKERS.items():
    badge = '<circle cx="50" cy="50" r="50" fill="%s"/>' % bg if shape == 'circle' else '<rect width="100" height="100" rx="20" fill="%s"/>' % bg
    turn = ' transform="rotate(90 50 50)"' if name == 'woodlouse' else ''
    write(f'stickers/{name}.svg', badge + f'<g transform="translate(14 14) scale(.72)"><g{turn}>{CREATURES[name](fg)}</g></g>', 100, 100)
write('stickers/val-badge.svg', '<circle cx="50" cy="50" r="50" fill="#E07A2E"/>', 100, 100)
write('stickers/map-scrap.svg', '<rect width="120" height="76" rx="6" fill="#f0dfb4"/><path d="M12 56 C30 34 44 64 62 44 S92 30 106 40" fill="none" stroke="#9c3d1a" stroke-width="3" stroke-dasharray="4 6" stroke-linecap="round"/><path d="M98 18 l10 10 M108 18 l-10 10" stroke="#9c3d1a" stroke-width="3" stroke-linecap="round"/>', 120, 76)
write('stickers/question.svg', '<circle cx="50" cy="50" r="50" fill="#efe6cf"/><path d="M36 38 C36 22 64 20 64 38 C64 50 50 50 50 62" fill="none" stroke="#9c3d1a" stroke-width="10" stroke-linecap="round"/><circle cx="50" cy="80" r="6.5" fill="#9c3d1a"/>', 100, 100)

# ---------- small icons for the promises (in ink) ----------
write('icons/bulb.svg', f'<g transform="translate(50 54) scale(3.4)"><circle cx="0" cy="-4" r="7" fill="{INK}"/><rect x="-4" y="3" width="8" height="8" fill="{INK}"/></g>', 100, 100)
write('icons/flag.svg', f'<path d="M30 92 V10" stroke="{INK}" stroke-width="7" stroke-linecap="round"/><path d="M33 12 H82 l-12 17 l12 17 H33Z" fill="#9c3d1a"/>', 100, 100)
write('icons/star.svg', '<path d="M50 6 L62 36 L94 37 L69 57 L77 90 L50 72 L23 90 L31 57 L6 37 L38 36Z" fill="#d9a22e" stroke="#7a5a12" stroke-width="3"/>', 100, 100)
write('icons/pencil.svg', f'<g transform="rotate(35 50 50)"><rect x="40" y="6" width="20" height="62" fill="#d9a22e" stroke="{INK}" stroke-width="3"/><path d="M40 68 L50 92 L60 68Z" fill="#efe6cf" stroke="{INK}" stroke-width="3"/></g>', 100, 100)


# ---------- the cover picture (portrait): dusk, a glow to chase, sea and shore ----------
def cover_picture():
    jelly = CREATURES['jelly']('#bff1f2')
    ant = CREATURES['ant']('#e8c58a')
    louse = CREATURES['woodlouse']('#c9b9a0')
    s = '''<defs>
  <linearGradient id="sky" x1="0" y1="0" x2="0" y2="1"><stop offset="0" stop-color="#16213f"/><stop offset=".55" stop-color="#5b3f6e"/><stop offset="1" stop-color="#e98a3b"/></linearGradient>
  <radialGradient id="glow"><stop offset="0" stop-color="#fff4c8"/><stop offset=".35" stop-color="#ffd884" stop-opacity=".85"/><stop offset="1" stop-color="#ffd884" stop-opacity="0"/></radialGradient>
  <linearGradient id="sea" x1="0" y1="0" x2="0" y2="1"><stop offset="0" stop-color="#2c6b78"/><stop offset="1" stop-color="#0f2f3b"/></linearGradient>
</defs>
<rect width="200" height="340" fill="url(#sky)"/>
<g fill="#fff" opacity=".8"><circle cx="22" cy="60" r=".9"/><circle cx="60" cy="90" r=".7"/><circle cx="170" cy="70" r=".9"/><circle cx="186" cy="110" r=".6"/><circle cx="40" cy="130" r=".6"/><circle cx="130" cy="40" r=".6"/><circle cx="96" cy="24" r=".8"/></g>
<circle cx="140" cy="186" r="74" fill="url(#glow)"/><circle cx="140" cy="186" r="15" fill="#fff6d6"/>
<path d="M0 226 C40 202 66 214 98 204 C130 194 160 210 200 200 V264 H0Z" fill="#2b2547"/>
<path d="M104 214 l9 -24 l9 24 Z M118 216 l7 -18 l7 18Z M160 208 l10 -28 l10 28Z" fill="#1d1a35"/>
<g fill="#f7e7c0"><path d="M118 160 l-7 -4 l1 7 Z M118 160 l7 -4 l-1 7Z"/><path d="M162 164 l-6 -3 l1 6Z M162 164 l6 -3 l-1 6Z"/><path d="M148 140 l-5 -3 l1 5Z M148 140 l5 -3 l-1 5Z"/></g>
<rect y="232" width="200" height="110" fill="url(#sea)"/>
<path d="M0 232 C30 228 60 236 90 232 S160 228 200 233" stroke="#9fd3da" stroke-width="1" fill="none" opacity=".6"/>'''
    for x, y, w in [(12, 252, 34), (46, 280, 22), (160, 262, 26)]:
        s += f'<g opacity=".85" transform="translate({x} {y}) scale({w/100})">{jelly}</g>'
    s += '<path d="M0 318 C30 296 62 300 82 306 C110 292 146 296 200 310 V340 H0Z" fill="#14121f"/>'
    s += '<path d="M124 312 C144 296 170 292 196 284" stroke="#2a2540" stroke-width="4" stroke-linecap="round" fill="none"/>'
    s += f'<g transform="translate(150 282) rotate(-25 8 8) scale(.16)">{ant}</g><g transform="translate(170 274) rotate(-25 7 7) scale(.14)">{ant}</g>'
    s += f'<g transform="translate(20 304) rotate(70 9 9) scale(.18)">{louse}</g>'
    return s


write('cover-picture.svg', cover_picture(), 200, 340)


# ---------- the map (after the owner's sketch, 2026-11-22): sea, shore, ridge, grass,
# a river from the mountains through a lake to the sea. No labels, no creatures, no path:
# those come from the content, on top. ----------
def the_map():
    rnd = random.Random(7)
    W, H = 1040, 840
    coast = 'M300 0 C250 150 290 300 285 440 C282 560 300 640 290 700 C282 760 300 800 292 840'
    ridge = 'M470 0 C500 150 470 300 488 450 C500 560 478 700 492 840'
    s = ['<defs>',
         f'<clipPath id="seaC"><path d="M0 0 {coast[1:]} H0Z"/></clipPath>',
         f'<clipPath id="shoreC"><path d="{coast} L{ridge[-7:]} {" ".join(reversed([]))}"/></clipPath>',
         f'<clipPath id="landC"><path d="{ridge} H{W} V0Z"/></clipPath>',
         '<radialGradient id="fogG" cx="1" cy="0" r="1"><stop offset=".15" stop-color="#f2ead6"/><stop offset=".6" stop-color="#f2ead6" stop-opacity="0"/></radialGradient>',
         '</defs>']
    # the sea: ballpoint scribble, then wave marks
    sea = []
    for y in range(6, H, 9):
        x0 = rnd.uniform(-10, 10)
        pts = f'M{x0:.1f} {y}'
        x = x0
        while x < 330:
            dx = rnd.uniform(10, 18)
            pts += f' q{dx/2:.1f} {rnd.uniform(-2.5, 2.5):.1f} {dx:.1f} 0'
            x += dx
        sea.append(f'<path d="{pts}"/>')
    s.append(f'<g clip-path="url(#seaC)" fill="none" stroke="{PEN}" stroke-width=".8" opacity=".32">{"".join(sea)}</g>')
    for x, y in [(60, 120), (200, 80), (230, 380), (70, 430), (180, 520), (60, 720), (230, 760), (140, 340)]:
        s.append(f'<path d="M{x} {y} c6 -10 14 -10 16 0 c2 -8 10 -8 12 2" fill="none" stroke="{PEN}" stroke-width="2.2" stroke-linecap="round"/>')
    # the shore: pencil hatching between the coast and the ridge
    hatch = []
    for k in range(-60, 120):
        x = 250 + k * 7
        hatch.append(f'<path d="M{x} {rnd.uniform(0, H):.0f} l{rnd.uniform(18, 30):.0f} {rnd.uniform(-34, -24):.0f}"/>')
    shore_path = f'M300 0 {coast[7:]} L492 840 C478 700 500 560 488 450 C470 300 500 150 470 0Z'
    s[2] = f'<clipPath id="shoreC"><path d="{shore_path}"/></clipPath>'
    hatch = []
    for y in range(-40, H + 40, 11):
        for x in range(260, 520, 26):
            if rnd.random() < .55:
                hatch.append(f'<path d="M{x + rnd.uniform(-6, 6):.0f} {y + rnd.uniform(-4, 4):.0f} l{rnd.uniform(14, 22):.0f} -{rnd.uniform(12, 18):.0f}"/>')
    s.append(f'<g clip-path="url(#shoreC)" stroke="#6d6a66" stroke-width=".9" opacity=".55" fill="none">{"".join(hatch)}</g>')
    # the grass: green pencil strokes, densest near the ridge and the river
    grass = []
    for _ in range(1500):
        x = rnd.uniform(480, W)
        y = rnd.uniform(0, H)
        h = rnd.uniform(6, 14)
        grass.append(f'<path d="M{x:.0f} {y:.0f} l{rnd.uniform(-1.5, 1.5):.1f} -{h:.0f}"/>')
    s.append(f'<g clip-path="url(#landC)" stroke="{GREEN}" stroke-width="1" opacity=".5" fill="none" stroke-linecap="round">{"".join(grass)}</g>')
    # the coast and the ridge
    s.append(f'<path d="{coast}" fill="none" stroke="{PEN}" stroke-width="2.6"/>')
    s.append(f'<path d="{ridge}" fill="none" stroke="#3a2a1c" stroke-width="3"/>')
    s.append(f'<path d="M462 0 C492 150 462 300 480 450 C492 560 470 700 484 840" fill="none" stroke="#3a2a1c" stroke-width="1" opacity=".6"/>')
    # the river: from the mouth, over the falls at the ridge, into the lake, up to the source
    lower = 'M290 652 C330 640 370 664 410 640 C440 622 462 612 486 604 C520 594 560 560 612 520'
    upper = 'M792 448 C820 410 836 370 862 330 C890 290 888 240 914 196 C926 176 934 160 946 142'
    for d in (lower, upper):
        s.append(f'<path d="{d}" fill="none" stroke="{PEN}" stroke-width="9" stroke-linecap="round" opacity=".18"/>')
        s.append(f'<path d="{d}" fill="none" stroke="{PEN}" stroke-width="2.2" stroke-linecap="round"/>')
    # the falls, where the river drops over the ridge
    s.append(f'<g stroke="{PEN}" stroke-width="1.4" fill="none"><path d="M480 594 v18 M486 592 v20 M492 590 v19"/><path d="M474 616 q12 6 24 0" /></g>')
    # the lake, scribbled in
    s.append(f'<path d="M612 520 C630 470 700 430 760 432 C810 434 820 470 792 500 C760 540 690 560 640 548 C618 542 606 532 612 520Z" fill="#dfe7f0" stroke="{PEN}" stroke-width="2.4"/>')
    lake = ''.join(f'<path d="M{620 + rnd.uniform(-4, 4):.0f} {y} q40 {rnd.uniform(-3, 3):.1f} 80 0 t80 0"/>' for y in range(452, 548, 8))
    s.append(f'<g fill="none" stroke="{PEN}" stroke-width=".8" opacity=".35" clip-path="url(#lakeC)">{lake}</g>')
    s[1:1] = ['<clipPath id="lakeC"><path d="M612 520 C630 470 700 430 760 432 C810 434 820 470 792 500 C760 540 690 560 640 548 C618 542 606 532 612 520Z"/></clipPath>']
    # the mountains, where the river begins
    for x, y, w in [(900, 120, 70), (960, 100, 60), (850, 150, 50), (990, 160, 40), (930, 70, 50)]:
        s.append(f'<path d="M{x - w/2} {y + w*0.6} L{x} {y - w*0.4} L{x + w/2} {y + w*0.6}" fill="#f2ead6" stroke="#3a2a1c" stroke-width="2"/>'
                 f'<path d="M{x - w*0.16} {y - w*0.08} L{x} {y - w*0.4} L{x + w*0.16} {y - w*0.08} l-{w*0.08} {w*0.06} l-{w*0.08} -{w*0.06} l-{w*0.08} {w*0.06}Z" fill="#3a2a1c" opacity=".25"/>')
    # forests, around the upper river and the trail wood
    def tree(x, y, k):
        if k % 3 == 0:
            return f'<g stroke="#2f6b3d" stroke-width="1.6" fill="#cfe0c4"><circle cx="{x}" cy="{y}" r="8"/><circle cx="{x + 7}" cy="{y + 3}" r="6"/></g>'
        return f'<path d="M{x - 7} {y + 9} L{x} {y - 11} L{x + 7} {y + 9}Z M{x} {y + 9} v5" fill="#cfe0c4" stroke="#2f6b3d" stroke-width="1.6"/>'
    k = 0
    for cx, cy, n in [(770, 250, 9), (840, 230, 4), (900, 260, 5), (720, 300, 4), (960, 250, 3)]:
        for _ in range(n):
            s.append(tree(cx + rnd.uniform(-34, 34), cy + rnd.uniform(-22, 22), k))
            k += 1
    # places drawn into the land (their names and creatures come from the content)
    s.append(f'<g stroke="{PEN}" stroke-width="1.8" fill="#dfe7f0"><path d="M340 250 C350 226 392 226 398 248 C404 270 360 284 346 272 C336 264 336 258 340 250Z"/><path d="M404 290 C412 272 444 272 448 292 C452 310 418 318 408 308 C402 302 402 296 404 290Z"/></g>')
    s.append(f'<g stroke="{INK}" stroke-width="2"><path d="M350 716 L376 666 L402 716Z" fill="#f2ead6"/><path d="M376 666 V646 l16 6 l-16 6" fill="#9c3d1a" stroke-width="1.4"/><path d="M370 716 L376 694 L382 716" fill="{INK}" opacity=".5"/></g>')
    s.append(f'<g stroke="{INK}" stroke-width="2"><rect x="600" y="690" width="120" height="30" rx="15" fill="#c9a865" fill-opacity=".45"/><ellipse cx="720" cy="705" rx="10" ry="15" fill="#f2ead6"/><ellipse cx="720" cy="705" rx="4" ry="8" fill="none"/></g>')
    flowers = ''.join(f'<g transform="translate({x} {y})"><path d="M0 0 v18" stroke="{INK}" stroke-width="1.2"/>' + ''.join(f'<ellipse cx="0" cy="-6" rx="4" ry="6" transform="rotate({a})" fill="#e6a94a" fill-opacity=".7" stroke="{INK}" stroke-width="1"/>' for a in (0, 72, 144, 216, 288)) + f'<circle r="3" fill="{INK}"/></g>' for x, y in [(850, 700), (880, 716), (910, 698), (940, 714)])
    s.append(flowers)
    s.append('<circle cx="940" cy="470" r="34" fill="#f8e7a6" opacity=".6"/>')
    s.append(f'<g stroke="{GREEN}" stroke-width="1.8" fill="none"><path d="M120 640 q14 -5 28 0 t28 0 t28 0 M130 652 q14 -5 28 0 t28 0"/></g>')
    # a compass rose, in the sea
    s.append(f'<g transform="translate(80 800) scale(.6)" stroke="{INK}" fill="none"><circle r="40" stroke-width="1.6"/><path d="M0 -56 L8 -8 L0 0 L-8 -8Z" fill="{INK}"/><path d="M0 56 L8 8 L0 0 L-8 8Z" fill="#f2ead6" stroke-width="1.4"/><path d="M-56 0 L-8 -8 L0 0 L-8 8Z" fill="#f2ead6" stroke-width="1.4"/><path d="M56 0 L8 -8 L0 0 L8 8Z" fill="{INK}"/></g>')
    # fog over the far corner, beyond the mountains
    s.append(f'<rect x="700" y="0" width="340" height="320" fill="url(#fogG)"/>')
    return '\n'.join(s)


write('map.svg', the_map(), 1040, 840)
print('art written to', HERE)
