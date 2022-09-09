#include "Messenger.h"

String Messenger::stepperMotorParametersToString(float stepAngle, int dialingSpeed, int maxspeed){
  String message;
  message += String(stepAngle);
  message += ";";
  message += String(dialingSpeed);
  message += ";";
  message += String(maxspeed);
  return message;
}
