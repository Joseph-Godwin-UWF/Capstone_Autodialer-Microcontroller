#include "MessageHandler.h"

MessageHandler::MessageHandler(String setUpHeader, String turnDialHeader){
  this->setUpHeader = setUpHeader;
  this->turnDialHeader = turnDialHeader;
}
int MessageHandler::getAction(String header){
  if(header == this->setUpHeader)
    return this->UPDATE_STEPPER_PARAMETERS;
  else if(header == this->turnDialHeader)
    return this->ROTATE_DIAL;
  else
    return this->INVALID_ACTION;
}
