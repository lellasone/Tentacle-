//#include "TMC5130A.h"

#define MAX_ROTATIONS 20
#define MIN_ROTATIONS -20

/*
 * approximation of arctangent
 */
float Kinematics::_arctangent(float arg){
  float abs_arg = abs(arg);
  float ans;
  if (abs_arg<1) ans = (abs_arg - (pow(abs_arg,3)/3) + (pow(abs_arg,5)/5)); // beginning of the Taylor series for artangent(arg) about 0
  else if (abs_arg<3) ans = (1.107 + (abs_arg-2)/5 - (2/25 * pow((abs_arg-2),2))); // beginning of the Taylor series for artangent(arg) about 2
  else if (abs_arg<8) ans = 1.5708 - 0.322 * (8 - abs_arg); // linear interpolation between arctangent(3) and arctangent(infinity)
  else ans = 1.5708; // after about 8, arctangent is pretty close to Pi/2

  return (arg / abs_arg * ans);
}

/*
 * convert (x,y,z) location to (r,theta,phi) location
 */
void Kinematics::_coordinate_transform(float x, float y, float z){
    theta = _arctangent(pow((x*x+y*y),0.5)/z); // determines the elevation (theta = 0 => straight down)
    if (x>0) phi = _arctangent(y/x); // determines the azimuth
    else phi = _arctangent(y/x) + 1.5708; // plus Pi/2
}

