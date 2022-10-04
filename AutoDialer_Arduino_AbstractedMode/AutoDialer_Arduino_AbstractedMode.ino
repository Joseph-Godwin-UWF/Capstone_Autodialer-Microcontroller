#include <AccelStepper.h>
#include "Messenger.h"
#include "MessageHandler.h"

#define pwmStepper 16
#define dirStepper 17


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
  while (Serial.available() == 0) {
    delay(200);
  }
  blockUntilSetUpMessageIsReceived();
  stepper.setMaxSpeed(MAX_SPEED);
  stepper.setSpeed(200);
}

/**
   FUNCTION DELCARATIONS
*/

void loop() {
  //WAIT FOR MESSAGE COMMUNICATION
  //String error = getDataFromSerial();
  //Serial.println("Data still on line: " + error);
  while (Serial.available() == 0) {
    delay(200);
  }
  //STORE DATA RECEIVED IN A STRING
  String recv = getDataFromSerial();
  Serial.println("RECV: " + recv);

  Serial.println("recv: " + recv);
  String header = recv.substring(0, recv.indexOf(':') + 1);
  int action = messageHandler.getAction(header);
  Serial.println("Header: -" + header + "-");
  switch (action) {

    case MessageHandler::INVALID_ACTION: {
        Serial.println(messenger.STEPPER_SETUP_FAILED + recv);
        break;
      }

    case MessageHandler::UPDATE_STEPPER_PARAMETERS: {
        parseStepperSetupMessage(recv, STEP_ANGLE, DIALING_SPEED, MAX_SPEED);
        String stepperInfo = messenger.stepperMotorParametersToString(STEP_ANGLE, DIALING_SPEED, MAX_SPEED);
        Serial.println(messenger.STEPPER_SETUP_COMPLETE + stepperInfo);
        break;
      }

    case MessageHandler::ROTATE_DIAL: {
        Serial.println("rotate dial case");
        int degreesOfRotation = getDegreesOfRotationFromMessage(recv);
        rotate(degreesOfRotation);
        break;
      }

    default:
      break;
      //error
  }

}


/**
   Collects data from serial monitor up until
   a new line character is reached. Returns a
   string, not including the newline char
*/
String getDataFromSerial() {
  String data;
  while (Serial.available() > 0) {
    char ch = Serial.read();
    if (ch == '\n')
      return data;
    data += ch;
  }
}

void rotate(int degreesOfRotation) {
  setDirectionPin(degreesOfRotation);
  degreesOfRotation = abs(degreesOfRotation);
  int stepsToTake = (int)((float)degreesOfRotation / STEP_ANGLE);
  runMotor(stepsToTake, DIALING_SPEED); //rotates motor until position reached
  /*FIXME:
     - should probably leave degreesOdRotation negative, moveTo() accepts negative
     - runMotor() might require resetting zero after each run, not sure yet (testing needed)
  */
}

void runMotor(int stepsToTake, int motorSpeed) {
  stepper.moveTo(stepsToTake);
  stepper.setSpeed(motorSpeed);
  while (stepper.distanceToGo() != 0)
    stepper.runSpeedToPosition();
  stepper.setCurrentPosition(0);
}

/**
   Sets the DIR pin for either CLOCKWISE (+) or COUNTER-CLOCKWISE (-)
*/
void setDirectionPin(int degreesOfRotation) {
  if (degreesOfRotation < 0) {
    digitalWrite(dirStepper, LOW);
  }
  else {
    digitalWrite(dirStepper, HIGH);
  }
  return;
}

void blockUntilSetUpMessageIsReceived() {
  String recv = getDataFromSerial();
  if ( isSetUpMessage(recv) ) {
    parseStepperSetupMessage(recv, STEP_ANGLE, DIALING_SPEED, MAX_SPEED);
    String stepperInfo = messenger.stepperMotorParametersToString(STEP_ANGLE, DIALING_SPEED, MAX_SPEED);
    Serial.println(messenger.STEPPER_SETUP_COMPLETE + stepperInfo);
  } else {
    do {
      Serial.println(messenger.STEPPER_SETUP_FAILED + recv);
      while (Serial.available() == 0) {
        delay(200);
      }
      recv = getDataFromSerial();
    } while (!isSetUpMessage(recv));
    parseStepperSetupMessage(recv, STEP_ANGLE, DIALING_SPEED, MAX_SPEED);
    String stepperInfo = messenger.stepperMotorParametersToString(STEP_ANGLE, DIALING_SPEED, MAX_SPEED);
    Serial.println(messenger.STEPPER_SETUP_COMPLETE + stepperInfo);
  }
}

void parseStepperSetupMessage(String recv, float &stepAngle, int &dialingSpeed, int &maxspeed) {
  /* REMOVE HEADER FROM DATA */
  int headerSizeToRemove = recv.indexOf(initialMessageHeader) + initialMessageHeader.length();
  recv.remove(0, headerSizeToRemove);
  /* c_str() returns a const char, so we can't use strtok()
     on it directly. Thus, we must copy sequence.c_str() to
     another cString */
  char cstringRecv[recv.length() + 1] = {};
  strcpy(cstringRecv, recv.c_str());

  String stepAngleString(strtok(cstringRecv, delimiter));
  String dialingSpeedString(strtok(NULL, delimiter));
  String maxSpeedString(strtok(NULL, delimiter));

  stepAngle = stepAngleString.toFloat();
  dialingSpeed = dialingSpeedString.toInt();
  maxspeed = maxSpeedString.toInt();
}

int getDegreesOfRotationFromMessage(String recv) {
  /* REMOVE HEADER FROM DATA */
  int headerSizeToRemove = recv.indexOf(turnDialHeader) + turnDialHeader.length();
  recv.remove(0, headerSizeToRemove);

  /* c_str() returns a const char, so we can't use strtok()
     on it directly. Thus, we must copy sequence.c_str() to
     another cString */
  char cstringRecv[recv.length() + 1] = {};
  strcpy(cstringRecv, recv.c_str());

  String degreesOfRotationString(strtok(cstringRecv, delimiter));
  return degreesOfRotationString.toInt();
}

boolean isSetUpMessage(String recv) {
  if (recv.indexOf(initialMessageHeader) < 0)
    return false;
  return true;
}
