double Pi = 3.14159;
double HEIGHT = 3;
double ARMLENGTH = .2;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int temp = millis();
  double tempRange = random(0.0,400.0) / 100.0;
  double theta1 = 0;
  double range = 0;
  while(tempRange - range > .1){
    double CurrentRange = compute_range(theta1, HEIGHT, ARMLENGTH);
    if(CurrentRange > tempRange){
      theta1 -= .01;
    } else {
      theta1 += .01;
    }
    range = compute_range(theta1, HEIGHT, ARMLENGTH);
    //Serial.println(String(tempRange) + "," + String(range));
  }
  Serial.println("Seconds: " + String(millis() - temp));
  Serial.println("Error: " + String(tempRange - range));
  delay(500);
  
}


double compute_range (double theta1, double height, double armLength){
  double straightLength = armLength * sqrt(2 - 2 * cos(theta1)) / theta1;
  double theta2 = 3*Pi / 2 - 3 * theta1 / 2;
  double armHeight = straightLength * cos(theta2); // vertical 
  double beamHeight = height - armHeight;
  double rangeArm = straightLength * sin(theta2); 
  double rangeBeam = beamHeight * tan(theta1); 
  double range = rangeArm + rangeBeam;
  return(range);
}

