include<foundational_modules.scad>

rad_allowance = 0.25;
cube_allowance = 0.5;

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
cube_allowance = 0.5;
rad_allowance  = 0.25;
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

module val_brooks(is_mirrored)
{
    translate([-peg_h * 3.5, 0, 0])
    {
        difference()
        {
            translate([0, 0, peg_h * 0.5])
            cube_rounded(body_x, body_y, peg_h * 2, key_t);

            //Motion module peg holes
            for(x = [-1, 1])
                for(y = [-1, 1])
                {
                    translate([mount_offset_x * x, key_t * y, peg_h * 0.5])
                    scale([1, 1, 2])
                    peg();//_hole_z(sock_w, use_cb);
                }
            
            //Side peg holes
            for(s = [-3, -1, 1, 3])
                for (y = [-1, 1])
                    translate([s * key_t, key_t * y, peg_h])
                    rotate([90, 0, 0])
                    peg();//_hole_y(sock_w);
            
        }
        //Front peg holes
        translate([-(body_x - key_t) * 0.5 - peg_h * 4, 0, 0])
        rotate([0, 90, 0])
        technic_beam_odd(5);
        translate([-(body_x - key_t) * 0.5 - peg_h * 4, 0, peg_h])
        rotate([0, 90, 0])
        technic_beam_odd(5);
    //    for(s = [-1, 1])
    //        translate([s * key_t, 0, 0])
    //        difference()
    //        {
    //            scale([1.25, 1, 1.25])
    //            rotate([90, 0, 0])
    //            peg();//_hole_y(sock_w);
    //            scale([1, (1 + cube_allowance)/1, 1])
    //            rotate([90, 0, 0])
    //            peg();//_hole_y(sock_w);
    //        }
    }

    //Counterweight mount//Usually just the outward facing peg walls, and 14 hole beams.
    //Servo mount
    //For mirrored version
    mirror([0, is_mirrored, 0])
    {
    translate([-peg_h * 2, peg_h * 2 - cube_allowance, 0])
    rotate([0, 90, 90])
    technic_beam_even(18);
    translate([-peg_h * 2, peg_h * 2 - cube_allowance, peg_h])
    rotate([0, 0, 90])
    technic_beam_even(18);

    translate([peg_pitch * 4.5, peg_h * 3 - cube_allowance, peg_h * 0.5])
    rotate([0, 0, 90])
    scale([1, 1, 2])
    technic_beam_odd(5);
    }    
    //Counterweight mount//Usually just the outward facing peg walls, and 14 hole beams.
    mirror([0, is_mirrored, 0])
    {
        mirror([0, 1, 0]){
            translate([-peg_h * 2, peg_h * 2 - cube_allowance, 0])
        rotate([0, 90, 90])
        technic_beam_even(18);
        translate([-peg_h * 2, peg_h * 2 - cube_allowance, peg_h])
        rotate([0, 0, 90])
        technic_beam_even(18);


        //    translate([peg_pitch * 4.5, peg_h * 3, 0])
        //    rotate([0, 0, 90])
        //    technic_beam_odd(5);
        }    
            
        //Servo counterweight ~= 9g at 100% = 7.26 cm^3... try 7.50 cm^3
        //counter_weight_xyz = 75;    
        //cube_rounded(counter_weight_xyz, counter_weight_xyz, counter_weight_xyz, 10);

        //The M12 hex nut dimensions are as follows: Width across flats: 18.00 mm (min) to 20.03 mm (max), and height of the nut: 10.80 mm (min) to 10.37 mm (max).
            
        M12_nut_r = 22/2;
        M12_nut_h = 8;
        rotate([90, 0, 0])
        //Invert sign of y translation for handedness.
        translate([peg_h * 4, peg_h * 0.125, (M12_nut_r + peg_pitch + cube_allowance * 1)])

        difference()
        {
            cube_rounded(M12_nut_r * 2, M12_nut_r * 2, M12_nut_h + peg_h, peg_h);   
            translate([0, 0, peg_h/2]) 
            rotate([0, 0, 30])
            cylinder_base(M12_nut_r, M12_nut_h + cube_allowance, 6);
            
        //    for(x = [-peg_h/2, peg_h/2])
        //        for(y = [- peg_h/2, peg_h/2])
        //            translate([x, y, -(M12_nut_h)/2])
        //            peg();
        }
    }
}

color("red")
translate([0, peg_h * 3.5, 0])
val_brooks(0);
color("gold")
translate([0, peg_h * -3.5, 0])
val_brooks(1);
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