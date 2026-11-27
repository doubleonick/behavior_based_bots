"""The notebook's cover label, after the owner's concept sketch (2026-11-27): an old-map
cartouche with "Val's Expedition!" along the top, Val's face in the centre, a sun and
moon with a figure-of-eight path on the left, a log, rock and tree with a slalom path on
the right, and the evolutionary path along the bottom. Sepia ink only."""
import re
INK = '#4e3b26'
C = '/home/user/behavior_based_bots/web/guide/art/notebook/creatures/'

def creature(name, x, y, size, turn=0):
    inner = open(C + name + '.svg').read()
    body = re.search(r'<svg[^>]*>(.*)</svg>', inner, re.S).group(1)
    return (f'<g transform="translate({x - size/2} {y - size/2}) rotate({turn} {size/2} {size/2}) scale({size/100})">'
            f'{body}</g>')

W, H = 600, 300
o = [f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {W} {H}" width="{W}" height="{H}">']
# ---- the cartouche: ends curving in, long sides a little; a double rule ----
def cart(i):
    x0, y0, x1, y1 = 14 + i, 14 + i, W - 14 - i, H - 14 - i
    b = 26 - i * 0.4          # how far the ends curve in
    s = 7 - i * 0.15          # how far the long sides dip
    return (f'M{x0} {y0} Q{W/2} {y0 + s} {x1} {y0} Q{x1 - b} {H/2} {x1} {y1} '
            f'Q{W/2} {y1 - s} {x0} {y1} Q{x0 + b} {H/2} {x0} {y0}Z')
o.append(f'<path d="{cart(0)}" fill="#efe6cf" stroke="{INK}" stroke-width="2.2"/>')
o.append(f'<path d="{cart(7)}" fill="none" stroke="{INK}" stroke-width="0.9"/>')
for (x, y, sx, sy) in [(14, 14, 1, 1), (W - 14, 14, -1, 1), (14, H - 14, 1, -1), (W - 14, H - 14, -1, -1)]:
    o.append(f'<path d="M{x} {y + 18*sy} l{6*sx} {-6*sy} M{x + 18*sx} {y} l{-6*sx} {6*sy}" stroke="{INK}" stroke-width="1.2"/>'
             f'<circle cx="{x + 5*sx}" cy="{y + 5*sy}" r="2.2" fill="{INK}"/>')
# ---- the title ----
o.append(f'<text x="{W/2}" y="66" text-anchor="middle" font-family="IM Fell English, Georgia, serif" font-size="40" fill="{INK}">Val’s Expedition!</text>')
o.append(f'<path d="M190 78 H272 M328 78 H410" stroke="{INK}" stroke-width="1"/><path d="M300 73 l5 5 l-5 5 l-5 -5Z" fill="{INK}"/>')
# ---- Val's face, in the centre ----
fx, fy, fs = W/2, 150, 92
o.append(f'<rect x="{fx-fs/2}" y="{fy-fs/2}" width="{fs}" height="{fs}" rx="16" fill="#e4dac0" stroke="{INK}" stroke-width="2"/>')
o.append(f'<rect x="{fx-fs/2+10}" y="{fy-fs/2+10}" width="{fs-20}" height="{fs-20}" rx="7" fill="#f6efdc" stroke="{INK}" stroke-width="1.4"/>')
lit = {(1, 1), (3, 1), (0, 3), (1, 4), (2, 4), (3, 4), (4, 3)}
cell, gap = 10, 3.2
g0 = fx - (5*cell + 4*gap)/2
for r in range(5):
    for c in range(5):
        on = (c, r) in lit
        o.append(f'<rect x="{g0 + c*(cell+gap):.1f}" y="{fy - (5*cell+4*gap)/2 + r*(cell+gap):.1f}" width="{cell}" height="{cell}" rx="1.5" '
                 f'fill="{INK if on else "none"}" stroke="{INK}" stroke-width="0.9"/>')
# ---- left: the sun and the moon, a figure-of-eight between them, a moth on it ----
sx, sy, mx, my = 82, 150, 168, 150
o.append(f'<path d="M{sx} {sy-30} C{sx+40} {sy-30} {mx-40} {my+30} {mx} {my+30} C{mx+38} {my+30} {mx+38} {my-30} {mx} {my-30} '
         f'C{mx-40} {my-30} {sx+40} {sy+30} {sx} {sy+30} C{sx-38} {sy+30} {sx-38} {sy-30} {sx} {sy-30}Z" '
         f'fill="none" stroke="{INK}" stroke-width="1.3" stroke-dasharray="2 5" stroke-linecap="round"/>')
o.append(f'<circle cx="{sx}" cy="{sy}" r="12" fill="none" stroke="{INK}" stroke-width="1.6"/>')
for k in range(10):
    import math
    a = 2 * math.pi * k / 10
    o.append(f'<path d="M{sx + 16*math.cos(a):.1f} {sy + 16*math.sin(a):.1f} L{sx + 22*math.cos(a):.1f} {sy + 22*math.sin(a):.1f}" stroke="{INK}" stroke-width="1.4" stroke-linecap="round"/>')
o.append(f'<path d="M{mx+4} {my-16} A16 16 0 1 0 {mx+4} {my+16} A11 16 0 0 1 {mx+4} {my-16}Z" fill="#e4dac0" stroke="{INK}" stroke-width="1.6"/>')
o.append(creature('moth', (sx + mx) / 2, sy, 22, 20))
# ---- right: a log, a rock and a tree in a row; the path goes over the log, under the
# rock and round the tree; a small creature sets off from the left ----
o.append(f'<g stroke="{INK}" stroke-width="1.6" fill="#e4dac0">'
         f'<rect x="418" y="144" width="42" height="17" rx="8.5"/><ellipse cx="456" cy="152.5" rx="4" ry="8.5" fill="#efe6cf"/>'
         f'<path d="M476 172 C474 156 490 148 500 151 C512 153 518 164 515 172Z"/>'
         f'<path d="M540 150 V166" fill="none"/><path d="M526 152 L540 114 L554 152Z"/></g>')
o.append(f'<path d="M404 178 C404 150 418 132 440 132 C462 132 468 184 495 186 C522 188 568 176 566 140 C564 112 548 98 532 104" '
         f'fill="none" stroke="{INK}" stroke-width="1.3" stroke-dasharray="2 5" stroke-linecap="round"/>')
o.append(f'<path d="M404 186 m-5 0 a5 5 0 1 0 10 0 a3.5 3.5 0 1 0 -7 0 a2 2 0 1 0 4 0" fill="none" stroke="{INK}" stroke-width="1.3"/>')
# ---- bottom: the evolutionary path, oldest to newest ----
o.append(f'<path d="M70 250 C130 236 170 262 230 248 S340 236 400 250 S470 262 505 248" fill="none" stroke="{INK}" stroke-width="1.4"/>')
o.append(f'<path d="M505 248 C520 242 528 244 540 248" fill="none" stroke="{INK}" stroke-width="1.4" stroke-dasharray="2 5" stroke-linecap="round"/>')
# a single cell: a blob with a tail (flagellum) and fine hairs
bx, by = 96, 246
o.append(f'<g stroke="{INK}" stroke-width="1.4" fill="#e4dac0"><path d="M{bx-12} {by} C{bx-12} {by-11} {bx+12} {by-11} {bx+12} {by} C{bx+12} {by+10} {bx-12} {by+10} {bx-12} {by}Z"/>'
         f'<path d="M{bx+12} {by} c8 -6 10 6 18 0" fill="none"/>'
         + ''.join(f'<path d="M{bx + 9*math.cos(a):.1f} {by + 7*math.sin(a):.1f} l{4*math.cos(a):.1f} {4*math.sin(a):.1f}" fill="none" stroke-width="0.9"/>' for a in [i*0.7 + 1.4 for i in range(7)])
         + f'<circle cx="{bx-2}" cy="{by}" r="2.5" fill="{INK}" stroke="none"/></g>')
o.append(creature('jelly', 196, 238, 38))
o.append(creature('woodlouse', 296, 242, 34, 90))
o.append(creature('ant', 396, 244, 32, 90))
o.append(f'<text x="552" y="258" text-anchor="middle" font-family="IM Fell English, Georgia, serif" font-size="30" fill="{INK}">?</text>')
o.append('</svg>')
open('label.svg', 'w').write('\n'.join(o))
print('ok')
