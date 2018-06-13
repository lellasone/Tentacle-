#define MAX_ROTATIONS 20
#define MIN_ROTATIONS -20

Kinematics::Kinematics(TMC5130A motorA, TMC5130A motorB, TMC5130A motorC, float home_A, float home_B, float home_C){
  motor_A = motorA;
  motor_B = motorB;
  motor_C = motorC;
  _find_zero(motor_A);
  _find_zero(motor_B);
  _find_zero(motor_C);
  motor_A.set_rotations(home_A);
  motor_B.set_rotations(home_B);
  motor_C.set_rotations(home_C);
  motor_A.set_home();
  motor_B.set_home();
  motor_C.set_home();
}

/*
 * Receives coordinates from serial parser and commands the arm to move to the indicated location
 */
void Kinematics::move_arm(float x_location, y_location, z_location){
  _coordinate_transform(x_location, y_location, z_location);
  _calculate_motor_positions(); // Not passing anything in because desired motor locations are stored in private variables
  _move_motors();
}

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
 * convert (x,y,z) location to (x,y,theta) location
 */
void Kinematics::_coordinate_transform(float x_des, float y_des, float z_des){
    x = x_des;
    y = y_des;
    theta = _arctangent(pow((x*x+y*y),0.5)/z_des); // determines the elevation (theta = 0 => straight down)
}

/*
 * Sends a motor to its end stop and sets that location as home
 * Currently empty because we still have to integrate limit switches
 */
void Kinematics::_find_zero(TMC5130A motor){
  //go to limit switch and set position to zero
}

/*
 * Converts an (x,y,theta) location into tendon lengths and commands the motors to move to effect those lengths
 */
void Kinematics::_calculate_motor_positions(){
  // a bunch of math
  
}

void Kinematics::_move_motors(){
  //Checks that motor targets are within acceptable range
  //if so, moves motors to targets
}

