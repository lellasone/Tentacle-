#include "Arduino.h"

class Kinematics {
  public:
    Kinematics(); //Initializes class
    //set motor positions given location input
    
  private:
    //home positions of motors A, B, C
    //desired positional of motors A, B, C
    float theta; //elevation
    float phi; //azimuth
    void _coordinate_transform(float, float, float); //convert (x,y,z) location to (r,theta,phi) location
    //convert (r,theta,phi) location to tendon lengths
    //convert tendon lengths to calls of set_rotations
    float _arctangent(float); //approximation of arctangent
};

