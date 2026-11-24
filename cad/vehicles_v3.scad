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

module val_brooks(is_mirrored)
{
    is_main = true;
    M12_nut_r = 22/2;
    M12_nut_h = 8;
    difference()
    {
        union()
        {
            electronics_bed(is_mirrored, is_main);
            //Counterweight mount//Usually just the outward facing peg walls, and 14 hole beams.
        
            mirror([0, is_mirrored, 0])
            {       
                
                rotate([90, 0, 0])
                //Invert sign of y translation for handedness.
                translate([peg_h * 4.125, peg_h * (1-0.125), (M12_nut_r + peg_pitch + cube_allowance * 2)])
                {
                    cube_rounded(M12_nut_r * 2, M12_nut_r * 2, M12_nut_h + peg_h, peg_h);   
//                    translate([0, 0, peg_h/2]) 
//                    rotate([0, 0, 30])
//                    cylinder_base(M12_nut_r, M12_nut_h + cube_allowance, 6);
                    
                }
            }
        }
        mirror([0, is_mirrored, 0]){
            rotate([90, 0, 0])
            //Invert sign of y translation for handedness.
            translate([peg_h * 4.125, peg_h * (1-0.125), (M12_nut_r + peg_pitch + cube_allowance * 2)])
            {  
                translate([0, 0, peg_h/2]) 
                rotate([0, 0, 30])
                cylinder_base(M12_nut_r, M12_nut_h + cube_allowance, 6);   
            }
        }
    }
    mirror([0, is_mirrored, 0]){
    //Inner sides (servo side)
    rotate([0, 0, 90])
    translate([peg_h * 2, 0, 0])
    {
        technic_beam_odd(9);
        translate([0, 0, peg_h])
        technic_beam_odd(9);
    }
    //Inner sides (M12 side)
    rotate([0, 0, 90])
    translate([-peg_h * 2, peg_h, 0])
    {
        technic_beam_even(7);
        translate([0, 0, peg_h])
        technic_beam_even(7);
    }
    //Outer Sides (M12 side)
    rotate([0, 90, 90])
    translate([0, peg_h, -peg_h * 3])
    {
        technic_beam_odd(7);
        translate([-peg_h, 0, 0])
        technic_beam_odd(7);
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