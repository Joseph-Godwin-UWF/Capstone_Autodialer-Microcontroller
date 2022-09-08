#include <AccelStepper.h>

#define pwmStepper 6
#define dirStepper 40


/* SETTING UP STEPPER MOTOR */
 const float STEP_ANGLE = 1.8;
 const float STEPS_PER_REV = 360 / STEP_ANGLE;
 const int DIALING_SPEED = 400;
 const int MAX_SPEED = 800;
 AccelStepper stepper(AccelStepper::DRIVER, pwmStepper, dirStepper);
 
void setup() {
  Serial.begin(9600);
   stepper.setMaxSpeed(MAX_SPEED);
   stepper.setSpeed(200);
}

/**
 * FUNCTION DELCARATIONS
 */

void loop() {
  //WAIT FOR INITIAL COMMUNICATION
  while(Serial.available() == 0) { delay(200); }

  //STORE DATA RECEIVED IN A STRING
  String recv = getDataFromSerial();

  Serial.print("Data Received: ");
  Serial.println(recv);
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
