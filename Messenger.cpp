#include "Messenger.h"

String Messenger::stepperMotorParametersToString(float stepAngle, int dialingSpeed, int maxspeed){
  String message;
  message.concat("STEP_ANGLE{" + String(stepAngle) + "} ");
  message.concat("DIALING_SPEED{" + String(dialingSpeed) + "} ");
  message.concat("MAX_SPEED{" + String(maxspeed) + "} ");
  return message;
}
