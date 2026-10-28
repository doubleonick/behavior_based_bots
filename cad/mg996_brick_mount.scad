/*
  MG996 brick-compatible servo mount (extrapolated, not M5Stack's own design)
  ===========================================================================

  M5Stack's small servo kits are "LEGO compatible": a clamp covers one broad face of
  the servo and has two Technic pin holes, so the servo's broad side pins flat onto a
  Technic beam (3 holes apart on the micro servo). This file applies the same idea to
  a standard-size servo (MG996 / MG996R): the holes are HOLE_GAP_PEGS apart (6 = 48
  mm, about the span of its ears). It uses the public Technic dimensions below and
  does NOT copy M5Stack's geometry (no drawings were available).

  Printable parts, plus a dummy servo for the preview:
    clamp         a cradle: a plate over one broad face (the face that goes against
                  the beam), a floor under the servo, and a block at each end that
                  the ears screw down onto. Two Technic pin holes go through the
                  end blocks, HOLE_GAP_PEGS apart. Each end has a slot for the cable.
                  The servo drops in from the top.
    horn_adapter  screws onto the servo's round horn; a Technic cross-axle hole in
                  the middle and four pin holes on the 8 mm grid around it.
    frame         an earlier idea, kept: a flat frame the servo drops through, with
                  pin holes through its sides and ends.

  Use:  part = "preview" (clamp, servo, adapter and a beam, assembled), "clamp",
        "horn_adapter", "frame", or "preview_frame".
        Export each printable part on its own (F6, then STL).

  MEASURE YOUR SERVO before printing: MG996 listings disagree by up to ~1 mm, and
  the horn's screw holes vary by maker (HORN_HOLE_R below).
  All sizes in mm.
*/

part = "preview";          // "preview", "clamp", "horn_adapter", "frame", "preview_frame"

// ---------- Technic (brick) standard ----------
PITCH       = 8.0;         // hole to hole
PIN_D       = 4.9;         // pin hole (4.8 nominal + print clearance)
PIN_CB_D    = 6.2;         // the counterbore at each end of a hole
PIN_CB_DEEP = 0.8;
BEAM_H      = 7.8;         // one beam thickness
AXLE_W      = 4.9;         // cross axle: across the arms (4.8 nominal + clearance)
AXLE_ARM    = 1.9;         // cross axle: arm width (1.8 nominal + clearance)

// ---------- MG996 (from listings; measure yours) ----------
SERVO_L      = 40.7;       // body length
SERVO_W      = 19.7;       // body width
SERVO_H      = 37.0;       // bottom to top face of the case
EAR_Z        = 26.5;       // bottom of the case to the underside of the ears
EAR_T        = 2.5;        // ear thickness
EAR_L        = 54.5;       // tip to tip of the ears
EAR_HOLE_X   = 49.5 / 2;   // ear screw holes: half their spacing along the length
EAR_HOLE_Y   = 10.0 / 2;   // ... and across (listings say 9.5 to 10)
SHAFT_X      = SERVO_L / 2 - 10.2;   // output shaft: 10.2 from one end
SHAFT_D      = 5.8;        // 25-tooth spline
HORN_D       = 21.0;       // round horn
HORN_T       = 2.0;
HORN_Z       = SERVO_H + 5.0;        // underside of the horn

// ---------- The clamp ----------
HOLE_GAP_PEGS = 6;         // pin holes this many holes apart (micro servo: 3)
FACE_T       = 3.0;        // the plate over the broad face
FLOOR_T      = 3.0;        // the floor under the servo
PIN_HOLE_Z   = 12.0;       // height of the pin holes above the bottom of the servo
CABLE_W      = 6.0;        // the cable slot in each end block: width ...
CABLE_H      = 8.0;        // ... and height above the bottom of the servo
SCREW_DEEP   = 9.0;        // pilot holes for the ear screws, this deep

// ---------- The frame and the adapter ----------
CLEAR        = 0.4;        // gap around the servo body, each side
FRAME_L      = 7 * PITCH;  // 56: room for the 54.5 ears
FRAME_W      = 5 * PITCH;  // 40: walls wide enough for pin holes
SCREW_D      = 3.0;        // pilot hole for the servo's own screws (M3 self-tapping)
ADAPTER_D    = 28.0;       // wide enough for a solid rim round the 8 mm pin holes
ADAPTER_T    = 6.0;
HUB_D        = 9.0;        // recess underneath for the horn's middle (and its screw)
HUB_DEEP     = 2.5;
HORN_HOLE_R  = 7.0;        // horn screw holes: distance from the middle (MEASURE)
HORN_HOLE_D  = 2.2;        // for M2 screws

