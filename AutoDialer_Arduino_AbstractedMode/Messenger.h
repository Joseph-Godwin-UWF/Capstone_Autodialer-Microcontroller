 #include <Arduino.h>
 
 class Messenger{
    public:
      String INITIAL_SETUP_COMPLETE = "000:";
      String INVALID_SETUP_MESSAGE   = "999:";
      String REQUEST_NEXT_ANGLE = "001-> ";
      String TORQUE_READING = "003->";
      String THRESHOLD_TORQUE_REACHED = "002->";

      String stepperMotorParametersToString(float stepAngle, int dialingSpeed, int maxspeed);
 };
