#include <AccelStepper.h>
#include "HX711.h"
#include "Messenger.h"
#include "MessageHandler.h"

TaskHandle_t Task1;
SemaphoreHandle_t calibratingMutex;

#define pwmStepper 2
#define dirStepper 15
#define ms1pin 19
#define ms2pin 18
#define ms3pin 5
#define stepperEnable 16

#define button 4

#define LOADCELL_DATA 26
#define LOADCELL_CLOCK 27
#define ACCELERATION 16000


/* SETTING UP STEPPER MOTOR */
float STEP_ANGLE = 1.8;
float STEPS_PER_REV = 360 / STEP_ANGLE;
int DIALING_SPEED = 200;

String initialMessageHeader =  "000:";
String setStepSizeHeader =     "001:";
String setDialingSpeedHeader = "002:";
String turnDialHeader =        "003:";
String calibrateTorqueThresholdHeader = "004:";

/* TORQUE THRESHOLD */
boolean calibrating = false;
boolean calibrated = false;
long TORQUE_THRESHOLD = 999999;
long TORQ_THRESH_BUFFER = 10000;
//double MIN_TORQUE_MULTIPLIER = 0.2; /* HOW MUCH TO MULTIPLY MINIMUM READ TORQUE FOR CALCULATING TORQUE_THRESHOLD */

const char* delimiter = ";";
//STEPSIZE SELECTION PINS
int ms1 = 0;
int ms2 = 0;
int ms3 = 0;
AccelStepper stepper(AccelStepper::DRIVER, pwmStepper, dirStepper);
HX711 torqueTransducer;

Messenger messenger;
MessageHandler messageHandler;

