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
  byte toSend[4] = {0xAA, 0xBB, 0xCC, 0xDD};
  
  motorDriverC._read_register(0x21, &bytes[0]);
  motorDriverC._write_register(0x21, toSend);
  byte statusByte = motorDriverC.get_status();
  mySerial.print("status: ");
  mySerial.print(statusByte, HEX);
  mySerial.print(", data: ");
  mySerial.print((bytes[1]), HEX);
  mySerial.print((bytes[2]), HEX);
  mySerial.print((bytes[3]), HEX);
  mySerial.println((bytes[4]), HEX);
  motorDriverC._write_register(0x21, toSend);
  delay(1000);
  for(int i = 4; i > 1; i--){
    mySerial.println(i);
  }
}

void print_datagram(byte bytes[5]){
  
}

