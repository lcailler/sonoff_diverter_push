
// https://github.com/adlerweb/ESP8266Webserver-Tutorial/blob/master/9%20-%20WebServer%20-%20AJAX_JSON/9.cpp

#include "main_loop.h"
#include "data_logger.h"
#include "relayjsonphp.h"
#include "webserver.h"
#include <time.h>


void httpNotFound() {
  server.send(404, F("text/plain"), F("404: Not found"));
}

void httpConfigJson() {
  char buff[64]="";
  
  String output = F("{\"sw_version\":\"");
  output += F(VERSION);
  output += "\"";

  output += ",\"localHostName\":";
  sprintf(buff,"\"%s\"", Get_local_host_name());
  output += buff;

  output += F("}");

  server.send(200, F("application/json"), output);
}
void httpTime() {
  String output = F("I am running for ");
  output += String(millis());
  output += " milliseconds.";
  server.send(200, "text/plain", output);
}

char *getLocalTime(char *buff, int buffSize){
/*  time_t rawtime;
  struct tm * timeinfo;
    
  rawtime=time(nullptr);
  timeinfo = localtime (&rawtime);
  strftime (buff,buffSize,"%H:%M:%S",timeinfo);
*/
  return getLocalStrftime(buff,buffSize,"%H:%M:%S");
}

char *getLocalDate(char *buff, int buffSize){
  /*
  time_t rawtime;
  struct tm * timeinfo;
    
  rawtime=time(nullptr);
  timeinfo = localtime (&rawtime);
  strftime (buff,buffSize,"%Y%m%d",timeinfo);
  return buff;
  */
  return getLocalStrftime(buff,buffSize,"%Y%m%d");

}

void httpTimeJson() {
  char time_hhmmss[9];
  String output = "{ \"uptime_ms\":";
  output += String(millis());

  output += ",\"time_sec\":";
  output += String(time(nullptr));

  getLocalTime(time_hhmmss, sizeof(time_hhmmss));
  output += ",\"time_hhmmss\":\"";
  output += time_hhmmss;
  output += "\"";

  getLocalDate(time_hhmmss, sizeof(time_hhmmss));
  output += ",\"date_yyyymmdd\":\"";
  output += time_hhmmss;
  output += "\"";

  output += " }";
  server.send(200, "application/json", output);
}

void httpGetPrg() {
  String output ="";

  output += F("{\"prg\":\"");
  output += prgStr;
  output.replace("\n","\\n");
  output += F("\"}");
  server.send(200, F("application/json"), output);
}


void httpPostPrg() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
    return;
  }

  httpPrg();
}


void httpPrg() {

  for (uint8_t i = 0; i < server.args(); i++) {

    if (String(server.argName(i)) == String( F("prg") ) )  {
        saveStringAsPrgStrFromLittleFs( server.arg(i) );
    }
    
  }

  httpGetPrg();
  
}

void httpPostRelayJsonPhp() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
    return;
  }

  httpRelayJsonPhp();
}


void httpRelayJsonPhp() {
  char buff[64];
  String output ="";

  for (uint8_t i = 0; i < server.args(); i++) {

    if (    (String(server.argName(i)) == String( F("set_relay_switch_state") ))
          ||(String(server.argName(i)) == String( F("set_relay_switch_state_channel_00") )) ) {
        set_relay_channel_active_number(0);
        set_relay_switch_state(server.arg(i));
    }
    else if (String(server.argName(i)) == String( F("set_relay_switch_state_channel_01")  )) {
      
      if (get_relay_channel_active_number()==1){
        set_relay_switch_state(server.arg(i));
      }
    }
    else if (String(server.argName(i)) == String( F("set_relay_channel_active_number")   )) {

      if        (String(server.arg(i)) == String( F("0") )) {
        set_relay_channel_active_number(0);
      } else if (String(server.arg(i)) == String( F("1") )) {
        set_relay_channel_active_number(1);
      }
    }
    
  }


  output += F("{\"uptime_ms\":");
  output += String(millis());
  
  output += F(",\"relay_channel_active_number\":");
  sprintf(buff,"%d", get_relay_channel_active_number());
  output += buff;

  output += F(",\"relay_switch_state\":");
  if (digitalRead(GPIO_12_RELAY)) {output += "1";} else {output += "0";}

  output += F("}");
  server.send(200, F("application/json"), output);
  
}



void httpPostSwitchState() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
    return;
  }

  httpDeviceJson();
}

