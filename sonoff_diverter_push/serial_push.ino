
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include <string.h>
#include <time.h>
#include "serial_controler.h"
#include "serial_push.h"

char* serial_push2url(char *url, char *resp, int resp_len){
  String message = "";
  time_t rawtime;
  struct tm *info;
  char buffer[17]; /* "20210110T140018":{"tria */

  if (mngReceivedMessagesComplete()){
    SERIALPRINTLN("complete");
  } else {
    SERIALPRINTLN("not complete");
  }

  unsigned long t=millis();
  while (millis()-t<(unsigned long)1000*5){
    //SERIALPRINTLN(millis()-t);
    if (Serial.available() > 0){
      if (mngReceivedMessagesComplete()){
        t=0;
        SERIALPRINTLN("break");
        break;
      }
    }
  }

  if (t!=0){
    return resp;
  }

  //time( &rawtime );
  rawtime=time(nullptr);
  //rawtime+=(unsigned long)3600*3;
  info = localtime( &rawtime );

  strftime(buffer,16,"%Y%m%dT%H%M%S", info);
  message += F("\"");
  message += buffer;
  message += F("\":{\"time_sec\":");
  message += String(time(nullptr));
  message += F(",\"log_date\":\"");
  message += buffer;
  message += F("\",");


  if (getReceivedMessageSize()>0){
    message += getReceivedMessage().substring(1);
    message += F(",");
  } else {
    buffer[0]=0;
  }

  //SERIALPRINTLN(message);
  //SERIALPRINTLN(url);
  SERIALPRINTLN(getReceivedMessageSize());
  SERIALPRINTLN("1111");
  
  if ((buffer[0]!=0)&&(WiFi.status()== WL_CONNECTED)) {

    WiFiClient wfClient;
    HTTPClient http;
     
    http.begin(wfClient,url);  
    http.addHeader("Content-Type", "application/json");         
     
     
    int httpResponseCode = http.POST(message);
    /* int httpResponseCode = http.GET(); */
 
    if (httpResponseCode>0){
       
      message = http.getString();

       
      SERIALPRINTLN(httpResponseCode);   
      SERIALPRINTLN(message);
      SERIALPRINTLN("2222");
     
    } else {
      message=String("");

      SERIALPRINTLN("Error occurred while sending HTTP POST\n");
      //Serial.printf("Error occurred while sending HTTP POST: %d:%s\n", httpResponseCode, http.errorToString(httpResponseCode).c_str());
       
    }

    strncpy(resp, message.c_str(), resp_len);
    resp[resp_len]=0;
  }

  return resp;
}
