#ifndef serial_controler_h
#define serial_controler_h


#define INPUT_STRING_BUFF_SIZE (1600*2+1)  // 1482*2+1 // 801 = 80*10+1
String        inputStringBuffFifo = "";    // a string buffered to hold incoming data
#define INPUT_STRING_SIZE 1600             // 1482 // 80
String        inputString = "";        // a string to hold incoming data
boolean       stringComplete = false;  // whether the string is complete
String        lastInputString = "";    // a string to hold incoming data
boolean       serialTxMsgAvailableState=true;


void setupSerial();
void serialSendMsg(char *msg);
void serialSendMsgAndLog(char *msg);
void serialSendStringMsg(String msg);
void serialSendStringMsgAndLog(String msg);

#define serialSetTxMsgAvailable(x)   {serialTxMsgAvailableState=x;}
#define serialGetTxMsgAvailable()   (serialTxMsgAvailableState)
#define getReceivedMessageSize() (lastInputString.length())
#define clearReceivedMessage()   {stringComplete=false;inputString = "";}



#define getReceivedMessageBuffSize() (inputStringBuffFifo.length())
#define clearReceivedMessageBuff()   {inputStringBuffFifo=String("");}


boolean mngReceivedMessagesComplete();

#endif // serial_controler_h