void httpDeviceJson() {
  char buff[64];
  String output ="";

  for (uint8_t i = 0; i < server.args(); i++) {

    if (String(server.argName(i)) == String("switchOFF")) {
      digitalWrite(GPIO_12_RELAY, LOW);   // Turn the relay off
    }
    else if (String(server.argName(i)) == String("switchON")) {
      digitalWrite(GPIO_12_RELAY, HIGH);   // Turn the relay on
    }
    else if (String(server.argName(i)) == String("switchTOGGLE")) {
      if (digitalRead(GPIO_12_RELAY)==0) {
        digitalWrite(GPIO_12_RELAY, HIGH);   // Turn the relay on
      } else{
        digitalWrite(GPIO_12_RELAY, LOW);   // Turn the relay off
      }
    }
    else if (String(server.argName(i)) == String("switch")) {
      
      if (String(server.arg(i)) == String("1")) {
        digitalWrite(GPIO_12_RELAY, HIGH);   // Turn the relay on
      } else if (String(server.arg(i)) == String("0")) {
        digitalWrite(GPIO_12_RELAY, LOW);   // Turn the relay on
      } else if (String(server.arg(i)) == String("2")) {
        if (digitalRead(GPIO_12_RELAY)==0) {
          digitalWrite(GPIO_12_RELAY, HIGH);   // Turn the relay on
        } else{
          digitalWrite(GPIO_12_RELAY, LOW);   // Turn the relay off
        }
      }
    }
    else if (String(server.argName(i)) == String("localHostName")) {
      //strcpy(local_host_name,server.arg(i));
      server.arg(i).toCharArray(local_host_name, sizeof(local_host_name)-1);
      SERIALPRINT(  "local_host_name:");
      SERIALPRINTLN( local_host_name  );
      // saveLocalHostName2Eeprom(local_host_name);
      // saveLocalHostName2LittleFs(local_host_name);
      saveConfig2LittleFs(httpServerPushUrl, local_host_name);
    }
    else if (String(server.argName(i)) == String("httpServerPushUrl")) {
      //strcpy(local_host_name,server.arg(i));
      server.arg(i).toCharArray(httpServerPushUrl, sizeof(httpServerPushUrl)-1);
      SERIALPRINT(  "httpServerPushUrl:");
      SERIALPRINTLN( httpServerPushUrl  );
      // saveLocalHostName2Eeprom(local_host_name);
      // saveLocalHostName2LittleFs(local_host_name);
      saveConfig2LittleFs(httpServerPushUrl, local_host_name);
    }
    
  }


  output += "{\"uptime_ms\":";
  output += String(millis());
  
  output += ",\"switch\":";
  if (digitalRead(GPIO_12_RELAY)) {output += "1";} else {output += "0";}

  output += F(",\"relay_channel_active_number\":");
  sprintf(buff,"%d", get_relay_channel_active_number());
  output += buff;

  output += F(",\"relay_switch_state\":");
  if (digitalRead(GPIO_12_RELAY)) {output += "1";} else {output += "0";}

  output += ",\"deviceid\":\"";
  sprintf(buff,"%010u", ESP.getFlashChipId());
  output += buff;
  output += "\"";

  output += ",\"signalStrength\":";
  sprintf(buff,"%d", WiFi.RSSI());
  output += buff;

  output += ",\"localHostName\":";
  sprintf(buff,"\"%s\"", Get_local_host_name());
  output += buff;

  output += ",\"httpServerPushUrl\":";
  sprintf(buff,"\"%s\"", Get_httpServerPushUrl());
  output += buff;

  output += "}";
  server.send(200, "application/json", output);
}

