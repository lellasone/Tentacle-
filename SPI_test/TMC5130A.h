#include "Arduino.h"

class TMC5130A {
public:
    TMC5130A(int, int); // inializes class, should be called immediatly upon system startup.
    void enable(); // sets registers and activates enable pin. Should be called before use. 
    void disable(); // re-sets the enable pin. 
    void setup();

    byte get_status(); // returns the status byte for the chip.
    void enable_stealth(bool); // enables or dissables stealthchop.

    void set_ramp(); //enables ramp mode. 
    void set_VSTART(long); //set the VSTART speed. 
    void set_V1(long);
    void set_A1(long);
    void set_AMAX(long);
    void set_VMAX(long);
    void set_DMAX(long);
    void set_D1(long);
    void set_VSTOP(long);
    void set_TZEROWAIT(long);
    void set_XTARGET(long);
    
    void _write_register(byte, byte[5]);
    void _read_register(byte, byte[5]);
    
private:
    int chipSelect; // Stores the CS pin for this driver, set by init.
    int enableDevice; //enables the motor driver power output.
    
    void _send_datagram(byte, byte[5], byte[5]);
    void _set_register(byte, long);// sets an arbitrary register on the device. 


    void _error(int);
    void _setByteArray(long, byte[4]); // converts byte values to byte arrays
    
};
