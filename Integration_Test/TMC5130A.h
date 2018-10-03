#include "Arduino.h"

class TMC5130A {
public:
    //setup, these should generally be called in order during a program. 
    TMC5130A(int, int, int, int, int); // inializes class, should be called immediatly upon system startup.
    void setup(); // initialize registers and palce device pins in "safe" mode. 
    void enable(); // set control pins to allow motor to turn. 
    void disable(); // prevent motor output.

    // main API functions. 
    void set_home(); // zero the current location register. 
    void go_home(bool); //Enables stallGuard and uses it to home to the end of an axis. 
    void set_rotations(float); // go to absolute position relative to home, units of rotations.
    byte get_status(); // returns the status byte for the chip.
    String get_DRV();
    String get_RAMPSTAT(); // clear Stall Guard conditions. 

    //advanced control options. 
    void _set_register(byte, long);// sets an arbitrary register on the device. 
    void _read_register(byte, byte[5]);
    void set_XTARGET(long);
    void set_XACTUAL(long);

    // current control settings. 
    void set_analog_scale(); // sets corresponding GCONF bit.
    void set_IHOLD_IRUN(long); //set the current control register. 
    void set_TPOWERDOWN(long);

    //convinience functions. 
    String datagram_to_string(byte [5]); // returns a datagram formatted as a string. 
    
private:
    // device control pins. 
    int chipSelect; // Stores the CS pin for this driver, set by init.
    int enableDevice; //enables the motor driver power output, active low.
    int currentScale; //sets the hardware max motor current between 0 and 2A
    int stepControl; // Step input for motor driver. 
    int directionControl; // direction input for motor driver. 

    // motion controller settings, generally these can be left as-is. 
    void set_ramp(); //enables ramp mode. 
    void set_VSTART(long); //sets the starting speed of the motor, below this acceleration is infinate.
    void set_V1(long); // sets the max speed for the initial acceleration phase.
    void set_A1(long); // sets acceleration for the initial acceleration phase. 
    void set_AMAX(long); // sets the acceleration for the main acceleration phase.
    void set_VMAX(long); // sets the absolute max speed of the motor. 
    void set_DMAX(long); // sets the absolute maximum deceleration speed. 
    void set_D1(long); // deceleration between V1 and VSTOP. 
    void set_VSTOP(long); //sets the velocity at which the motor just stops (should be low). 
    void set_TZEROWAIT(long); // sets the delay between subsequent motor moves. 
    void set_CHOPCONF(long); // allows configuration of the chop settings. 
    void set_GCONF(long); // set the general configuration register. 
    void set_SWMODE(long); // sets configuration registers relating to stallguard and end stops. 
    void set_COOLCONF(long ); // sets a number of stallGuard features.
    void set_TCOOLTHRS(long); // determines the time between steps below which stallGuard will operate. 

    byte _get_register(byte);
    long get_XACTUAL(); // gets the current XACTUAL value in rotations. 
    void _write_register(byte, byte[5]);
    void _send_datagram(byte, byte[5], byte[5]);

    //convinience functions. 
    void _error(int);
    void _setByteArray(long, byte[4]); // converts byte values to byte arrays
    
};
