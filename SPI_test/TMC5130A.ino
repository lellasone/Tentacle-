#include <Arduino.h>
#include <SPI.h>

#define TMC5130A_ADR_VSTART 0x23
#define TMC5130A_ADR_A1 0x24
#define TMC5130A_ADR_V1 0x25
#define TMC5130A_ADR_VSTOP 0x2B


TMC5130A::TMC5130A(int chipSelectPin, int enablePin){
  SPI.begin();
  chipSelect = chipSelectPin;
  enableDevice = enablePin;
  //setup and dissable SPI chip select. 
}

/*
 * This function sets up the control pins for the motor controller. 
 * 
 * It should be called after the class has been instantiated, but before
 * any attempt is made to communicate with the motor driver. 
 */
void TMC5130A::setup(){
  pinMode(chipSelect, OUTPUT);
  digitalWrite( chipSelect, HIGH);

  //setup the enable pin, and disable the chip. 
  pinMode(enableDevice, OUTPUT);
  digitalWrite(enableDevice, HIGH);

  TMC5130A::set_ramp();
  byte value[4];
  TMC5130A::set_VSTART(0x00000000);
  TMC5130A::set_VSTOP(0x0000000B);
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

/*
 * Returns the motor controller status byte in MSB first form. 
 * 
 * bit values run as follows: (See datasheet page 22)
 * 7 - Signals stop right switch status. 
 * 6 - Signals stop left switch status. 
 * 5 - Signals target reached.
 * 4 - Signals target velocity reached.
 * 3 - Signals motor stand still.
 * 2 - Signals stallguard flag active. 
 * 1 - Signals driver 1 error. 
 * 0 - Signals reset has occured (and GSTAT has not been read). 
 * 
 * Returns: Byte. 
 */
byte TMC5130A::get_status(){
  byte bytes[5];
  TMC5130A::_read_register(0x00, bytes);
  return(bytes[0]);
}

/*
 * This function sets the device to ramp mode.
 * 
 * This should be called during class initialization, before 
 * any attempt is made to set the target or current locations. 
 */
void TMC5130A::set_ramp(){
  byte rampBytes[] = {0x00, 0x00, 0x00, 0xFF};
  TMC5130A::_write_register(0x20, rampBytes);
}

void TMC5130A::set_VSTART(long value){
  byte bytes[4];
  TMC5130A::_setByteArray(value, bytes);
  TMC5130A::_write_register(TMC5130A_ADR_VSTART, bytes);
}

void TMC5130A::set_VSTOP(long value){
  byte bytes[4];
  TMC5130A::_setByteArray(value, bytes);
  TMC5130A::_write_register(TMC5130A_ADR_VSTOP, bytes);
}

/*
 * This function enables or dissables stealth chop. 
 * 
 * Param enableStealth enable if true, dissable if false. 
 */
void TMC5130A::enable_stealth(bool enableStealth){
  byte registerValue[5];
  TMC5130A::_read_register(0x00, registerValue);
  
  if(enableStealth){
    registerValue[5] = registerValue[5] | 0x04;
  } else {
    registerValue[5] = registerValue[5] & 0x04;
  }
  TMC5130A::_write_register(0x00, registerValue[1]);
}
/*
 * Write a value to a register onboard the motor driver.
 * 
 * @Param targetRegister The register to which a value is to be written. 
 *        Note that this is the 7 bit value, the MSB will be disregarded. 
 * @Param value the 4 byte word to be written. Formatted as an array. 
 * @Return None. 
 */
void TMC5130A::_write_register(byte address, byte value[4] ){
  byte data[5];
  address = address + 0x80;
  _send_datagram(address, value, &data[0]);
}

/*
 * This function reads the specified target register.
 * 
 * Data is returned through the provided returnData pointer. That pointer
 * must point to a 5 byte array. The status byte is provided as the first 
 * byte and should be disregarded for most purposes. 
 * 
 * Param targetRegister The register to read. The MSB should be 0. 
 * Param returnData The array into which read data should be placed. 
 */
void TMC5130A::_read_register(byte targetRegister, byte returnData[5]){
  byte *data = returnData;
  targetRegister = targetRegister & 0x7F;
  _send_datagram(targetRegister, 0x10000000, &returnData[0]);
  _send_datagram(targetRegister, 0x10000000, &returnData[0]);
}

void TMC5130A::_send_datagram(byte address, byte value[4], byte returnData[5]){
  
  digitalWrite(chipSelect, LOW);
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE3));
  returnData[0] = SPI.transfer(address);
  for(int i = 0; i < 4; i++){
    returnData[i+1] = SPI.transfer(value[i-1]);
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

void TMC5130A::_setByteArray(long value, byte Array[4]){
  long valuet = 0x00AA00FF;
  for(int i = 0; i < 4; i++){
    int temp = i * 8;
    Array[3 - i] = (value >> temp) & 0xFF;
  }
}

