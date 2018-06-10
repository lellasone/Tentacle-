#include <Arduino.h>
#include <SPI.h>

TMC5130A::TMC5130A(int chipSelectPin, int enablePin){
  SPI.begin();
  chipSelect = chipSelectPin;
  enableDevice = enablePin;
  //setup and dissable SPI chip select. 
}

void TMC5130A::setup(){
  pinMode(chipSelect, OUTPUT);
  digitalWrite( chipSelect, HIGH);

  //setup the enable pin, and disable the chip. 
  pinMode(enableDevice, OUTPUT);
  digitalWrite(enableDevice, HIGH);
}

/*
 * Enables the chip before use. 
 * 
 * This function, sets the control pins to allow the device to operate
 * normally. Note that it must be called once after init, and again
 * after each disable call when the device is to be used. 
 */
void TMC5130A::enable(){
  digitalWrite(enableDevice, LOW); 
}

byte TMC5130A::get_status(){
  byte bytes[5];
  TMC5130A::_read_register(0x00, bytes);
  return(bytes[0]);
}

/*
 * Write a value to a register onboard the motor driver.
 * 
 * @Param targetRegister The register to which a value is to be written. 
 *        Note that this is the 7 bit value, the MSB will be disregarded. 
 * @Param value the 4 byte word to be written. Formatted as an array. 
 * @Return None. 
 */
void TMC5130A::_write_register(byte targetRegister, byte value[5] ){
  byte data[5];
  targetRegister = targetRegister + 0x80;
  _send_datagram(targetRegister, value, &data[0]);
}

void TMC5130A::_read_register(byte targetRegister, byte returnData[5]){
  byte *data = returnData;
  _send_datagram(targetRegister, 0x00000000, &returnData[0]);
}

void TMC5130A::_send_datagram(byte address, byte value[4], byte returnData[5]){
  digitalWrite(chipSelect, LOW);
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE3));
  returnData[0] = SPI.transfer(address);
  for(int i = 4; i > 0; i--){
    returnData[i] = SPI.transfer(value[i-1]);
  }
  SPI.endTransaction();
  digitalWrite(chipSelect, HIGH);
}

/*
 * Error handeling function. Currently does nothing. 
 * 
 * Error codes are as follows: 
 * 
 */
void TMC5130A::_error(int errorNumber){
  
}