void setup() { 
  calibratingMutex = xSemaphoreCreateMutex();
  stepper.setAcceleration(ACCELERATION);
  messageHandler.setInitialMessageHeader(initialMessageHeader);
  messageHandler.setSetStepBitsHeader(setStepSizeHeader);
  messageHandler.setSetDialingSpeedHeader(setDialingSpeedHeader);
  messageHandler.setTurnDialHeader(turnDialHeader);
  messageHandler.setCalibrateTorqueThresholdHeader(calibrateTorqueThresholdHeader);

  pinMode(button, INPUT);
  pinMode(ms1pin, OUTPUT);
  pinMode(ms2pin, OUTPUT);
  pinMode(ms3pin, OUTPUT);
  pinMode(stepperEnable, OUTPUT);
  Serial.begin(115200);
  torqueTransducer.begin(LOADCELL_DATA, LOADCELL_CLOCK);
  //WAIT FOR INITIAL COMMUNICATION
  while (Serial.available() == 0) {
    delay(200);
  }
  blockUntilSetUpMessageIsReceived();
  stepper.setMaxSpeed(DIALING_SPEED);
  stepper.setSpeed(DIALING_SPEED);
  digitalWrite(stepperEnable, LOW);

  xTaskCreatePinnedToCore(
                    Task1code,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
  delay(500);  
}

void Task1code( void * pvParameters ){
  for(;;){
    if(torqueTransducer.is_ready()) {
      long reading = torqueTransducer.read();
      
      xSemaphoreTake( calibratingMutex, portMAX_DELAY );
      if(calibrating && (reading < TORQUE_THRESHOLD)){
        Serial.println("Calibrating");
        TORQUE_THRESHOLD = reading;
      }
      xSemaphoreGive( calibratingMutex );
      
      if(reading < ( TORQUE_THRESHOLD - TORQ_THRESH_BUFFER) && calibrated){
        Serial.println(messenger.THRESHOLD_TORQUE_REACHED);
        Serial.print(TORQUE_THRESHOLD - TORQ_THRESH_BUFFER);
        Serial.print(" > ");
        Serial.println(reading);
        digitalWrite(stepperEnable, HIGH);
      }
      else{
        //Serial.print(messenger.TORQUE_READING);
        //Serial.println(reading);
      }
    }
    vTaskDelay(50);
  } 
}

/**
   FUNCTION DELCARATIONS
*/

void loop() {
  //WAIT FOR MESSAGE COMMUNICATION
  //String error = getDataFromSerial();
  //Serial.println("Data still on line: " + error);
  while (Serial.available() == 0 || digitalRead(button) == HIGH) {
    delay(200);
  }
  //STORE DATA RECEIVED IN A STRING
  String recv = getDataFromSerial();

  String header = recv.substring(0, recv.indexOf(':') + 1);
  int action = messageHandler.getAction(header);
  switch (action) {

    case MessageHandler::INVALID_ACTION: {
        Serial.println(messenger.INVALID_SETUP_MESSAGE);
        break;
      }

    case MessageHandler::INITIAL_SETUP: {
        Serial.println(messenger.INITIAL_SETUP_COMPLETE);
        break;
      }

    case MessageHandler::ROTATE_DIAL: {
        int ticksToRotate = getTicksToRotateFromMessage(turnDialHeader, recv);
        rotate(ticksToRotate);
        Serial.println(messenger.REQUEST_NEXT_TURN);
        break;
      }

    case MessageHandler::UPDATE_STEP_SIZE: {
        parseSetStepperBitsMessage(recv, ms1, ms2, ms3);
        digitalWrite(ms1pin, ms1);
        digitalWrite(ms2pin, ms2);
        digitalWrite(ms3pin, ms3);
        //Serial.println(messenger.STEP_RESOLUTION_SET);
        break;
    }

    case MessageHandler::UPDATE_DIALING_SPEED: {
      parseSetDialingSpeedMessage(recv, DIALING_SPEED);
      stepper.setMaxSpeed(DIALING_SPEED);
      //stepper.setSpeed(DIALING_SPEED); /* removed for acceleration */
      //Serial.println(messenger.DIALING_SPEED_SET);
      break;
    }

    case MessageHandler::CALIBRATE_TORQUE_THRESHOLD: {
      Serial.print("Before calibration: ");
      Serial.println(TORQUE_THRESHOLD);
      int ticksToRotate = getTicksToRotateFromMessage(calibrateTorqueThresholdHeader, recv);
      stepper.moveTo(ticksToRotate);
      xSemaphoreTake( calibratingMutex, portMAX_DELAY );
      calibrating = true;
      xSemaphoreGive( calibratingMutex );
      while (stepper.distanceToGo() != 0){
        stepper.run();
      }
      xSemaphoreTake( calibratingMutex, portMAX_DELAY );
      calibrating = false;
      calibrated = true;
      xSemaphoreGive( calibratingMutex );
      Serial.println("Finished Calibrating");
      delay(50);
      Serial.print("After calibration: ");
      Serial.println(TORQUE_THRESHOLD);
      stepper.setCurrentPosition(0);
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

void rotate(int ticksToRotate) {
  runMotor(ticksToRotate, DIALING_SPEED); //rotates motor until position reached
}

void runMotor(int stepsToTake, int motorSpeed) {
  stepper.moveTo(stepsToTake);
  //stepper.setSpeed(motorSpeed); /* removed for acceleration */
  while (stepper.distanceToGo() != 0){
    //stepper.runSpeedToPosition(); /* removed for acceleration */
    stepper.run();
  }
  stepper.setCurrentPosition(0);
}

void blockUntilSetUpMessageIsReceived() {
  String recv = getDataFromSerial();
  if ( isSetUpMessage(recv) ) {
    Serial.println(messenger.INITIAL_SETUP_COMPLETE);
  } else {
    do {
      Serial.println(messenger.INVALID_SETUP_MESSAGE);
      while (Serial.available() == 0) {
        delay(200);
      }
      recv = getDataFromSerial();
    } while (!isSetUpMessage(recv));
    Serial.println(messenger.INITIAL_SETUP_COMPLETE);
  }
}

void parseSetStepperBitsMessage(String recv, int &ms1, int &ms2, int &ms3){
  /* REMOVE HEADER FROM DATA */
  int headerSizeToRemove = recv.indexOf(setStepSizeHeader) + setStepSizeHeader.length();
  recv.remove(0, headerSizeToRemove);
  ms1 = recv[0] - 48;
  ms2 = recv[1] - 48;
  ms3 = recv[2] - 48;  
}

void parseSetDialingSpeedMessage(String recv, int &dialingSpeed){
  /* REMOVE HEADER FROM DATA */
  int headerSizeToRemove = recv.indexOf(setDialingSpeedHeader) + setDialingSpeedHeader.length();
  recv.remove(0, headerSizeToRemove);
  dialingSpeed = recv.toInt();
}

//FIXME: THIS IS DEPRECATED
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

int getTicksToRotateFromMessage(String header, String recv) {
  /* REMOVE HEADER FROM DATA */
  int headerSizeToRemove = recv.indexOf(header) + turnDialHeader.length();
  recv.remove(0, headerSizeToRemove);

  /* c_str() returns a const char, so we can't use strtok()
     on it directly. Thus, we must copy sequence.c_str() to
     another cString */
  char cstringRecv[recv.length() + 1] = {};
  strcpy(cstringRecv, recv.c_str());

  String ticksToRotateString(strtok(cstringRecv, delimiter));
  return ticksToRotateString.toInt();
}

boolean isSetUpMessage(String recv) {
  if (recv.indexOf(initialMessageHeader) < 0)
    return false;
  return true;
}
