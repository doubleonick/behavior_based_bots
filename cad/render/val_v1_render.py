"""
val_v1_render.py - draw Val (V1 build) as 16-bit-style pixel art from the CAD.

The chassis comes from ../vechicles_v2.scad (exported through val_v1_chassis.scad).
Everything else (Motion base, battery, Atom Matrix and its face, wheel, servo,
counterweight nut, the sensor and its two grey 2x4 L beams) is placed here as simple
shapes, positioned from photos of the built robot. Sizes of those parts are
approximate.

    python cad/render/val_v1_render.py          writes val_v1_<view>.png here

Needs: Python 3 with numpy and matplotlib, and OpenSCAD on the PATH (only to export
the chassis the first time; delete chassis.stl to export it again after a CAD change).
pix.py (beside this file) does the drawing: flat-shaded triangles, three light
levels, dark outlines, then each pixel scaled up 3x.

Coordinates, in mm: x runs front (-) to rear (+), y Val's left (-) to right (+),
z up. The front crossbar is at x = -96, the rear fork ends at x = 56.

For a later build (V2, V3, ...), copy this file, change the parts list, and render
again: the avatar and the build sheets can share one scene.
"""
import os
import re
import subprocess
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from pix import render, colorize, upscale

HERE = os.path.dirname(os.path.abspath(__file__))
CHASSIS_STL = os.path.join(HERE, 'chassis.stl')


def export_chassis():
    if os.path.exists(CHASSIS_STL):
        return
    print('Exporting the chassis from vechicles_v2.scad (takes a minute)...')
    subprocess.run(['openscad', '-o', CHASSIS_STL, os.path.join(HERE, 'val_v1_chassis.scad')],
                   check=True)


def load_ascii_stl(path):
    v = np.array(re.findall(r'vertex\s+(\S+)\s+(\S+)\s+(\S+)', open(path).read()), float)
    return v.reshape(-1, 3, 3)


def box(x0, x1, y0, y1, z0, z1):
    v = np.array([[x0, y0, z0], [x1, y0, z0], [x1, y1, z0], [x0, y1, z0],
                  [x0, y0, z1], [x1, y0, z1], [x1, y1, z1], [x0, y1, z1]], float)
    f = [(0, 2, 1), (0, 3, 2), (4, 5, 6), (4, 6, 7), (0, 1, 5), (0, 5, 4),
         (1, 2, 6), (1, 6, 5), (2, 3, 7), (2, 7, 6), (3, 0, 4), (3, 4, 7)]
    return v[np.array(f)]


def prism(axis, c, r, l0, l1, n):
    """An n-sided prism (a cylinder when n is large) along 'x', 'y' or 'z'.
    c = its centre in the other two coordinates, r = radius, from l0 to l1."""
    a = np.linspace(0, 2 * np.pi, n, endpoint=False) + (np.pi / n if n == 6 else 0)
    ring = np.c_[np.cos(a) * r, np.sin(a) * r]
    tris = []

    def P(l, p):
        u, v = c[0] + p[0], c[1] + p[1]
        return {'x': (l, u, v), 'y': (u, l, v), 'z': (u, v, l)}[axis]

    for i in range(n):
        p, q = ring[i], ring[(i + 1) % n]
        tris += [(P(l0, p), P(l1, p), P(l1, q)), (P(l0, p), P(l1, q), P(l0, q))]
        tris += [(P(l0, (0, 0)), P(l0, q), P(l0, p)), (P(l1, (0, 0)), P(l1, p), P(l1, q))]
    return np.array(tris, float)


# The face on the matrix: row 0 is toward the front. E = eye, M = mouth, . = off
FACE = [".....", ".E.E.", ".....", ".MMM.", "....."]

PALETTE = {
    'outline': '#0b0c10', 'chassis': '#2a2c33', 'motion': '#4a4e58', 'holder': '#15161a',
    'battery': '#3b6fd1', 'matrix': '#c9ccd3', 'panel': '#0d0e12', 'tyre': '#222329',
    'hub': '#8a8f99', 'servo': '#a3a8b0', 'nut': '#b9bec6', 'grey': '#b9bec6',
    'sensor': '#efede6', 'window': '#2b2f3a',
    'led.': '#1b1d24', 'ledE': '#3D7BFF', 'ledM': '#FF6E00',
}

# name: (elevation, azimuth) in degrees. above_rear is "A+", the avatar.
VIEWS = {'top': (90, 180), 'above_rear': (58, 0), 'rear34': (32, 35)}


def build_parts():
    chassis = load_ascii_stl(CHASSIS_STL)
    chassis[:, :, 1] *= -1          # the built module is the mirrored one: servo on Val's left (-y)
    parts = [(chassis, 'chassis')]
    # Atom Motion on the centre plate (top of the plate is z = 12), battery lengthwise at the front
    parts.append((box(-65, 9, -12, 12, 12, 22), 'motion'))
    parts.append((box(-63, -24, -10, 10, 22, 24), 'holder'))
    parts.append((prism('x', (0, 32), 9, -60, -27, 24), 'battery'))
    # Atom Matrix face up at the rear end of the Motion, its face's top toward the front
    parts.append((box(-15, 9, -12, 12, 22, 36), 'matrix'))
    parts.append((box(-14, 8, -11, 11, 36, 36.4), 'panel'))
    pitch = 4.2
    for r in range(5):
        for c in range(5):
            x = -3 + (r - 2) * pitch      # row 0 toward the front (-x)
            y = (c - 2) * pitch           # column 0 toward Val's left (-y)
            parts.append((box(x - 1.2, x + 1.2, y - 1.2, y + 1.2, 36.4, 36.7), 'led' + FACE[r][c]))
    # black wheel centred in the rear fork, grey hub
    parts.append((prism('y', (36, -9), 13, -4, 4, 32), 'tyre'))
    parts.append((prism('y', (36, -9), 5, -4.3, 4.3, 16), 'hub'))
    # servo hanging on Val's left, under the short beam
    parts.append((box(20, 52, -31, -19, -22, -4), 'servo'))
    # the M12 nut inside the counterweight block on the right
    parts.append((prism('y', (32, 1), 10.5, 11, 19, 6), 'nut'))
    # The sensor sits in front, facing forward, framed by two grey 2x4 L beams. Each L
    # stands upright against the front crossbar; its short arm runs inward along the
    # floor and acts as a caster.
    for side in (-1, 1):
        parts.append((box(-104, -96, side * 20 - 3.6, side * 20 + 3.6, -22, 14), 'grey'))   # upright
        y0, y1 = sorted((side * 20, side * 8))
        parts.append((box(-104, -96, y0, y1, -22, -14), 'grey'))                           # foot
    parts.append((box(-108, -98, -12, 12, -12, 12), 'sensor'))
    parts.append((box(-108.4, -108, -5, 5, -5, 5), 'window'))                              # facing forward
    return parts


def main():
    export_chassis()
    parts = build_parts()
    for name, (el, az) in VIEWS.items():
        ids, shade, zbuf, names = render(parts, el, az, 1.6)
        img = colorize(ids, shade, zbuf, names, PALETTE, 1.6)
        out = os.path.join(HERE, f'val_v1_{name}.png')
        plt.imsave(out, upscale(img, 3))
        print('wrote', out)


if __name__ == '__main__':
    main()
