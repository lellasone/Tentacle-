double Pi = 3.14159;
double HEIGHT = 5;
double ARMLENGTH = .2;


void setup() {
  Serial.begin(9600);
  Serial.println("Begin");
}

void loop() {
  // put your main code here, to run repeatedly:
  int temp = millis();
  double tempRange = random(0.0,400.0) / 100.0;
  double theta1 = execute_descent(tempRange, HEIGHT);
  double range = compute_range(theta1, HEIGHT, ARMLENGTH);
  
  Serial.println("");
  Serial.println("Target: " + String(tempRange));
  Serial.println("Seconds: " + String((millis() - temp) / 1000.0));
  Serial.println("Error: " + String(tempRange - range));
  delay(2000);
  
}

double execute_descent (double rangeTarget, double height){
  double theta1 = 0.001;
  double rangeCurrent = compute_range(theta1, height, ARMLENGTH);
  double rangeOld = rangeCurrent;
  double stepSize = .002;
  bool breakLoop = 1;
  while(breakLoop){
    double rangeCurrent = compute_range(theta1, height, ARMLENGTH);
    if(rangeCurrent > rangeTarget){
      theta1 -= stepSize;
    } else {
      theta1 += stepSize;
    }
    //Serial.println(abs(compute_cost(rangeTarget, rangeCurrent)));
    breakLoop = abs(compute_cost(rangeTarget, rangeCurrent)) > .05;
    //Serial.println(abs(compute_cost(rangeTarget, rangeCurrent)) > .05);
    //if (compute_cost(rangeTarget, rangeCurrent) * compute_cost(rangeTarget, rangeOld) < 0){
    //  stepSize = stepSize / 2;
    //}
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

