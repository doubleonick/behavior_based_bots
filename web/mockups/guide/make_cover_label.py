"""The cover label as a ribbon scroll (owner, 2026-11-27): a banner whose ends fold back
into notched tails; "Val's Expedition!" on top; Val's face in the centre; beside each roller an upright
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
# ---- the ribbon scroll: a banner that waves (its top and bottom edges ripple), each
# end curling under into a rolled fold, then a long swallowtail tail flowing out behind ----
L, R, T, B = 60, W - 60, 28, H - 28
BACK = '#d8caa6'
def edge(y, amp, flip):
    # a gentle wave along the banner's top or bottom
    return (f'C{L+90} {y - flip*amp} {L+170} {y + flip*amp} {W/2} {y} '
            f'C{R-170} {y - flip*amp} {R-90} {y + flip*amp} {R} {y}')
for side in (-1, 1):
    x = L if side < 0 else R
    e = x + side * 50                         # the tail's far end
    # the tail: comes out from behind the fold, sweeps out with a curve, notched end
    o.append(f'<path d="M{x - side*4} {T+44} C{x + side*16} {T+40} {e - side*8} {T+30} {e} {T+36} '
             f'C{e - side*10} {(T+B)/2 + 6} {e - side*10} {(T+B)/2 + 6} {e - side*22} {(T+B)/2 + 18} '
             f'C{e - side*10} {(T+B)/2 + 34} {e - side*4} {B+4} {e + side*6} {B+18} '
             f'C{e - side*16} {B+24} {x + side*14} {B+20} {x - side*4} {B+14}Z" '
             f'fill="{SHADE}" stroke="{INK}" stroke-width="2" stroke-linejoin="round"/>')
    # fine lines along the tail, as on an engraved ribbon
    for k in (0.35, 0.65):
        y0 = T + 44 + k * (B - T - 30)
        o.append(f'<path d="M{x + side*6} {y0:.0f} C{x + side*22} {y0 - 6:.0f} {e - side*20} {y0 - 4:.0f} {e - side*28} {y0 + 2:.0f}" '
                 f'fill="none" stroke="{INK}" stroke-width="0.7" opacity="0.6"/>')
    # the fold where the banner turns under: a curl at top and bottom
    for y, d in ((T, 1), (B, -1)):
        o.append(f'<path d="M{x} {y} C{x + side*14} {y + d*2} {x + side*16} {y + d*16} {x + side*4} {y + d*18} '
                 f'C{x - side*4} {y + d*19} {x - side*6} {y + d*10} {x} {y + d*8}Z" fill="{BACK}" stroke="{INK}" stroke-width="1.6" stroke-linejoin="round"/>')
o.append(f'<path d="M{L} {T} {edge(T, 10, 1)} C{R + 10} {(T+B)/2 - 40} {R + 10} {(T+B)/2 + 40} {R} {B} '
         f'{edge(B, 10, -1).replace(str(W/2), str(W/2))} C{L - 10} {(T+B)/2 + 40} {L - 10} {(T+B)/2 - 40} {L} {T}Z" fill="{PAPER}" stroke="{INK}" stroke-width="2"/>'.replace(
         f'{edge(B, 10, -1)}', f'C{R-90} {B+10} {R-170} {B-10} {W/2} {B} C{L+170} {B+10} {L+90} {B-10} {L} {B}'))
o.append(f'<path d="M{L+12} {T+12} C{L+90} {T+2} {L+170} {T+22} {W/2} {T+12} C{R-170} {T+2} {R-90} {T+22} {R-12} {T+12} '
         f'L{R-12} {B-12} C{R-90} {B-2} {R-170} {B-22} {W/2} {B-12} C{L+170} {B-2} {L+90} {B-22} {L+12} {B-12}Z" fill="none" stroke="{INK}" stroke-width="0.9"/>')
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