$fn = 48;

// ---------------------------------------------------------------------
// A Technic pin hole along z, centred on the origin, `len` long, with counterbores
module pin_hole(len) {
  cylinder(d = PIN_D, h = len + 0.2, center = true);
  for (s = [-1, 1]) {
    translate([0, 0, s * (len / 2 - PIN_CB_DEEP / 2 + 0.05)]) {
      cylinder(d = PIN_CB_D, h = PIN_CB_DEEP + 0.1, center = true);
    }
  }
}

// A Technic cross-axle hole along z
module axle_hole(len) {
  cube([AXLE_W, AXLE_ARM, len + 0.2], center = true);
  cube([AXLE_ARM, AXLE_W, len + 0.2], center = true);
}

// ---------------------------------------------------------------------
// The frame. Origin: the middle of its top face, which the ears sit on.
module frame() {
  open_l = SERVO_L + 2 * CLEAR;
  open_w = SERVO_W + 2 * CLEAR;
  difference() {
    translate([0, 0, -BEAM_H / 2]) {
      cube([FRAME_L, FRAME_W, BEAM_H], center = true);
    }
    // the opening the servo drops through
    cube([open_l, open_w, 3 * BEAM_H], center = true);
    // the servo's ear screws
    for (x = [-EAR_HOLE_X, EAR_HOLE_X], y = [-EAR_HOLE_Y, EAR_HOLE_Y]) {
      translate([x, y, -BEAM_H / 2]) {
        cylinder(d = SCREW_D, h = BEAM_H + 1, center = true);
      }
    }
    // pin holes through the long sides (crosswise), on the 8 mm grid
    for (x = [-2 : 2]) {
      translate([x * PITCH, 0, -BEAM_H / 2]) {
        rotate([90, 0, 0]) {
          pin_hole(FRAME_W);
        }
      }
    }
    // pin holes through the ends (lengthwise): the middle and the outer corners,
    // clear of the ear screws
    for (y = [-2, 0, 2]) {
      translate([0, y * PITCH, -BEAM_H / 2]) {
        rotate([0, 90, 0]) {
          pin_hole(FRAME_L);
        }
      }
    }
  }
}

// ---------------------------------------------------------------------
// The clamp. Origin: the middle of the bottom of the servo, as in servo_dummy().
// x along the servo, y across it (the face against the beam is at -y), z up.
module clamp() {
  in_x = SERVO_L / 2 + CLEAR;          // inside faces of the end blocks
  in_y = SERVO_W / 2 + CLEAR;          // inside face of the plate, and the open side
  hole_x = HOLE_GAP_PEGS * PITCH / 2;
  end_x = max(hole_x + PITCH / 2, EAR_L / 2);   // outer ends: past the holes and the ears
  face_y = -in_y - FACE_T;             // the outside of the plate (against the beam)
  difference() {
    union() {
      // the plate over the broad face, floor to just under the ears
      translate([-end_x, face_y, -FLOOR_T]) {
        cube([2 * end_x, FACE_T, EAR_Z + FLOOR_T]);
      }
      // the floor
      translate([-end_x, face_y, -FLOOR_T]) {
        cube([2 * end_x, in_y - face_y, FLOOR_T]);
      }
      // the end blocks, up to the underside of the ears
      for (s = [-1, 1]) {
        translate([s > 0 ? in_x : -end_x, face_y, -FLOOR_T]) {
          cube([end_x - in_x, in_y - face_y, EAR_Z + FLOOR_T]);
        }
      }
    }
    // the two Technic pin holes, through the end blocks from the beam side
    for (s = [-1, 1]) {
      translate([s * hole_x, (face_y + in_y) / 2, PIN_HOLE_Z]) {
        rotate([90, 0, 0]) {
          pin_hole(in_y - face_y);
        }
      }
    }
    // the ear screws, down into the end blocks
    for (x = [-EAR_HOLE_X, EAR_HOLE_X], y = [-EAR_HOLE_Y, EAR_HOLE_Y]) {
      translate([x, y, EAR_Z - SCREW_DEEP]) {
        cylinder(d = SCREW_D, h = SCREW_DEEP + 0.1);
      }
    }
    // a slot for the cable at each end (the MG996's cable leaves from one end)
    for (s = [-1, 1]) {
      translate([s * (in_x + end_x) / 2, 0, (CABLE_H - FLOOR_T) / 2 - 0.1]) {
        cube([end_x - in_x + 1, CABLE_W, CABLE_H + FLOOR_T + 0.2], center = true);
      }
    }
  }
}

