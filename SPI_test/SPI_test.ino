#include "TMC5130A.h"
#include <SoftwareSerial.h>

// statically define a motor object. 
TMC5130A motorDriverC(10, 32, 9, 49, 48);
TMC5130A motorA(10, 32, 9, 49, 48);
TMC5130A motorB(10, 32, 9, 49, 48);
TMC5130A motorC(10, 32, 9, 49, 48);


void setup() {
  // must be called before your program runs. 
  motorDriverC.setup();

  //can be called any time before you wish the motor to move. 
  motorDriverC.enable();
}
void loop() {
  motorDriverC.set_rotations(1.0);
  delay(3500);
  motorDriverC.set_rotations(0.0);
  delay(2500);
  motorDriverC.set_rotations(-1.0);
  delay(2500);
}

