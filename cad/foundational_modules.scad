roundness      = 200;
cube_allowance = 0.5;
rad_allowance  = 0.25;
in_to_mm       = 25.4;

/***************************************************************************/
/***************************************************************************/
module cube_base(width, length, height)
{
    cube(size = [width, length, height], center = true);
}

module cylinder_base(radius, height, roundness)
{
    cylinder(r = radius, h = height, $fn = roundness, center = true);
}

module cone_base(radius1, radius2, height, roundness)
{
    cylinder(r1 = radius1, r2 = radius2, h = height, $fn = roundness, center = true);
}

module sphere_base(radius, roundness, centered)
{
    sphere(r = radius, $fn = roundness);
}
/***************************************************************************/
/***************************************************************************/
module cube_rounded(width, length, height, corner_width)
{    
    difference()
    {
        cube_base(width, length, height);
       
        for(i = [-1, 1])
        {
            for(j = [-1, 1])
            {
                translate([width/2 * i, length/2 * j, 0])
                cube_base(corner_width, corner_width, height + 0.1);
            }
        }
    }
    
    for(i = [-1, 1])
    {
        for(j = [-1, 1])
        {
            translate([(width - corner_width)/2 * i, (length - corner_width)/2 * j, 0])
            cylinder_base(corner_width * 0.5, height, roundness);
        }
    }
}
/***************************************************************************/
/***************************************************************************/
module cube_rounded_full(width, length, height, corner_width)
{
    for(i = [-1, 1])
    {
        for(j = [-1, 1])
        {
            for(k = [-1, 1])
            {
                translate([(width - corner_width)/2 * i, (length - corner_width)/2 * j, 0])
                cylinder_base(corner_width * 0.5, height - corner_width, roundness);
                
                translate([(width - corner_width)/2 * i, (length - corner_width)/2 * j, (height - corner_width)/2 * k])
     
                sphere_base(corner_width * 0.5, roundness);
                
            }
        }
    }
    
    for(i = [-1, 1])
    {
        for(j = [-1, 1])
        {
            
            rotate([90, 0, 0])
            translate([(width - corner_width)/2 * i, (height - corner_width)/2 * j, 0])
            cylinder_base(corner_width * 0.5, length - corner_width, roundness);
            
            rotate([0, 90, 0])
            translate([(height - corner_width)/2 * i, (length - corner_width)/2 * j, 0])
            cylinder_base(corner_width * 0.5, width - corner_width, roundness);
            
        }
    }
    
    cube_base(width, length - corner_width, height - corner_width);
    cube_base(width - corner_width, length, height - corner_width);
    cube_base(width - corner_width, length - corner_width, height);
}
/***************************************************************************/
/***************************************************************************/
//Basic module demonstrations:
module full_demonstration()
{
    cube_rounded_full(10, 10, 10, 5);

    translate([15, 0, 0])
    cube_rounded(10, 10, 10, 5);

    translate([30, 0, 0])
    cube_base(10, 10, 10);

    translate([-15, 0, 0])
    sphere_base(5, roundness);

    translate([-30, 0, 0])
    cylinder_base(5, 10, roundness);

    translate([0, 15, 0])
    cone_base(5, 2, 10, roundness);
}

//full_demonstration();
//cube_rounded_full(80, 180, 60, 20);
//cube_rounded_full(10, 10, 10, 5);