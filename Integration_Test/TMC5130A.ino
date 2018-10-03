#include <Arduino.h>
#include <SPI.h>

#define TMC5130A_ADR_GCONF 0x00
#define TMC5130A_ADR_IHOLD_IRUN 0x10
#define TMC5130A_ADR_VSTART 0x23
#define TMC5130A_ADR_A1 0x24
#define TMC5130A_ADR_V1 0x25
#define TMC5130A_ADR_AMAX 0x26
#define TMC5130A_ADR_VMAX 0x27
#define TMC5130A_ADR_DMAX 0x28
#define TMC5130A_ADR_D1 0x2A
#define TMC5130A_ADR_VSTOP 0x2B
#define TMC5130A_ADR_TZEROWAIT 0x2C
#define TMC5130A_ADR_XTARGET 0x2D
#define TMC5130A_ADR_XACTUAL 0x21
#define TMC5130A_ADR_CHOPCONF 0x6C
#define TMC5130A_ADR_SWMODE 0x34
#define TMC5130A_ADR_RAMPSTAT 0x35
#define TMC5130A_ADR_COOLCONF 0x6D
#define TMC5130A_ADR_DRVSTATUS 0x6F
#define TMC5130A_ADR_TCOOLTHRS 0x14

#define TMC5130A_DEFAULT_IHOLD_IRUN 0x00070603
#define TMC5130A_DEFAULT_VSTART 0x0000000F
#define TMC5130A_DEFAULT_A1 0x000004F4
#define TMC5130A_DEFAULT_V1 0x0000C350
#define TMC5130A_DEFAULT_AMAX 0x000388B4
#define TMC5130A_DEFAULT_VMAX 0x0FFF2085
#define TMC5130A_DEFAULT_DMAX 0x000002BC
#define TMC5130A_DEFAULT_D1 0x00000578
#define TMC5130A_DEFAULT_VSTOP 0xF100000A
#define TMC5130A_DEFAULT_TZEROWAIT 0xF100000A
#define TMC5130A_DEFAULT_XTARGET 0x00000000
#define TMC5130A_DEFAULT_GCONF 0x0000004
#define TMC5130A_DEFAULT_CHOPCONF 0x000001C5
#define TMC5130A_DEFAULT_COOLCONF 0x01380000
#define TMC5130A_DEFAULT_TCOOLTHRS 0x00000000
#define TMC5130A_DEFAULT_SWMODE_STALLGUARD 0x00000000

#define TMC5130A_BITMASK_I_SCALE_ANALOG 0x00000001
#define TMC5130A_BITMASK_EN_PWM_MODE 0x00000004
#define TMC5130A_BITMASK_SWMODE_STALLGUARD 0x00000400
#define TMC5130A_BITMASK_STALL_THRESHOLD 0x002F0000
#define TMC5130A_BITMASK_BLANK 0x00000000