void listDir(const char * dirname) {
  Serial.printf("Listing directory: %s\n", dirname);

  Dir root = LittleFS.openDir(dirname);

  while (root.next()) {
    File file = root.openFile("r");
    Serial.print("  FILE: ");
    Serial.print(root.fileName());
    Serial.print("  SIZE: ");
    Serial.print(file.size());
    time_t cr = file.getCreationTime();
    time_t lw = file.getLastWrite();
    file.close();
    struct tm * tmstruct = localtime(&cr);
    Serial.printf("    CREATION: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
    tmstruct = localtime(&lw);
    Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
  }
  FSInfo fs_info;
  LittleFS.info(fs_info);
  Serial.println(F("LittleFS infos:"));
  Serial.print(F("totalBytes:"));
  Serial.println(fs_info.totalBytes);
  Serial.print(F("usedBytes:"));
  Serial.println(fs_info.usedBytes);
  Serial.print(F("blockSize:"));
  Serial.println(fs_info.blockSize);
  Serial.print(F("pageSize:"));
  Serial.println(fs_info.pageSize);
  Serial.println(F(""));
  Serial.print(F("size of one element of DataLogger:"));
  Serial.println(sizeof(DataLogger_GetLastDataValues()));
  
}


void httpSerial() {
  String message = F("");
  serialSetTxMsgAvailable(false);

  clearReceivedMessage();


  for (uint8_t i = 0; i < server.args(); i++) {
    message += String(F(" ")) + server.argName(i) + F(": ") + server.arg(i) + F("\n");
    

    /* sonoff code */
    if (String(server.argName(i)) == String("RON")) {
      digitalWrite(GPIO_12_RELAY, HIGH);   // Turn the relay off
    }
    else if (String(server.argName(i)) == String("ROFF")) {
      digitalWrite(GPIO_12_RELAY, LOW);   // Turn the relay on
    }
    else {
      //Serial.println(server.argName(i));
      serialSendStringMsg(server.argName(i));

    }
    
  }

  long t=millis()+1000;
  while (millis()<t){
    if (Serial.available() > 0){
      if (mngReceivedMessagesComplete()){
        break;
      }
    }
  }

  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");

  //delay(500);
  message += String(F("\ninputString: "))+getReceivedMessage();

  server.send(200, "text/plain", message);
}

void httpSerialJson() {
  String message = "";

  clearReceivedMessage();


  for (uint8_t i = 0; i < server.args(); i++) {
    // message += String(F(" ")) + server.argName(i) + F(": ") + server.arg(i) + F("\n");
    

    /* sonoff code */
    if (String(server.argName(i)) == String("RON")) {
      digitalWrite(GPIO_12_RELAY, HIGH);   // Turn the relay off
    }
    else if (String(server.argName(i)) == String("ROFF")) {
      digitalWrite(GPIO_12_RELAY, LOW);   // Turn the relay on
    }
    else {
      Serial.println(server.argName(i)); 
    }
    
  }

  long t=millis()+1000;
  while (millis()<t){
    if (Serial.available() > 0){
      if (mngReceivedMessagesComplete()){
        break;
      }
    }
  }

  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");

  //delay(500);
  if (getReceivedMessageSize()>0){
    message = getReceivedMessage();
  } else {
    message = String(F("{}"));
  }

  server.send(200, "text/plain", message);
}



void httpSerialBuffJson() {
  // String message = "";



  for (uint8_t i = 0; i < server.args(); i++) {
    

    /* sonoff code */
    if (String(server.argName(i)) == String("RON")) {
      digitalWrite(GPIO_12_RELAY, HIGH);   // Turn the relay off
    }
    else if (String(server.argName(i)) == String("ROFF")) {
      digitalWrite(GPIO_12_RELAY, LOW);   // Turn the relay on
    }
    else {
      Serial.println(server.argName(i)); 
    }
    
  }

  long t=millis()+1000;
  while (millis()<t){
    if (Serial.available() > 0){
      if (getReceivedMessageBuffSize()>0){
        break;
      }
    }
  }

  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");

  //delay(500);
  if (getReceivedMessageBuffSize()>0){
    //message = getReceivedMessageBuff();
    server.send(200, "text/plain", getReceivedMessageBuff() );
  } else {
    //message = String(F("{}"));
    server.send(200, "text/plain", String(F("{}")) );
  }

  
}


void httpTest() {
  String output = "ABCDEFGHIJK"; // 11c
  String s      = "abcdefghij"; // 10c
  #define MAX_SIZE 11
  int n=s.length();
  output=output.substring(n)+s;
  
  server.send(200, "text/plain", output);
}



void httpDataJson() {
  String output;
  output = info2json(output, &DataLogger_GetLastDataValues());
  server.send(200, "application/json", output);
}



void httpDataLogJson00() {
  // init phase
  // https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WebServer/examples/FSBrowser/FSBrowser.ino
  // use HTTP/1.1 Chunked response to avoid building a huge temporary string
  //https://gist.github.com/spacehuhn/6c89594ad0edbdb0aad60541b72b2388
  server.sendHeader("Content-Length", (String)(LOG_SIZE*195+2));  // 88500+2=300*195+2=LOG_SIZE*195+2
  server.send(200, "application/json", "");
  
  String output;

  output  = F("{\n");
  //output += info2json2ElementLog(output, &DataLogger_GetLastDataValues());
  server.sendContent(output);
  int n=LOG_SIZE; //debug

//taille totale :88500+2
  while (n>0){ // debug
    for (int m=0; m<LOG_SIZE; m++){
      //Serial.println(m);//debug
  
//      if (DataLogger_GetLogDataValues(m).uptime!=(unsigned long)0){
        //output = ",\n";
        //output += info2json2ElementLog(output, &(DataLogger_GetLogDataValues(m)));
        // 294+1
        output = "\"1601727918\":{\"uptime_ms\":2241341,\"time_sec\":1601727918,\"InputPilotState\":\"A\",\"OutputPilotState\":\"@\",\"output_pilot_voltage_min_v\":2.00,\"output_pilot_voltage_max_v\":2.02,\"input_pilot_voltage_min_v\":0.62,\"input_pilot_voltage_max_v\":0.62,\"power_table_index\":0,\"power_table_index_sp\":0,\"ErrCtr\":0},\n";
        server.sendContent(output);
        
        n-=1; // debug
//      }

    } // end for
  } // debug
  //server.sendContent(F("\n}"));
  
  //server.sendContent_P(buf, len); //send the current buffer

  // end of phase
  //server.chunkedResponseFinalize();

}

void httpDataLogJson() {
  // init phase
  // https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WebServer/examples/FSBrowser/FSBrowser.ino
  // use HTTP/1.1 Chunked response to avoid building a huge temporary string
  if (DataLogger_GetLastDataValues().uptime==0){
    /* no log data available */
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "-1");
    server.send(200, F("application/json"), F("{}") );
    return;
    
  }
  if (!server.chunkedResponseModeStart(200, "application/json")) {
    server.send(505, F("application/json"), F("HTTP1.1 required"));
    return;
  }
  
  String output;

  output  = F("{\n");
  output += info2json2ElementLog(output, &DataLogger_GetLastDataValues());
  server.sendContent(output);
  int n=LOG_SIZE; //debug

//taille totale :88502
  while (n>0){ // debug
    for (int m=0; m<LOG_SIZE; m++){
  
      if (DataLogger_GetLogDataValues(m).uptime!=(unsigned long)0){
        output = ",\n";
        output += info2json2ElementLog(output, &(DataLogger_GetLogDataValues(m)));
        // 294+1
        server.sendContent(output);
        
        n-=1; // debug
      }

    } // end for
  } // debug
  server.sendContent(F("\n}"));
  

  // end of phase
  server.chunkedResponseFinalize();

}


