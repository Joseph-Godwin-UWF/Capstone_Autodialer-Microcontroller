#include "MessageHandler.h"

MessageHandler::MessageHandler(){
  
}

MessageHandler::MessageHandler(String setUpHeader, String turnDialHeader, String setStepBitsHeader){
  this->setUpHeader = setUpHeader;
  this->turnDialHeader = turnDialHeader;
  this->setStepBitsHeader = setStepBitsHeader;
}

void MessageHandler::setInitialMessageHeader(String header){
  this->setUpHeader = header;
}

void MessageHandler::setSetStepBitsHeader(String header){
  this->setStepBitsHeader = header;
}

void MessageHandler::setSetDialingSpeedHeader(String header){
  this->setDialingSpeedHeader = header;
}

void MessageHandler::setTurnDialHeader(String header){
  this->turnDialHeader = header;
}

void MessageHandler::setCalibrateTorqueThresholdHeader(String header){
  this->calibrateTorqueThresholdHeader = header;
}


int MessageHandler::getAction(String header){
  if(header == this->setUpHeader)
    return this->INITIAL_SETUP;
  else if(header == this->turnDialHeader)
    return this->ROTATE_DIAL;
  else if(header == this->setStepBitsHeader)
    return this->UPDATE_STEP_SIZE;
  else if(header == this->setDialingSpeedHeader)
    return this->UPDATE_DIALING_SPEED;
  else if(header == this->calibrateTorqueThresholdHeader)
    return this->CALIBRATE_TORQUE_THRESHOLD;
  else
    return this->INVALID_ACTION;
}