TMC5130A::TMC5130A(int chipSelectPin, int enablePin, int currentScalePin, int stepPin, int dirPin){
  SPI.begin();
  chipSelect = chipSelectPin;
  enableDevice = enablePin;
  currentScale = currentScalePin;
  stepControl = stepPin;
  directionControl = dirPin;
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

  pinMode(currentScale, OUTPUT);
  analogWrite(currentScale, 0);

  pinMode(stepControl, OUTPUT);
  digitalWrite(stepControl,LOW);
  
  pinMode(directionControl, OUTPUT);
  digitalWrite(directionControl,LOW);

  TMC5130A::set_home();
  TMC5130A::get_RAMPSTAT();
  TMC5130A::set_XTARGET(TMC5130A_DEFAULT_XTARGET);
  TMC5130A::set_VSTART(TMC5130A_DEFAULT_VSTART);
  TMC5130A::set_V1(TMC5130A_DEFAULT_V1);
  TMC5130A::set_A1(TMC5130A_DEFAULT_A1);
  TMC5130A::set_AMAX(TMC5130A_DEFAULT_AMAX);
  TMC5130A::set_VMAX(TMC5130A_DEFAULT_VMAX);
  TMC5130A::set_D1(TMC5130A_DEFAULT_D1);
  TMC5130A::set_VSTOP(TMC5130A_DEFAULT_VSTOP);
  TMC5130A::set_TZEROWAIT(TMC5130A_DEFAULT_TZEROWAIT);
  TMC5130A::set_GCONF(TMC5130A_DEFAULT_GCONF);
  TMC5130A::set_IHOLD_IRUN(TMC5130A_DEFAULT_IHOLD_IRUN);
  TMC5130A::set_CHOPCONF(TMC5130A_DEFAULT_CHOPCONF);
  TMC5130A::set_COOLCONF(TMC5130A_DEFAULT_COOLCONF);
  TMC5130A::set_TCOOLTHRS(TMC5130A_DEFAULT_TCOOLTHRS);
  TMC5130A::set_SWMODE(TMC5130A_DEFAULT_SWMODE_STALLGUARD);

  
  TMC5130A::set_ramp();
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
  analogWrite(currentScale, 100);
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

String TMC5130A::get_DRV(){
  byte bytes[5];
  TMC5130A::_read_register(TMC5130A_ADR_DRVSTATUS, bytes);
  return(datagram_to_string(bytes));
}

// used to clear stall guard conditions. 
String TMC5130A::get_RAMPSTAT(){
  byte bytes[5];
  TMC5130A::_read_register(TMC5130A_ADR_RAMPSTAT, bytes);
  return(datagram_to_string(bytes));
}

/* 
 *  Moves the motor to the specified location.
 *  
 *  Movement is conducted in absolute cordinates relative to the zero 
 *  position of the motor driver. The input value may not exceed +-42107.
 *  
 *  Param: rotations - the desired location in number of rotations as a float. 
 */
void TMC5130A::set_rotations(float rotations){
  long usteps = rotations * 200 * 256;
  TMC5130A::set_XTARGET(usteps);
}

/*
 * Zeros the device's internal home location. 
 */
void TMC5130A::set_home(){
  TMC5130A::set_XACTUAL(0x00000000);
}

/*
 * This function uses stallGuard collision control to home the motor. 
 * 
 * This process has the following steps:
 *    - Move the motor one turn in the direction the motor will not be homing. 
 *    - Set the stallGuard settings. 
 *    - Begin motion in the homing direction. 
 *    - Return once a collision has occured, or the specified timeout has elapsed. 
 * 
 * Param: directions - determines if the motor will home in the "positive" or "negative" direction. 
 */
void TMC5130A::go_home(bool directions){
  const long backOff = 1.0; // distance to back off before homing. 
  const long homeDistance = 5.0; // allowable turns before homing timeout. 
  
  // Set the positive/negative homing direction. 
  long directionModifier = -1;
  if(directions){ directionModifier = 1;};

  // Move away from the intended home direction to build up speed.
  long current = TMC5130A::get_XACTUAL();
  TMC5130A::set_rotations(current + -1 * directionModifier * backOff);
  delay(2000);
  TMC5130A::set_rotations(current + directionModifier * homeDistance);
  delay(10000); //time to home per axis. 
  TMC5130A::set_home();
  
  /*
  // Enable stallGuard. 
  // Move towards the target end stop. 
  
  TMC5130A::set_GCONF(TMC5130A_BITMASK_BLANK);
  current = TMC5130A::get_XACTUAL();
  TMC5130A::set_rotations(current + directionModifier * homeDistance);
  delay(1000);
  TMC5130A::set_SWMODE(TMC5130A_BITMASK_BLANK);
  TMC5130A::set_SWMODE(TMC5130A_BITMASK_SWMODE_STALLGUARD);
  Serial.println(TMC5130A::get_DRV());
  TMC5130A::get_RAMPSTAT();
  Serial.println(TMC5130A::get_DRV());
  // Dissable stallGuard. 
  delay(25000);
  
  TMC5130A::set_SWMODE(TMC5130A_BITMASK_BLANK);
  Serial.println(TMC5130A::get_DRV());
  TMC5130A::set_home();
  */
}

/*
 * This function sets the device to ramp mode.
 * 
 * This should be called during class initialization, before 
 * any attempt is made to set the target or current locations. 
 */
void TMC5130A::set_ramp(){
  byte rampBytes[] = {0x00, 0x00, 0x00, 0x00};
  TMC5130A::_write_register(0x20, rampBytes);
}

/*
 * An internal function for setting an arbitrary register. 
 * 
 * This should be called by adress wrappers. If you need to 
 * access a register for which there is no wrapper create a new one. 
 */
void TMC5130A::_set_register(byte address, long value){
  byte bytes[4];
  TMC5130A::_setByteArray(value, bytes);
  TMC5130A::_write_register(address, bytes);
}

void TMC5130A::set_VSTART(long value){
  TMC5130A::_set_register(TMC5130A_ADR_VSTART, value); 
}
void TMC5130A::set_A1(long value){
  TMC5130A::_set_register(TMC5130A_ADR_A1, value); 
}
void TMC5130A::set_V1(long value){
  TMC5130A::_set_register(TMC5130A_ADR_V1, value); 
}
void TMC5130A::set_AMAX(long value){
  TMC5130A::_set_register(TMC5130A_ADR_AMAX, value); 
}
void TMC5130A::set_VMAX(long value){
  TMC5130A::_set_register(TMC5130A_ADR_VMAX, value); 
}
void TMC5130A::set_DMAX(long value){
  TMC5130A::_set_register(TMC5130A_ADR_DMAX, value); 
}
void TMC5130A::set_D1(long value){
  TMC5130A::_set_register(TMC5130A_ADR_D1, value); 
}
void TMC5130A::set_VSTOP(long value){
  TMC5130A::_set_register(TMC5130A_ADR_VSTOP, value); 
}
void TMC5130A::set_TZEROWAIT(long value){
  TMC5130A::_set_register(TMC5130A_ADR_TZEROWAIT, value); 
}
void TMC5130A::set_XTARGET(long value){
  TMC5130A::_set_register(TMC5130A_ADR_XTARGET, value); 
}
void TMC5130A::set_GCONF(long value){
  TMC5130A::_set_register(TMC5130A_ADR_GCONF, value); 
}
void TMC5130A::set_IHOLD_IRUN(long value){
  TMC5130A::_set_register(TMC5130A_ADR_IHOLD_IRUN, value); 
}
void TMC5130A::set_XACTUAL(long value){
  TMC5130A::_set_register(TMC5130A_ADR_XACTUAL, value); 
}
void TMC5130A::set_CHOPCONF(long value){
  TMC5130A::_set_register(TMC5130A_ADR_CHOPCONF, value); 
}
void TMC5130A::set_SWMODE(long value){
  TMC5130A::_set_register(TMC5130A_ADR_SWMODE, value);
}
void TMC5130A::set_COOLCONF(long value){
  TMC5130A::_set_register(TMC5130A_ADR_COOLCONF, value);
}
void TMC5130A::set_TCOOLTHRS(long value){
  TMC5130A::_set_register(TMC5130A_ADR_TCOOLTHRS, value);
}

/*
 * This function returns the current XACTUAL value as a long. 
 */
long TMC5130A::get_XACTUAL(){
  byte bytes[5];
  TMC5130A::_read_register(TMC5130A_ADR_XACTUAL, bytes); 
  long actual = 0;
  actual += bytes[4] << 24;
  actual += bytes[3] << 16;
  actual += bytes[2] << 8;
  actual += bytes[1];
  return(actual);
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
  _send_datagram(targetRegister, 0x10000000, &returnData[0]);
}

void TMC5130A::_send_datagram(byte address, byte value[4], byte returnData[5]){
  
  digitalWrite(chipSelect, LOW);
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE3));
  returnData[0] = SPI.transfer(address);
  for(int i = 0; i < 4; i++){
    returnData[i+1] = SPI.transfer(value[i]);
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
  for(int i = 0; i < 4; i++){
    int temp = i * 8;
    Array[3 - i] = (value >> temp) & 0xFF;
  }
}

/*
 * This function takes a datagram and returns a printable string. 
 * 
 * This is a convience function intended for use during debug, and
 * returns a string which is formatted as follows: 
 * "Status: [0xSS], Data: [DD,DD,DD,DD]"
 * 
 * Param datagram is a 5 byte array containing the responce data from 
 * a TMC4130A in MSB format, including the status byte. 
 */
String TMC5130A::datagram_to_string(byte datagram[5]){
  String returnString = "status: ";
  returnString += String(datagram[0], HEX);
  returnString += ", data: ";
  returnString += String(datagram[1], HEX);
  returnString += ",";
  returnString += String(datagram[2], HEX);
  returnString += ",";
  returnString += String(datagram[3], HEX);
  returnString += ",";
  returnString += String(datagram[4], HEX);
  return(returnString);
}