void webserver_setup() {
  server.onNotFound(httpNotFound);
  
  server.on("/config.json", httpConfigJson);
  server.on("/time",      httpTime);
  server.on("/time.json", httpTimeJson);
  server.on("/device.json", httpDeviceJson);
  server.on("/postswitchstate", httpPostSwitchState);
  
  server.on("/serial",          httpSerial);
  server.on("/serial.json",     httpSerialJson);
  server.on("/serialbuff.json", httpSerialBuffJson);
  server.on("/data.json",       httpDataJson);
  server.on("/datalog.json",    httpDataLogJson);

  server.on("/relay.json.php",     httpRelayJsonPhp);
  server.on("/postrelay.json.php", httpPostRelayJsonPhp);

  server.on("/set_prg.json.php",  httpPrg);
  server.on("/post_prg.json.php", httpPostPrg);
  server.on("/get_prg.json.php",  httpGetPrg);
  

  
  
  server.on("/test.json", httpTest);

  if (true){ //(LittleFS.begin()) {
    listDir("/");
    server.serveStatic("/",           LittleFS, "/index.html");
    server.serveStatic("/time.html",  LittleFS, "/time.html");
    //server.serveStatic("/datalog.html",  LittleFS, "/datalog.html");

    server.serveStatic("/test_relais.html",  LittleFS, "/test_relais.html");
    server.serveStatic("/test_relais.js",    LittleFS, "/test_relais.js");
    server.serveStatic("/images/switched_auto.png",            LittleFS, "/images/switched_auto.png");
    server.serveStatic("/images/switched_man.png",             LittleFS, "/images/switched_man.png");
    server.serveStatic("/images/switched_off_auto.png",        LittleFS, "/images/switched_off_auto.png");
    server.serveStatic("/images/switched_off.png",             LittleFS, "/images/switched_off.png");
    server.serveStatic("/images/switched_on_auto.png",         LittleFS, "/images/switched_on_auto.png");
    server.serveStatic("/images/switched_on.png",              LittleFS, "/images/switched_on.png");
    server.serveStatic("/images/switched_unknown_status.png",  LittleFS, "/images/switched_unknown_status.png");
    Serial.println("serveStatic init done");
  }

  server.begin();

}
