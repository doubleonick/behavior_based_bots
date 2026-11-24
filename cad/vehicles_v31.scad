include<foundational_modules.scad>

//rad_allowance = 0.25;
//cube_allowance = 0.5;

// ---------------- Technic holes ----------------
peg_d     = 5;//4.8;
peg_pitch = 8;
cb_d      = 6.4;//6.2;
cb_depth  = 1;//0.8;
use_cb    = true;

key_t      = 8;
sock_cheek = 0;//4;
sock_w   = key_t + rad_allowance;//2*sock_cheek;

body_x = peg_pitch * 9;
body_y = peg_pitch * 3;
body_z = peg_pitch;
mount_offset_x = peg_pitch * 2;

roundness      = 48;
//cube_allowance = 0.5;
//rad_allowance  = 0.25;
in_to_mm       = 25.4;

peg_r          = 2.5;// + rad_allowance;
peg_h          = 8;// + cube_allowance;

module peg_hole_z(w, cb = use_cb)
{
    cylinder_base(peg_d/2, peg_pitch, 100);//(d = peg_d, h = w + 2, center = true, $fn = 48);
    if (cb) for (s = [-1, 1]) translate([0, 0, s*(w/2 - cb_depth/2 + 0.5)])
        cylinder_base(cb_d/2, cb_depth + 1, 100);
}

module peg_hole_y(w, cb = use_cb) {
    rotate([90, 0, 0]) {
        cylinder(d = peg_d, h = w + 2, center = true, $fn = 48);
        if (cb) for (s = [-1, 1]) translate([0, 0, s*(w/2 - cb_depth/2 + 0.5)])
            cylinder(d = cb_d, h = cb_depth + 1, center = true, $fn = 48);
    }
}
/************************************************************/
module peg()
{
    cylinder_base(peg_r, peg_h, roundness);
    for (s = [-1, 1]) translate([0, 0, s*(peg_h/2 - cb_depth/2)])
        cylinder_base(cb_d/2, cb_depth + 1, 100);
}
/************************************************************/
module technic_beam_even(peg_count)//PROBABLY CAN PARAMETERIZE THIS
{
    difference()
    {
        cube_rounded(peg_h, peg_h * peg_count, peg_h, peg_h);
        
        for(y = [-peg_count/2:peg_count/2])
        {
            translate([0, peg_h * 0.5 + peg_h * y, 0])
            scale([1, 1, 1.1])
            peg();
        }
    }
}
/************************************************************/
module technic_beam_odd(peg_count)//PROBABLY CAN PARAMETERIZE THIS
{
    difference()
    {
        cube_rounded(peg_h, peg_h * peg_count, peg_h, peg_h);
        
        for(y = [-round(peg_count/2):round(peg_count/2)])
        {
            translate([0, peg_h * y, 0])
            scale([1, 1, 1.1])
            peg();
        }
    }
}

module vehicle_bridge()
{
//    translate([0, peg_h * 2 - cube_allowance, 0])
    for(y = [-1:1])
    {
        translate([0, peg_h * y, 0])
        {
            rotate([0, 90, 90])
            technic_beam_odd(7);

            translate([0, 0, peg_h])
            rotate([0, 0, 90])
            technic_beam_odd(7);
        }
    }
    
//    translate([0, - peg_h, 0])
//    {
//        translate([0, peg_h * 2 - cube_allowance, 0])
//        rotate([0, 90, 90])
//        technic_beam_odd(7);
//
//        translate([0, peg_h * 2 - cube_allowance, peg_h])
//        rotate([0, 0, 90])
//        technic_beam_odd(7);
//    }
}

