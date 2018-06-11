#include "TMC5130A.h"
#include <SoftwareSerial.h>

TMC5130A motorDriverC(10, 32);
SoftwareSerial mySerial(14, 15);

void setup() {
  // put your setup code here, to run once:
  motorDriverC.setup();
  mySerial.begin(9600);
  mySerial.println("READY");
}

void loop() {
  // put your main code here, to run repeatedly:
  byte bytes[5];
  motorDriverC.set_ramp();
  motorDriverC._read_register(0x20, bytes);
  print_datagram(bytes);
  delay(2000);
}

void print_datagram(byte bytes[5]){
  mySerial.print("status: ");
  mySerial.print(bytes[0], HEX);
  mySerial.print(", data: ");
  mySerial.print((bytes[1]), HEX);
  mySerial.print((bytes[2]), HEX);
  mySerial.print((bytes[3]), HEX);
  mySerial.println((bytes[4]), HEX);
}

