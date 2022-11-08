#include <Arduino.h>

class MessageHandler{
  public:
    MessageHandler();
    MessageHandler(String setUpHeader, String turnDialHeader, String setStepBitsHeader);


    //FLAGS
    const static int INVALID_ACTION = -1;
    const static int INITIAL_SETUP = 0;
    const static int ROTATE_DIAL = 1;
    const static int UPDATE_STEP_SIZE = 2;
    const static int UPDATE_DIALING_SPEED = 3;

    //PROGRAM ACTION HEADERS
    String setUpHeader = "";
    String turnDialHeader = "";
    String setStepBitsHeader = "";
    String setDialingSpeedHeader = "";
    
    int getAction(String header);
    void setInitialMessageHeader(String header);
    void setTurnDialHeader(String header);
    void setSetStepBitsHeader(String header);
    void setSetDialingSpeedHeader(String header);
};