module electronics_bed(is_mirrored, is_main)
{
    difference()
    {
//            translate([0, 0, peg_h])
        cube_rounded(body_x, body_y, peg_h, key_t);

        cube_rounded(peg_h * 2.75, peg_h * 2.75, peg_h + cube_allowance, key_t);    
        //Motion module peg holes
        for(x = [-1, 1])
        {    
            translate([peg_h * 3.5 * x, 0, 0])
            cube_rounded(peg_h * 1.5, peg_h * 2.75, peg_h + cube_allowance, key_t);
            for(y = [-1, 1])
            {
                translate([mount_offset_x * x, key_t * y, peg_h * 0.5])
                scale([1, 1, 2])
                peg();//_hole_z(sock_w, use_cb);
            }
        }
//        //Side peg holes
//        for(s = [-3, -1, 1, 3])
//            for (y = [-1, 1])
//                translate([s * key_t, key_t * y, peg_h])
//                rotate([90, 0, 0])
//                peg();//_hole_y(sock_w);
        
    }
    front_peg_holes = is_main == true ? 7 : 3;
    //Front and Back peg holes
    for(x = [-1, 1])
    {
//        back_off = is_mirrored==1 ? -1 : 1;
//        
//        beam_off = x==1 ? 0 : 1 * back_off;
        
        translate([- peg_h * 5 * x, 0, 0])
        rotate([0, 90, 0])
        technic_beam_odd(front_peg_holes);
        translate([-peg_h * 5 * x, 0, peg_h])
        rotate([0, 90, 0])
        technic_beam_odd(front_peg_holes);
    }    
//    //Side beams
//    translate([-peg_h * 2.5, peg_h * 2 - cube_allowance, 0])
//    rotate([0, 90, 90])
//    technic_beam_even(4);
//
//    translate([peg_h * 2, peg_h * 2 - cube_allowance, 0])
//    rotate([0, 0, 90])
//    technic_beam_odd(5);

//    translate([0, peg_h * 2 - cube_allowance, peg_h])
//    rotate([0, 0, 90])
//    technic_beam_odd(9);
//    mirror([0, 1, 0]){
//        translate([-peg_h * 1.5, peg_h * 2 - cube_allowance, 0])
//        rotate([0, 90, 90])
//        technic_beam_even(6);
//    
//        translate([peg_h * 3, peg_h * 2 - cube_allowance, 0])
//        rotate([0, 0, 90])
//        technic_beam_odd(3);
//    
//        translate([0, peg_h * 2 - cube_allowance, peg_h])
//        rotate([0, 0, 90])
//        technic_beam_odd(9);
//    }
}

//vehicles_v31 (2026-11-24), from vehicles_v3:
// - The M12 nut housing is its own part (m12_mount), printed separately and pegged
//   onto the outward-facing 1x9 row on the side opposite the servo, by a vertical 1x3
//   tab either side. It can be moved hole by hole, to trim Val's balance.
// - That side has ONE row (a stacked pair) of 1x9 beams facing out, against the
//   electronics bed, between the two 1x7 end caps (v3 had 1x7s facing up AND 1x7s
//   facing out, in front of the M12 housing).
// - The M12 no longer sits in the far rear corner, so its nut hollow no longer cuts
//   the rear end cap.
// Everything else is as in v3.
//
//Which part to show or export (each prints separately):
//  "assembly", "brooks_red", "brooks_gold", "bed", "m12"
part = "assembly";

//M12 counterweight mount: its own part.
M12_nut_r = 22/2;
M12_nut_h = 8;
m12_hole  = 2;      //which 1x9 hole the mount centres on, counted from the middle:
                    //-2 to 2 (+ = toward the rear, where the servo and M12 sit); its
                    //tabs land 2 holes either side. 2 is the rearmost spot where both
                    //tabs peg in. Not further: the far rear corner (as in v3) is what
                    //unbalanced Val (owner, 2026-11-24).

//Where the mount sits on a val_brooks (before the mirror for the other hand).
//Its tabs' back faces touch the 1x9 row's outer face.
module m12_place()
{
    translate([peg_h * m12_hole, -peg_h * 3, peg_h])
    children();
}

//The hollow for the nut.
module m12_nut_hollow()
{
    translate([0, -peg_h/2, 0])
    rotate([90, 0, 0])
    translate([0, 0, peg_h/2])
    rotate([0, 0, 30])
    cylinder_base(M12_nut_r, M12_nut_h + cube_allowance, 6);
}

