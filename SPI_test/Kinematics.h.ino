#include "Arduino.h"
//#include "TMC5130A.h"

class Kinematics {
  public:
    Kinematics(TMC5130A, TMC5130A, TMC5130A, float, float, float); //Initializes class
    void move_arm(float, float, float); // Takes an (x,y,z) position and commands the arm to articulate to point at that position
    
  private:
    TMC5130A motor_A;
    TMC5130A motor_B;
    TMC5130A motor_C;
    float motor_target_A;
    float motor_target_B;
    float motor_target_C; //desired positions of motors A, B, C
    float x;
    float y;
    float theta; //elevation
    void _coordinate_transform(float, float, float); //convert (x,y,z) location to (x,y,theta) location
    float _arctangent(float); //approximation of arctangent
    void _find_zero(TMC5130A); //zeros a motor
    void _calculate_motor_positions(); //convert (x,y,theta) locations to desired motor locations
    void _move_motors(); //checks that motor targets are within bounds and moves motors
};