// ---------------------------------------------------------------------
// The horn adapter. Origin: the middle of its underside (on the horn).
module horn_adapter() {
  difference() {
    cylinder(d = ADAPTER_D, h = ADAPTER_T);
    // Technic cross axle in the middle
    translate([0, 0, ADAPTER_T / 2]) {
      axle_hole(ADAPTER_T);
    }
    // the horn's middle (and its screw head) sits in here
    translate([0, 0, -0.1]) {
      cylinder(d = HUB_D, h = HUB_DEEP + 0.1);
    }
    // four Technic pin holes on the 8 mm grid
    for (a = [0, 90, 180, 270]) {
      rotate([0, 0, a]) {
        translate([PITCH, 0, ADAPTER_T / 2]) {
          pin_hole(ADAPTER_T);
        }
      }
    }
    // four screws into the horn, between the pin holes
    for (a = [45, 135, 225, 315]) {
      rotate([0, 0, a]) {
        translate([HORN_HOLE_R, 0, -0.1]) {
          cylinder(d = HORN_HOLE_D, h = ADAPTER_T + 0.2);
        }
      }
    }
  }
}

// ---------------------------------------------------------------------
// A dummy MG996, for the preview only. Origin: the middle of the bottom of the case.
module servo_dummy() {
  color([0.15, 0.15, 0.18]) {
    translate([0, 0, SERVO_H / 2]) {
      cube([SERVO_L, SERVO_W, SERVO_H], center = true);
    }
    // the ears
    difference() {
      translate([0, 0, EAR_Z + EAR_T / 2]) {
        cube([EAR_L, SERVO_W, EAR_T], center = true);
      }
      for (x = [-EAR_HOLE_X, EAR_HOLE_X], y = [-EAR_HOLE_Y, EAR_HOLE_Y]) {
        translate([x, y, EAR_Z]) {
          cylinder(d = 4.2, h = 10, center = true);
        }
      }
    }
    // the boss under the shaft
    translate([SHAFT_X, 0, SERVO_H]) {
      cylinder(d = 13, h = 2);
    }
  }
  color([0.8, 0.8, 0.8]) {
    translate([SHAFT_X, 0, SERVO_H + 2]) {
      cylinder(d = SHAFT_D, h = 3);
    }
    translate([SHAFT_X, 0, HORN_Z]) {
      cylinder(d = HORN_D, h = HORN_T);
    }
  }
}

// A plain Technic beam, for scale in the preview
module beam(holes) {
  color([0.75, 0.1, 0.1]) {
    difference() {
      hull() {
        for (k = [0, holes - 1]) {
          translate([k * PITCH, 0, 0]) {
            cylinder(d = 7.4, h = BEAM_H, center = true);
          }
        }
      }
      for (k = [0 : holes - 1]) {
        translate([k * PITCH, 0, 0]) {
          pin_hole(BEAM_H);
        }
      }
    }
  }
}

// ---------------------------------------------------------------------
if (part == "clamp") {
  // printed lying on the face that goes against the beam
  rotate([-90, 0, 0]) {
    clamp();
  }
} else if (part == "frame") {
  // printed upside down (top face on the bed)
  rotate([180, 0, 0]) {
    frame();
  }
} else if (part == "horn_adapter") {
  horn_adapter();
} else if (part == "preview_frame") {
  servo_dummy();
  translate([0, 0, EAR_Z]) {
    color([0.95, 0.55, 0.1]) {
      frame();
    }
  }
  translate([SHAFT_X, 0, HORN_Z + HORN_T]) {
    color([0.2, 0.5, 0.9]) {
      horn_adapter();
    }
  }
} else {
  servo_dummy();
  color([0.95, 0.55, 0.1]) {
    clamp();
  }
  translate([SHAFT_X, 0, HORN_Z + HORN_T]) {
    color([0.2, 0.5, 0.9]) {
      horn_adapter();
    }
  }
  // a 9-hole beam pinned flat against the clamp's face, for scale
  translate([-4 * PITCH, -(SERVO_W / 2 + CLEAR + FACE_T) - BEAM_H / 2 - 0.2, PIN_HOLE_Z]) {
    rotate([90, 0, 0]) {
      beam(9);
    }
  }
}
