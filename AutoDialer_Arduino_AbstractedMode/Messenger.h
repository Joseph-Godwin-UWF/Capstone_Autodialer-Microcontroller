 #include <Arduino.h>
 
 class Messenger{
    public:
      String STEPPER_SETUP_COMPLETE = "000-> ";
      String STEPPER_SETUP_FAILED   = "600-> ";
      String REQUEST_NEXT_ANGLE = "001-> ";

      String stepperMotorParametersToString(float stepAngle, int dialingSpeed, int maxspeed);
 };
