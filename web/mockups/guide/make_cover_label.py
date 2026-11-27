"""The cover label as a horizontal scroll (owner, 2026-11-27): rolled ends left and
right; "Val's Expedition!" on top; Val's face in the centre; beside each roller an upright
picture that finishes the frame (left: the sun above the moon, a figure-of-eight between
them and a moth where the loops cross; right: a tree, a rock and a log, a slalom path
weaving up between them); along the bottom, the path's creatures, oldest to newest.
Sepia ink only."""
import math, re
INK = '#4e3b26'; PAPER = '#efe6cf'; SHADE = '#e2d6b8'
C = '/home/user/behavior_based_bots/web/guide/art/notebook/creatures/'
W, H = 620, 300

def creature(name, x, y, size, turn=0):
    body = re.search(r'<svg[^>]*>(.*)</svg>', open(C + name + '.svg').read(), re.S).group(1)
    return f'<g transform="translate({x - size/2} {y - size/2}) rotate({turn} {size/2} {size/2}) scale({size/100})">{body}</g>'

o = [f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {W} {H}" width="{W}" height="{H}">']
# ---- the scroll: the sheet, then a rolled end at each side ----
L, R, T, B = 38, W - 38, 22, H - 22
o.append(f'<path d="M{L} {T} C{W*0.3} {T-6} {W*0.7} {T+6} {R} {T} V{B} C{W*0.7} {B+6} {W*0.3} {B-6} {L} {B}Z" fill="{PAPER}" stroke="{INK}" stroke-width="2"/>')
o.append(f'<path d="M{L+12} {T+12} C{W*0.3} {T+7} {W*0.7} {T+17} {R-12} {T+12} V{B-12} C{W*0.7} {B-7} {W*0.3} {B-17} {L+12} {B-12}Z" fill="none" stroke="{INK}" stroke-width="0.9"/>')
for side in (-1, 1):
    x = L if side < 0 else R                       # where the sheet meets the roll
    cx = x + side * 4
    rw = 22
    rx0 = cx - rw / 2
    o.append(f'<rect x="{rx0}" y="{T-12}" width="{rw}" height="{B-T+24}" rx="5" fill="{SHADE}" stroke="{INK}" stroke-width="2"/>')
    # the rolled paper: a few curling lines along the roll, and the spiral at each end
    for k in (-6, 0, 6):
        o.append(f'<path d="M{cx+k} {T-4} V{B+4}" stroke="{INK}" stroke-width="0.6" opacity="0.55"/>')
    for y, d in ((T - 12, 1), (B + 12, -1)):
        o.append(f'<ellipse cx="{cx}" cy="{y}" rx="{rw/2}" ry="5" fill="{PAPER}" stroke="{INK}" stroke-width="1.6"/>')
        o.append(f'<path d="M{cx-1} {y} a2 1.4 0 1 1 3 0 a4.5 2.6 0 1 1 -8 0 a7 3.6 0 1 1 13 0" fill="none" stroke="{INK}" stroke-width="0.9"/>')
# ---- the title ----
o.append(f'<text x="{W/2}" y="68" text-anchor="middle" font-family="IM Fell English, Georgia, serif" font-size="36" fill="{INK}">Val’s Expedition!</text>')
o.append(f'<path d="M222 80 H296 M324 80 H398" stroke="{INK}" stroke-width="1"/><path d="M310 75 l5 5 l-5 5 l-5 -5Z" fill="{INK}"/>')
# ---- Val's face ----
fx, fy, fs = W/2, 146, 84
o.append(f'<rect x="{fx-fs/2}" y="{fy-fs/2}" width="{fs}" height="{fs}" rx="15" fill="{SHADE}" stroke="{INK}" stroke-width="2"/>')
o.append(f'<rect x="{fx-fs/2+9}" y="{fy-fs/2+9}" width="{fs-18}" height="{fs-18}" rx="6" fill="#f6efdc" stroke="{INK}" stroke-width="1.3"/>')
lit = {(1, 1), (3, 1), (0, 3), (1, 4), (2, 4), (3, 4), (4, 3)}
cell, gap = 9, 3
g0x, g0y = fx - (5*cell + 4*gap)/2, fy - (5*cell + 4*gap)/2
for r in range(5):
    for c in range(5):
        o.append(f'<rect x="{g0x + c*(cell+gap):.1f}" y="{g0y + r*(cell+gap):.1f}" width="{cell}" height="{cell}" rx="1.4" '
                 f'fill="{INK if (c, r) in lit else "none"}" stroke="{INK}" stroke-width="0.85"/>')
# ---- left, upright beside the roller: the sun above, the moon below, a figure-of-eight ----
lx, sy, my = 92, 88, 208
o.append(f'<path d="M{lx-30} {sy} C{lx-30} {sy+40} {lx+30} {my-40} {lx+30} {my} C{lx+30} {my+36} {lx-30} {my+36} {lx-30} {my} '
         f'C{lx-30} {my-40} {lx+30} {sy+40} {lx+30} {sy} C{lx+30} {sy-36} {lx-30} {sy-36} {lx-30} {sy}Z" '
         f'fill="none" stroke="{INK}" stroke-width="1.3" stroke-dasharray="2 5" stroke-linecap="round"/>')
o.append(f'<circle cx="{lx}" cy="{sy}" r="11" fill="{SHADE}" stroke="{INK}" stroke-width="1.6"/>')
for k in range(10):
    a = 2 * math.pi * k / 10
    o.append(f'<path d="M{lx + 15*math.cos(a):.1f} {sy + 15*math.sin(a):.1f} L{lx + 20*math.cos(a):.1f} {sy + 20*math.sin(a):.1f}" stroke="{INK}" stroke-width="1.3" stroke-linecap="round"/>')
# the moon: a full disc with a smaller disc taken out of its right side
o.append(f'<mask id="moonbite"><rect x="0" y="0" width="{W}" height="{H}" fill="#fff"/><circle cx="{lx+9}" cy="{my-4}" r="13" fill="#000"/></mask>'
         f'<circle cx="{lx}" cy="{my}" r="15" fill="{SHADE}" stroke="{INK}" stroke-width="1.6" mask="url(#moonbite)"/>'
         f'<path d="M{lx+2.3} {my-14.8} A13 13 0 0 0 {lx+11.6} {my+8.4}" fill="none" stroke="{INK}" stroke-width="1.6"/>')
o.append(creature('moth', lx, (sy + my) / 2, 22, 10))
# ---- right, upright beside the roller: a tree, a rock and a log; the path weaves up ----
rx = W - 92
o.append(f'<g stroke="{INK}" stroke-width="1.6" fill="{SHADE}">'
         f'<path d="M{rx} 108 V122" fill="none"/><path d="M{rx-13} 110 L{rx} 76 L{rx+13} 110Z"/>'
         f'<path d="M{rx-19} 168 C{rx-21} 152 {rx-5} 144 {rx+5} 147 C{rx+16} 149 {rx+22} 160 {rx+19} 168Z"/>'
         f'<rect x="{rx-21}" y="204" width="38" height="15" rx="7.5"/><ellipse cx="{rx+14}" cy="211.5" rx="3.5" ry="7.5" fill="{PAPER}"/></g>')
o.append(f'<path d="M{rx+2} 252 C{rx-36} 246 {rx-38} 226 {rx-30} 212 C{rx-20} 194 {rx+34} 196 {rx+32} 172 '
         f'C{rx+30} 150 {rx-34} 152 {rx-32} 128 C{rx-30} 102 {rx+26} 108 {rx+22} 84 C{rx+20} 70 {rx+6} 66 {rx} 64" '
         f'fill="none" stroke="{INK}" stroke-width="1.3" stroke-dasharray="2 5" stroke-linecap="round"/>')
o.append(f'<path d="M{rx+8} 254 m-5 0 a5 5 0 1 0 10 0 a3.5 3.5 0 1 0 -7 0 a2 2 0 1 0 4 0" fill="none" stroke="{INK}" stroke-width="1.3"/>')
# ---- bottom: the path's creatures, oldest to newest ----
o.append(f'<path d="M134 246 C180 234 210 258 250 246 S320 234 360 248 S420 258 444 246" fill="none" stroke="{INK}" stroke-width="1.4"/>')
o.append(f'<path d="M444 246 C452 242 458 244 462 247" fill="none" stroke="{INK}" stroke-width="1.4" stroke-dasharray="2 5" stroke-linecap="round"/>')
bx, by = 148, 244                                   # a single cell, with its tail
o.append(f'<g stroke="{INK}" stroke-width="1.3" fill="{SHADE}"><path d="M{bx-11} {by} C{bx-11} {by-10} {bx+11} {by-10} {bx+11} {by} C{bx+11} {by+9} {bx-11} {by+9} {bx-11} {by}Z"/>'
         f'<path d="M{bx-11} {by} c-7 -5 -9 5 -16 0" fill="none"/>'
         + ''.join(f'<path d="M{bx + 9*math.cos(a):.1f} {by + 7*math.sin(a):.1f} l{3.5*math.cos(a):.1f} {3.5*math.sin(a):.1f}" fill="none" stroke-width="0.8"/>' for a in [i*0.8 - 1.3 for i in range(6)])
         + f'<circle cx="{bx+2}" cy="{by}" r="2.3" fill="{INK}" stroke="none"/></g>')
o.append(creature('jelly', 206, 236, 34))                 # stop 1: the jellies
o.append(creature('flatworm', 266, 242, 34, 90))          # stop 2: a flatworm
o.append(creature('ant', 326, 244, 28, 90))               # trails and fields: an ant
fx2, fy2 = 396, 246                                       # subsumption: an early jawless fish
o.append(f'<g stroke="{INK}" stroke-width="1.3" fill="{SHADE}">'
         f'<path d="M{fx2-26} {fy2} C{fx2-22} {fy2-8} {fx2+10} {fy2-8} {fx2+24} {fy2-2} L{fx2+34} {fy2-8} L{fx2+32} {fy2} L{fx2+34} {fy2+8} L{fx2+24} {fy2+2} C{fx2+10} {fy2+8} {fx2-22} {fy2+8} {fx2-26} {fy2}Z"/>'
         f'<path d="M{fx2-2} {fy2-6} C{fx2+4} {fy2-11} {fx2+14} {fy2-10} {fx2+18} {fy2-4}" fill="none"/>'
         f'<circle cx="{fx2-21} " cy="{fy2}" r="2.6" fill="{PAPER}"/><circle cx="{fx2-15}" cy="{fy2-2}" r="1.2" fill="{INK}" stroke="none"/>'
         + ''.join(f'<circle cx="{fx2-10 + i*3.2}" cy="{fy2+1.5}" r="0.9" fill="{INK}" stroke="none"/>' for i in range(7)) + '</g>')
o.append(f'<text x="474" y="256" text-anchor="middle" font-family="IM Fell English, Georgia, serif" font-size="28" fill="{INK}">?</text>')
o.append('</svg>')
open('scroll.svg', 'w').write('\n'.join(o))
print('ok')
