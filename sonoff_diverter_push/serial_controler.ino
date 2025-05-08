/*
 * remaining mem
 * v      LOG_SIZE of log_power
 * v      v
 * 51460  0
 * 51060  100
 * 47460  1000
 * 28420  5760  long int float - correspond a 24h 4 valeurs par minute 24*60*4 
 * 5412   5760  double /!\
 * 11460  10000 long   /!\ La mémoire disponible faible, des problèmes de stabilité pourraient survenir.
 * Le croquis utilise 378920 octets (39%) de l'espace de stockage de programmes. Le maximum est de 958448 octets.
 * Les variables globales utilisent 30460 octets (37%) de mémoire dynamique, ce qui laisse 51460 octets pour les variables locales. Le maximum est de 81920 octets.
 */

#include "serial_controler.h"
#include <Ticker.h>
Ticker ticker_timeout;
//#define serialSetTxMsgAvailable(x)   {serialTxMsgAvailableState=x;}
//#define serialGetTxMsgAvailable()   (serialTxMsgAvailableState)

void setupSerial(){
  serialSetTxMsgAvailable(true);
  inputString.reserve(INPUT_STRING_SIZE);
  inputStringBuffFifo.reserve(INPUT_STRING_BUFF_SIZE);
  initLog();
}

void tick_timeout()
{
  serialSetTxMsgAvailable(true);
}


void serialSendMsg(char *msg){
  ticker_timeout.once(1.0, tick_timeout);
  while (!serialGetTxMsgAvailable());
  serialSetTxMsgAvailable(false);
  Serial.println(msg);

}
void serialSendMsgAndLog(char *msg){
  DataLogger_logNextRcvMsg();
  serialSendMsg( msg);
}


void serialSendStringMsg(String msg){
  ticker_timeout.once(1.0, tick_timeout);
  while (!serialGetTxMsgAvailable());
  serialSetTxMsgAvailable(false);
  Serial.println(msg);
}
void serialSendStringMsgAndLog(String msg){
  DataLogger_logNextRcvMsg();
  serialSendStringMsg( msg);
}



String getReceivedMessage(){
  return lastInputString;
}



String getReceivedMessageBuff(){
  //String s=String(inputStringBuffFifo);
  //clearReceivedMessageBuff();
  //clearReceivedMessage();
  /* read protection needed ? -> no */
  return inputStringBuffFifo;
}


boolean mngReceivedMessagesComplete(){
//  inputString=Serial.read();
//  Serial.println(inputString);
//}
//void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    //Serial.println(inChar, HEX);
    // add it to the inputString:
    // inputString += inChar;
    if (inChar == '\n') {
      parseLineInputString(inputString);
      
      SERIALPRINT(">>>");
      SERIALPRINTLN(inputString);

      //int n=inputStringBuffFifo.length()+inputString.length()+1;
      int n=inputString.length()+1;
      if (n+inputStringBuffFifo.length()<=INPUT_STRING_BUFF_SIZE){
        inputStringBuffFifo+=String("\n")+inputString;
      }else{
        inputStringBuffFifo =inputStringBuffFifo.substring(n)+String("\n")+inputString;
        n=inputStringBuffFifo.indexOf("\n")+1;
        inputStringBuffFifo =inputStringBuffFifo.substring(n);
      }
      lastInputString=inputString;
      
      inputString="";
      stringComplete = true;
      serialSetTxMsgAvailable(true);

    } else {
      if (inputString.length()<INPUT_STRING_SIZE){
        if ((inChar>31) && (inChar<127)) {
          if (stringComplete){
            inputString = inChar;
            stringComplete = false;
          } else {
            inputString += inChar;
          }
        } else;
        
      } else;
    }
  }
  return stringComplete;
}
