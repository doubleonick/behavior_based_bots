/*
  Servo wheel: wheel, tire and servo horn in one part
  ===================================================

  A copy of the wheel sketched (commented out) at the bottom of vechicles_v2.scad,
  modelled on the wheel that comes with M5Stack's motors, made into a servo horn so
  there is no rubber band to slip off:
    - the same outer shape: a rim (RIM_D) with a rounded tire bulge (TIRE_D),
    - WIDTH is a setting,
    - the four Technic peg holes at +/-8 mm are kept,
    - plus a hub for the servo (HUB, below).

  HUB = "m5"    for M5Stack's LEGO-compatible servo kits: a Technic cross-axle hole
                in the middle. Push a short axle into the servo's output, the wheel
                onto the axle (and/or pin it through the peg holes).
  HUB = "horn"  for a generic micro servo (SG90 / MG90 style): a pocket on the
                servo side that the servo's own plastic horn presses into, and a hole
                through the middle for the horn's screw. The pocket is the horn's
                outline: MEASURE YOUR HORN (HORN_* below). A stock SG90 horn is
                longer than the 25 mm M5-size wheel, so for this hub either make the
                wheel bigger (TIRE_D, RIM_D) or trim the horn's arms (HORN_REACH).
                The pocket is turned 45 degrees so its arms run between the peg holes.

  PRINTING
    part = "whole"  one solid part: print it all in TPU, or in one material.
    part = "core" and part = "tire": the same wheel split in two, for printing TPU
      (tire) together with a rigid material (core): export both, load both into
      the slicer as one object, give each its material. The split is at CORE_D.
    part = "preview": the whole wheel.

  Units: mm. Untested: nothing here has been printed yet.
*/

part = "preview";          // "preview", "whole", "core", "tire"
HUB  = "m5";               // "m5" or "horn"

// ---------- The wheel (defaults: the M5 motor wheel, as in vechicles_v2.scad) ----------
TIRE_D  = 25.16;           // across the tire, at its widest
RIM_D   = 23.0;            // across the rim
WIDTH   = 8.0;             // how wide the wheel is (the original: one peg, 8)
BULGE   = 0.786;           // how much of WIDTH the tire's rounded bulge covers (original: 0.786)
CORE_D  = RIM_D;           // two-material split: the rigid core is this wide

// ---------- Technic (the values used in vechicles_v2.scad) ----------
PEG_PITCH = 8;
PEG_D     = 5;             // peg hole (4.8 nominal, opened up)
CB_D      = 6.4;           // counterbore at each end
CB_DEPTH  = 1;
AXLE_W    = 5.0;           // cross axle hole: across the arms (4.8 nominal, opened up)
AXLE_ARM  = 2.0;           // cross axle hole: arm width (1.8 nominal, opened up)

// ---------- Horn pocket (HUB = "horn"). MEASURE YOUR HORN. ----------
HORN_ARMS   = 2;           // 1 = single arm, 2 = double arm, 4 = cross
HORN_REACH  = 16.0;        // middle to the end of one arm
HORN_HUB_D  = 7.5;         // the round middle of the horn
HORN_TIP_W  = 4.0;         // width at the end of an arm
HORN_ARM_T  = 1.8;         // arm thickness (the pocket's depth for the arms)
HORN_HUB_T  = 4.0;         // the middle's height (the pocket's depth there)
HORN_FIT    = 0.2;         // gap all round, so the horn presses in snugly
HORN_ANGLE  = 45;          // turned so the arms run between the peg holes
SCREW_D     = 2.4;         // the horn's centre screw goes through this
SCREW_HEAD_D = 4.8;        // ... and its head sits in this, from the outside face
SCREW_HEAD_DEEP = 1.5;

$fn = 96;

// ---------------------------------------------------------------------
// The outer shape, centred on the origin, axle along z: the rim plus the tire's bulge
module wheel_shape() {
  cylinder(d = RIM_D, h = WIDTH, center = true);
  intersection() {
    scale([1, 1, (WIDTH * BULGE / 2) / (TIRE_D / 2)]) {
      sphere(d = TIRE_D);
    }
    cylinder(d = TIRE_D + 1, h = WIDTH, center = true);
  }
}

// A Technic peg hole along z, through the wheel, counterbored at both faces
module peg_hole() {
  cylinder(d = PEG_D, h = WIDTH + 1, center = true);
  for (s = [-1, 1]) {
    translate([0, 0, s * (WIDTH / 2 - CB_DEPTH / 2 + 0.05)]) {
      cylinder(d = CB_D, h = CB_DEPTH + 0.1, center = true);
    }
  }
}

// A Technic cross-axle hole along z, through the wheel
module axle_hole() {
  cube([AXLE_W, AXLE_ARM, WIDTH + 1], center = true);
  cube([AXLE_ARM, AXLE_W, WIDTH + 1], center = true);
}

// The horn's outline, flat on z = 0, `deep` tall (arms only, from the middle outward)
module horn_arms(deep) {
  for (k = [0 : HORN_ARMS - 1]) {
    rotate([0, 0, HORN_ANGLE + k * 360 / HORN_ARMS]) {
      hull() {
        cylinder(d = HORN_HUB_D + 2 * HORN_FIT, h = deep);
        translate([HORN_REACH - HORN_TIP_W / 2, 0, 0]) {
          cylinder(d = HORN_TIP_W + 2 * HORN_FIT, h = deep);
        }
      }
    }
  }
}

// Everything cut out of the wheel: peg holes, and the hub
module holes() {
  for (a = [0, 90, 180, 270]) {
    rotate([0, 0, a]) {
      translate([PEG_PITCH, 0, 0]) {
        peg_hole();
      }
    }
  }
  if (HUB == "m5") {
    axle_hole();
  } else {
    // the servo side is -z: the horn presses in from there
    servo_face = -WIDTH / 2 - 0.01;
    translate([0, 0, servo_face]) {
      horn_arms(HORN_ARM_T + 0.01);
      cylinder(d = HORN_HUB_D + 2 * HORN_FIT, h = HORN_HUB_T + 0.01);
    }
    // the centre screw, and its head from the outside face
    cylinder(d = SCREW_D, h = WIDTH + 1, center = true);
    translate([0, 0, WIDTH / 2 - SCREW_HEAD_DEEP]) {
      cylinder(d = SCREW_HEAD_D, h = SCREW_HEAD_DEEP + 0.1);
    }
  }
}

module whole() {
  difference() {
    wheel_shape();
    holes();
  }
}

module core() {
  intersection() {
    whole();
    cylinder(d = CORE_D, h = WIDTH + 1, center = true);
  }
}

module tire() {
  difference() {
    wheel_shape();
    cylinder(d = CORE_D, h = WIDTH + 1, center = true);
  }
}

// Warn when the horn would not fit inside the core
if (HUB == "horn" && HORN_REACH + HORN_FIT > CORE_D / 2 - 0.8) {
  echo(str("WARNING: the horn (reach ", HORN_REACH, ") is too long for this wheel: ",
           "make CORE_D at least ", 2 * (HORN_REACH + HORN_FIT + 0.8),
           " (and RIM_D / TIRE_D with it), or trim the horn and lower HORN_REACH"));
}

// ---------------------------------------------------------------------
if (part == "whole") {
  whole();
} else if (part == "core") {
  core();
} else if (part == "tire") {
  tire();
} else {
  color([0.75, 0.1, 0.1]) {
    whole();
  }
}
