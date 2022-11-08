 #include <Arduino.h>
 
 class Messenger{
    public:
      String INITIAL_SETUP_COMPLETE   = "000:";
      String STEP_RESOLUTION_SET      = "001:";
      String DIALING_SPEED_SET        = "002:";
      String REQUEST_NEXT_TURN        = "003:";
      String TORQUE_READING           = "004:";
      String THRESHOLD_TORQUE_REACHED = "555:";
      String INVALID_SETUP_MESSAGE    = "999:";
      
      String stepperMotorParametersToString(float stepAngle, int dialingSpeed, int maxspeed);
 };
