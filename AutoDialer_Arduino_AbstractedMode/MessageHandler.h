#include <Arduino.h>

class MessageHandler{
  public:
    MessageHandler(String setUpHeader, String turnDialHeader);


    //FLAGS
    const static int INVALID_ACTION = -1;
    const static int UPDATE_STEPPER_PARAMETERS = 0;
    const static int ROTATE_DIAL = 1;

    //PROGRAM ACTION HEADERS
    String setUpHeader = "";
    String turnDialHeader = "";
    
    int getAction(String header);
};
