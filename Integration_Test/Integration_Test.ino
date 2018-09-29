#include "TMC5130A.h"
#include <SoftwareSerial.h>

double Pi = 3.14159;
double HEIGHT = 5;
double ARMLENGTH = .2;
double RADIUS = .5;


// statically define a motor object. 
TMC5130A motorDriverC(10, 32, 9, 49, 48);
TMC5130A motorDriverB(11, 31, 7, 47, 46);
TMC5130A motorDriverA(12, 30, 8, 45, 44);

void setup() {
  Serial.begin(9600);
  Serial.println("Begin");

  motorDriverA.setup();
  motorDriverB.setup();
  motorDriverC.setup();
  //can be called any time before you wish the motor to move. 
  motorDriverA.enable();
  motorDriverB.enable();
  motorDriverC.enable();
}

void loop() {
  // put your main code here, to run repeatedly:

  static float theta1 = 2;
  static float theta2 = 0;

  float deltas[3];
  compute_tendon_changes(theta1, theta2, deltas);
  
  motorDriverA.set_rotations(deltas[0]);
  motorDriverB.set_rotations(deltas[1]);
  motorDriverC.set_rotations(deltas[2]);
  theta2 += .1;
  delay(500);
  
}

/* 
 *  This function computes the required change in tendon length (from neutral) 
 *  for a given base angle (theta1 in execute_descent) and direction about the 
 *  default tenticle axis (z axis). 
 *  
 *  For the purposes of this function it is assumed that A is the 0 direction of 
 *  theta2. Such that for a theta2 value of 0 the tenticle would bend towords the 
 *  motor A tendon. 
 */
void compute_tendon_changes(float theta1, float theta2, float deltas[]){
  deltas[0] = RADIUS * (theta1) * cos(theta2 + 0 + PI);
  deltas[1] = RADIUS * (theta1) * cos(theta2 + (2/3)*PI + PI);
  deltas[2] = RADIUS * (theta1) * cos(theta2 + (4/3)*PI  + PI);
}

/*
 * This function uses gradiant descent to compute the angle made by the 
 * tendon base, the z axis (rest position) and the desired head position. 
 * 
 * It does this by computing the distance between where the proposed angle 
 * would point the head and the target location. The angle is then increased
 * or decreased as appropriate and the process begins again. 
 * 
 * Note: This function can take upwards of 10ms to execute depending on the 
 * specified precision. 
 * 
 * 
 */
double execute_descent (double rangeTarget, double height){
  double theta1 = 0.001; //starting value must be non-zero
  double rangeCurrent = compute_range(theta1, height, ARMLENGTH);
  double rangeOld = rangeCurrent;
  double stepSize = .1; //starting step size. 
  bool breakLoop = 1; // loop breaks when false. 
  while(breakLoop){
    if(rangeCurrent > rangeTarget){
      theta1 -= stepSize;
    } else {
      theta1 += stepSize;
    }
    rangeCurrent = compute_range(theta1, height, ARMLENGTH);
    Serial.println(abs(compute_cost(rangeTarget, rangeCurrent)));
    breakLoop = abs(compute_cost(rangeTarget, rangeCurrent)) > .01;
    Serial.println(abs(compute_cost(rangeTarget, rangeCurrent)) > .01);

    // decrease the size of 
    if (compute_cost(rangeTarget, rangeCurrent) * compute_cost(rangeTarget, rangeOld) < 0){
      stepSize = stepSize / 2;
    }
    double rangeOld = rangeCurrent;
  }
  return(theta1);
}

double compute_cost(double rangeTarget, double rangeCurrent){
  double cost = rangeTarget - rangeCurrent;
  //Serial.println(cost);
  return(cost);
}

double compute_range (double theta1, double height, double armLength){
  double straightLength = armLength * sqrt(2 - 2.0 * cos(theta1)) / theta1;
  //Serial.println(straightLength);
  double theta2 = theta1 / 2.0;
  //Serial.println(theta2);
  double armHeight = straightLength * cos(theta2); // vertical 
  double beamHeight = height - armHeight;
  double rangeArm = straightLength * sin(theta2); 
  double rangeBeam = beamHeight * tan(theta1); 
  double range = rangeArm + rangeBeam;
  //Serial.println(range);
  return(range);
}
