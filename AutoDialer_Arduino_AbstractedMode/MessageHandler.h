#include <Arduino.h>

class MessageHandler{
  public:
    MessageHandler(String setUpHeader, String turnDialHeader, String setStepBitsHeader);


    //FLAGS
    const static int INVALID_ACTION = -1;
    const static int UPDATE_STEPPER_PARAMETERS = 0;
    const static int ROTATE_DIAL = 1;
    const static int UPDATE_STEP_SIZE = 2;

    //PROGRAM ACTION HEADERS
    String setUpHeader = "";
    String turnDialHeader = "";
    String setStepBitsHeader = "";
    
    int getAction(String header);
};
