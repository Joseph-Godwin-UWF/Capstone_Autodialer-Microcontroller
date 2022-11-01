#include "MessageHandler.h"

MessageHandler::MessageHandler(String setUpHeader, String turnDialHeader, String setStepBitsHeader){
  this->setUpHeader = setUpHeader;
  this->turnDialHeader = turnDialHeader;
  this->setStepBitsHeader = setStepBitsHeader;
}
int MessageHandler::getAction(String header){
  if(header == this->setUpHeader)
    return this->UPDATE_STEPPER_PARAMETERS;
  else if(header == this->turnDialHeader)
    return this->ROTATE_DIAL;
  else if(header == this->setStepBitsHeader)
    return this->UPDATE_STEP_SIZE;
  else
    return this->INVALID_ACTION;
}