//The mount: the outer shell, with a vertical 1x3 technic tab either side whose holes
//face out (along y), lined up with the 1x9's holes. +y faces the robot.
module m12_mount()
{
    difference()
    {
        union()
        {
            //the shell: as wide as the gap between the tabs plus 0.5 mm into each,
            //so shell and tabs print as one piece (the tabs sit 2 holes apart).
            //(As in v3, the nut's points touch the shell's outside: slicers may call
            //that non-manifold and repair it.)
            translate([0, -peg_h/2, 0])
            rotate([90, 0, 0])
            cube_rounded(peg_h * 3 + 1, M12_nut_r * 2, M12_nut_h + peg_h, peg_h);
            for (s = [-1, 1])
                translate([s * peg_h * 2, 0, 0])
                rotate([90, 0, 0])
                technic_beam_odd(3);
        }
        m12_nut_hollow();
    }
}

module val_brooks(is_mirrored)
{
    is_main = true;
    electronics_bed(is_mirrored, is_main);
    mirror([0, is_mirrored, 0]){
    //Inner sides (servo side)
    rotate([0, 0, 90])
    translate([peg_h * 2, 0, 0])
    {
        technic_beam_odd(9);
        translate([0, 0, peg_h])
        technic_beam_odd(9);
    }
    //M12 side: one row of 1x9, facing out, against the electronics bed,
    //between the two 1x7 end caps (replaces v3's 1x7s facing up and facing out)
    rotate([0, 90, 90])
    translate([0, 0, -peg_h * 2])
    {
        technic_beam_odd(9);
        translate([-peg_h, 0, 0])
        technic_beam_odd(9);
    }
    //Outer Sides (servo side)
    rotate([0, 90, 90])
    translate([0, 0, peg_h * 3])
    {
//        translate([0, peg_h * 2.5, 0])
        technic_beam_odd(9);
        translate([-peg_h, peg_h * 2.5, 0])
        technic_beam_even(4);
    }
    
    //Servo
    rotate([0, 0, 90])
    translate([peg_h * 3, 0, 0])
    {
        translate([0, -peg_h * 2, peg_h])
        technic_beam_odd(5);
    }
    }    
}

if (part == "assembly")
{
    color("red")
    translate([0, peg_h * 6.5, 0])
    val_brooks(0);

    //translate([0, - peg_h * 1.5, 0])
    color("silver")
    electronics_bed(0, false);
    //vehicle_bridge();
    color("gold")
    translate([0, peg_h * -6.5, 0])
    val_brooks(1);

    //The M12 mounts, shown pegged on
    color("orange")
    translate([0, peg_h * 6.5, 0])
    m12_place() m12_mount();
    color("orange")
    translate([0, peg_h * -6.5, 0])
    mirror([0, 1, 0])
    m12_place() m12_mount();
}
if (part == "brooks_red")  { val_brooks(0); }
if (part == "brooks_gold") { val_brooks(1); }
if (part == "bed")         { electronics_bed(0, false); }
if (part == "m12")         { m12_mount(); }   //print two: it is symmetric, so it serves both hands

//electronics_bed();
//tire_r = 25.16/2;
//wheel_r = 23/2;
//wheel_z = peg_h;
//
//difference()
//{
//    union()
//    {
//        cylinder_base(wheel_r, wheel_z, roundness);
//        scale([1, 1, 0.25])
//        sphere_base(tire_r, roundness);
//    }
//    translate([peg_h, 0, 0])
//    peg();
//    translate([-peg_h, 0, 0])
//    peg();
//    rotate([0, 0, 90])
//    {
//        translate([peg_h, 0, 0])
//        peg();
//        translate([-peg_h, 0, 0])
//        peg();
//    }
//}

////Connective Tissue
//rotate([0, 90, 90])
//translate([0, peg_h * 3, -peg_h * 3.5])
//scale([1, 1, 2])
//technic_beam_even(8);