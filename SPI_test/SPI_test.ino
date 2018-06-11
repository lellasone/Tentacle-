#include "TMC5130A.h"
#include <SoftwareSerial.h>

TMC5130A motorDriverC(10, 32, 9, 49, 48);
SoftwareSerial mySerial(14, 15);

void setup() {
  // put your setup code here, to run once:
  motorDriverC.setup();
  mySerial.begin(9600);
  mySerial.println("READY");
  motorDriverC.enable();
  pinMode(49, OUTPUT);
  digitalWrite(49,LOW);
  pinMode(48, OUTPUT);
  digitalWrite(48,LOW);
  
  byte data[4];
  motorDriverC.TMC5130A::_read_register(0x6C, data);
  print_datagram(data);
  motorDriverC.TMC5130A::_read_register(0x21, data);
  mySerial.print("actual: ");
  print_datagram(data);
}
void loop() {
  motorDriverC.set_rotations(1.0);
  delayByReading(3500);
  motorDriverC.set_rotations(0.0);
  delayByReading(2500);
  motorDriverC.set_rotations(-1.0);
  delayByReading(2500);
}

void delayByReading(int timeDelay){
  byte data[4];
  for(int i = 0; i < timeDelay ; i = i + 200){
    motorDriverC.TMC5130A::_read_register(0x21, data);
    mySerial.print("actual: ");
    print_datagram(data);
    motorDriverC.TMC5130A::_read_register(0x2D, data);
    mySerial.print("target: ");
    print_datagram(data);
    delay(100);
  }
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

