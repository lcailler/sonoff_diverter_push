#include <LittleFS.h>
#include <EEPROM.h>

#include "main_loop.h"
#include "credentials.h"



void loadPrgStrFromLittleFs(){
  // https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html
  File file = LittleFS.open("/prg.cfg", "r");
  prgStr=String("");
  if (file) {
      while (file.available()) {
        prgStr+=file.readStringUntil('\n');
        prgStr+="\n";
      }
  } else {
      Serial.println(F("Problem on reading prg.cfg file!"));
  }
  Serial.print(F("loaded prgStr String :"));
  Serial.println(prgStr);
}

void savePrgStrFromLittleFs(){
  // https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html
  File file = LittleFS.open("/prg.cfg", "w");
  if (file) {
      if (!prgStr.endsWith("\n")){
        prgStr+="\n";
      }
      file.print(prgStr);
  } else {
      Serial.println(F("Problem on writing prg.cfg file!"));
  }
 
}

void saveStringAsPrgStrFromLittleFs(String s){
  if (s!=prgStr){
    prgStr=s;
    savePrgStrFromLittleFs();
  }
 
}

void loadConfigFromLittleFs(){
  #define CONFIG_CFG_FILE_PATH "/config.cfg"
  // https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html
  File file = LittleFS.open(CONFIG_CFG_FILE_PATH, "r");
  if (file) {
      char s[255];
      int bytesQty=0;
      while (file.available()) {
        //bytesQty = file.readBytesUntil('=',local_host_name,sizeof(local_host_name)-1);
        bytesQty = file.readBytesUntil('=',s,sizeof(s)-1);
        s[bytesQty]=0;
        if (String(s) == String("local_host_name")) {
          bytesQty = file.readBytesUntil('\n',local_host_name,sizeof(local_host_name)-1);
          local_host_name[bytesQty]=0;
        }
        else if (String(s) == String("httpServerPushUrl")) {
          bytesQty = file.readBytesUntil('\n',httpServerPushUrl,sizeof(httpServerPushUrl)-1);
          httpServerPushUrl[bytesQty]=0;
        } else {
          file.readBytesUntil('\n',s,sizeof(s)-1);
        }
        
      }
      file.close();
      //Serial.write(file.read());
      //fscanf(file,"local_host_name=%s\n",local_host_name);
      //Serial.write(sprintf("local_host_name=%s\n",local_host_name));
      //Serial.write(sprintf("httpServerPushUrl=%s\n",httpServerPushUrl));

  } else {
      Serial.print(  "Problem on reading file!");
      Serial.println(CONFIG_CFG_FILE_PATH);
      strcpy(local_host_name,   HOSTNAME_DEF);
      // strcpy(httpServerPushUrl, HTTP_SERVER_PUSH_URL_DEF);
      SET_HTTP_SERVER_PUSH_URL_DEF(httpServerPushUrl);
  }
  Serial.write(sprintf("local_host_name=%s\n",local_host_name));
  Serial.write(sprintf("httpServerPushUrl=%s\n",httpServerPushUrl));
  
}



void saveConfig2LittleFs(char *httpServerPushUrl_p, char *local_host_name_p){
  // https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html
  File file = LittleFS.open("/config.cfg", "w");
  if (file) {
    file.print("local_host_name=");
    file.print(local_host_name_p);
    file.print("\n");
    file.print("httpServerPushUrl=");
    file.print(httpServerPushUrl_p);
    file.print("\n");
  } else {
      Serial.println("Problem on writing file!");
  }
}



void loadCredentials() {
  EEPROM.begin(512);
  EEPROM.get(0, ssid);
  EEPROM.get(0 + sizeof(ssid), password);
  char ok[2 + 1];
  EEPROM.get(0 + sizeof(ssid) + sizeof(password), ok);
  EEPROM.end();
  if (String(ok) != String("OK")) {
    ssid[0] = 0;
    password[0] = 0;
  }
  strcpy(ssid_ini,ssid);
  strcpy(password_ini,password);

  Serial.println("Recovered credentials:");
  Serial.println(ssid);
  Serial.println(password);
  Serial.println(strlen(password) > 0 ? "********" : "<no password>");
}

/** Store WLAN credentials to EEPROM */
void saveCredentials() {
  if (strcmp(ssid_ini,ssid) or strcmp(password_ini,password)){
    /* save to eeprom - password or ssid changed .  */
    EEPROM.begin(512);
    EEPROM.put(0, ssid);
    EEPROM.put(0 + sizeof(ssid), password);
    char ok[2 + 1] = "OK";
    EEPROM.put(0 + sizeof(ssid) + sizeof(password), ok);
    EEPROM.commit();
    EEPROM.end();
  } else {
    /* do nothing .   - password and ssid unchanged */
  }
}
