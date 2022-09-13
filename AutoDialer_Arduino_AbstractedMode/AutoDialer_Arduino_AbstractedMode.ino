#include <AccelStepper.h>
#include "Messenger.h"
#include "MessageHandler.h"

#define pwmStepper 12
#define dirStepper 14


/* SETTING UP STEPPER MOTOR */
 float STEP_ANGLE = 1.8;
 float STEPS_PER_REV = 360 / STEP_ANGLE;
 int DIALING_SPEED = 400;
 int MAX_SPEED = 800;
 String initialMessageHeader = "SetUpStepper:";
 String turnDialHeader = "TurnDial:";
 const char* delimiter = ";";
 AccelStepper stepper(AccelStepper::DRIVER, pwmStepper, dirStepper);

 Messenger messenger;
 MessageHandler messageHandler(initialMessageHeader, turnDialHeader);
 
void setup() {
  Serial.begin(115200);
  Serial.println("Setting up...");
  //WAIT FOR INITIAL COMMUNICATION
  while(Serial.available() == 0) { delay(200); }
  blockUntilSetUpMessageIsReceived();
  stepper.setMaxSpeed(MAX_SPEED);
  stepper.setSpeed(200);
}

/**
 * FUNCTION DELCARATIONS
 */

void loop() {
  //WAIT FOR MESSAGE COMMUNICATION
  String error = getDataFromSerial();
  //Serial.println("Data still on line: " + error);
  while(Serial.available() == 0) { delay(200); }
  //STORE DATA RECEIVED IN A STRING
  String recv = getDataFromSerial();

  Serial.println("recv: " + recv);
  String header = recv.substring(0, recv.indexOf(':') + 1);
  int action = messageHandler.getAction(header);
  Serial.println("Header: -" + header + "-");
  switch(action){
    
    case MessageHandler::INVALID_ACTION:{
      Serial.println(messenger.STEPPER_SETUP_FAILED + recv);
      break;
    }
      
    case MessageHandler::UPDATE_STEPPER_PARAMETERS:{
      parseStepperSetupMessage(recv, STEP_ANGLE, DIALING_SPEED, MAX_SPEED);
      String stepperInfo = messenger.stepperMotorParametersToString(STEP_ANGLE, DIALING_SPEED, MAX_SPEED);
      Serial.println(messenger.STEPPER_SETUP_COMPLETE + stepperInfo);
      break;
    }
  
    case MessageHandler::ROTATE_DIAL:{
      Serial.println("rotate dial case");
      break;
    }

    default:
      break;
      //error
  }

}


/**
 * Collects data from serial monitor up until
 * a new line character is reached. Returns a
 * string, not including the newline char
 */
String getDataFromSerial(){
  String data;
  while(Serial.available() > 0) {
    char ch = Serial.read();
    if(ch == '\n')
      return data;
    data += ch;
  }
}

void blockUntilSetUpMessageIsReceived(){
  String recv = getDataFromSerial();
  if( isSetUpMessage(recv) ){
    parseStepperSetupMessage(recv, STEP_ANGLE, DIALING_SPEED, MAX_SPEED);
    String stepperInfo = messenger.stepperMotorParametersToString(STEP_ANGLE, DIALING_SPEED, MAX_SPEED);
    Serial.println(messenger.STEPPER_SETUP_COMPLETE + stepperInfo);
  }else{
    do{
      Serial.println(messenger.STEPPER_SETUP_FAILED + recv);
      while(Serial.available() == 0) { delay(200); }
      recv = getDataFromSerial();
    }while(!isSetUpMessage(recv));
    parseStepperSetupMessage(recv, STEP_ANGLE, DIALING_SPEED, MAX_SPEED);
    String stepperInfo = messenger.stepperMotorParametersToString(STEP_ANGLE, DIALING_SPEED, MAX_SPEED);
    Serial.println(messenger.STEPPER_SETUP_COMPLETE + stepperInfo);
  }
}

void parseStepperSetupMessage(String recv, float &stepAngle, int &dialingSpeed, int &maxspeed){
  /* REMOVE HEADER FROM DATA */
  int headerSizeToRemove = recv.indexOf(initialMessageHeader) + initialMessageHeader.length();
  recv.remove(0, headerSizeToRemove);
  /* c_str() returns a const char, so we can't use strtok()
   * on it directly. Thus, we must copy sequence.c_str() to
   * another cString */
   char cstringRecv[recv.length() + 1] = {};
   strcpy(cstringRecv, recv.c_str());

   String stepAngleString(strtok(cstringRecv, delimiter));
   String dialingSpeedString(strtok(NULL, delimiter));
   String maxSpeedString(strtok(NULL, delimiter));

   stepAngle = stepAngleString.toFloat();
   dialingSpeed = dialingSpeedString.toInt();
   maxspeed = maxSpeedString.toInt();

   
}

boolean isSetUpMessage(String recv){
  if(recv.indexOf(initialMessageHeader) < 0)
    return false;
  return true;
}
