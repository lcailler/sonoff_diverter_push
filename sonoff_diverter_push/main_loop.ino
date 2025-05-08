#define VERSION "sonoff_diverter_push_commit_based_0e978c5_20250503_2110"
//debug small update -> put module/001 config
//#define VERSION "sonoff_diverter_push_20211103_1925"
// done : update locatime config to TZ_Europe_Paris
// done : update PRG
//#define VERSION "sonoff_diverter_push_20211023_1310"
// to be done : regexp  \[[X.]\]_[0-9][0-9]h[0-9][0-9]_[0-9][0-9]h[0-9][0-9]_.*\n https://regex101.com https://stackoverflow.com/questions/1085083/regular-expressions-in-c-examples
/*
 * 
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>        


int main()
{
    printf("Hello World\n");
regex_t regex;
int reti;
char msgbuf[100];

// Compile regular expression
reti = regcomp(&regex, "\[[X.]\]_[0-9][0-9]h[0-9][0-9]_[0-9][0-9]h[0-9][0-9]_.*\n", 0);
if (reti) {
    fprintf(stderr, "Could not compile regex\n");
    exit(1);
}

// Execute regular expression
reti = regexec(&regex, "[.]_09h00_00h59_TRIAC1_hhh\n", 0, NULL, 0);
if (!reti) {
    puts("Match");
}
else if (reti == REG_NOMATCH) {
    puts("No match");
}
else {
    regerror(reti, &regex, msgbuf, sizeof(msgbuf));
    fprintf(stderr, "Regex match failed: %s\n", msgbuf);
    exit(1);
}

// Free memory allocated to the pattern buffer by regcomp()
regfree(&regex);
    return 0;
}


 * 
 */

//#define VERSION "sonoff_diverter_push_20211022_1255"
// add loadPrgStrFromLittleFs & savePrgStrFromLittleFs
//#define VERSION "sonoff_diverter_push_20210905_2155"
//#define VERSION "sonoff_diverter_push_20210626_1406"
//#define VERSION "sonoff_diverter_push_20210608_1242"
// 
//#define VERSION "sonoff_diverter_push_20210526_1337"
// add http url into config.cfg local file
// add load / save config.cfg functions
// tbd one minute recurrency to set TRIAC ON  [X]_12h00_12h01_001_TRIAC_REQUEST_ON or [.]_12h00_12h01_001_TRIAC_REQUEST_ON
//#define VERSION "sonoff_diverter_push_20210110_2024"
// add push by sending a POST request with content of serial response of "DIVERTER_JSON\n" and send the POST response to serial
//#define VERSION "sonoff_relay_20210103_1542"
// add : remove eeprom save of hostname by renaming functions
// add : add channels mangt
//#define VERSION "sonoff_relay_20201214_0827"
// add hostname setup config
//#define VERSION "sonoff_relay_20201213_1725"
// config flash size FS 256 OTA 3XX 
// tbd add possibility to post serial request instaed of get
// ...tbd add button ctrl relay on/off using 2 channels with 2 priorities : manuel(prio1) and auto(prio2), add a delay to disable auto
// ...tbd remove relay control with get
// add OTA password
//#define VERSION "sonoff_diverter_20201115_1821"
// add under compiler switch 60sec recurrent serial call
// add button to ctrl relay on/off
// add device.json and relay control
// add ota, update reference from CtrErr to ErrCtr
// store log info if time is uptodate (add check log time in seconds is greater than .....)
//
// #define VERSION "sonoff_evse_diverter_20201003_1346"
//#define VERSION "AutoConnectWithFeedbackLED 20200924_2030"
// add teleinfo.json
//#define VERSION "AutoConnectWithFeedbackLED 20200909_1817"
// LED will blink when in config mode

/*
 * Length: Each label can theoretically be from 0 to 63 characters in length. In practice, a length of 1 to about 20 characters is most common, with a special exception for the label assigned to the root of the tree (see below). 
 *
 * Symbols: Letters, numbers are allowed, as well as the dash symbol (“-”). No other punctuation is permitted, including the underscore (“_”). 
 * Case: Labels are not case-sensitive. This means that “Jabberwocky” and “jabberwocky” are both permissible domain name labels, but they are equivalent.
 * 
 */ 

#include "main_loop.h"
#include "relayjsonphp.h"
#include "credentials.h"
#include "serial_push.h"


#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <EEPROM.h>
#include <time.h>
#include <TZ.h>
#include "WiFiManager.h" // https://github.com/tzapu/WiFiManager
#include <ESP.h>

#ifdef OTA_ACTIVE
// includes necessaires au fonctionnement de l'OTA :
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#endif // OTA_ACTIVE

#include "serial_controler.h"
#include "webserver.h"


//for LED status
//#include <Ticker.h>
//Ticker ticker;


/*
 * /!\ add below function in WiFiManager.cpp/.h
  public:
  void setPassSsid( String pass, String ssid);
  String getPass();
  String getSsid();

and add line conneced=true; in function


*/

void test(class WiFiManager *myWiFiManager){
  ;
}

char *getLocalStrftime(char *buff, int buffSize, char *strftime_format){
  time_t rawtime;
  struct tm * timeinfo;
    
  rawtime=time(nullptr);
  timeinfo = localtime (&rawtime);
  strftime (buff,buffSize,strftime_format,timeinfo);
  return buff;
}


boolean isTimeToRun(char *hhmm_start_str, char *hhmm_stop_str, char *hhmm_str){
  // https://www.cplusplus.com/reference/ctime/strftime/
  // https://www.cplusplus.com/reference/cstring/strcmp/?kw=strcmp
  
  if (strcmp(hhmm_start_str, hhmm_stop_str)>0){
      //if ( (strcmp(hhmm_str, hhmm_start_str)>0  ) ||  (strcmp(hhmm_str, hhmm_stop_str)<=0 ) ){
      if   ( (strcmp(hhmm_str, hhmm_start_str)>=0 ) ||  (strcmp(hhmm_str, hhmm_stop_str)<=0 ) ){
        return true;
      } else {
        return false;
      }
    
  } else {
      //if ( (strcmp(hhmm_str, hhmm_start_str)>=0 ) && (strcmp(hhmm_str, hhmm_stop_str)<0  ) ){
      if   ( (strcmp(hhmm_str, hhmm_start_str)>=0 ) && (strcmp(hhmm_str, hhmm_stop_str)<=0 ) ){
        return true;
      } else {
        return false;
      }
    
  }

  return true;
  
}

boolean isTimeToRun2(char *hhmm_start_stop_str, char *hhmm_str){
  /* expected format hhmm_start_stop_str: "[.]_00h00_23h59_"  hhmm_str: 23h59*/
/*
      Serial.print("-");
      Serial.print(hhmm_start_stop_str);
      Serial.print(hhmm_str);
      Serial.print(">");
*/
  if (hhmm_start_stop_str[ 1]=='X'){
    char  hhmm_start_str[6],
          hhmm_stop_str[6];
    if (   (hhmm_start_stop_str[ 0]=='[')
/*      && (hhmm_start_stop_str[ 1]=='X') */
        && (hhmm_start_stop_str[ 2]==']')
        && (hhmm_start_stop_str[ 3]=='_')
        && (hhmm_start_stop_str[ 6]=='h')
        && (hhmm_start_stop_str[ 9]=='_')
        && (hhmm_start_stop_str[12]=='h')
        && (hhmm_start_stop_str[15]=='_')
        && (hhmm_str[2]=='h')
        && (strlen(hhmm_start_stop_str)>15)
        && (strlen(hhmm_str)>4)
       ){
      strncpy(hhmm_start_str, hhmm_start_stop_str+4 ,5);
      hhmm_start_str[5]=0;
      strncpy(hhmm_stop_str,  hhmm_start_stop_str+10 ,5);
      hhmm_stop_str[5]=0;
/*
      Serial.print("-");
      Serial.print(hhmm_start_str);
      Serial.print(hhmm_stop_str);
      Serial.print(hhmm_str);
      Serial.print("+");
*/
      return isTimeToRun(hhmm_start_str, hhmm_stop_str, hhmm_str);
    }
    
  }
  return false;
  
}





/* gets called when WiFiManager enters configuration mode */
void configModeCallback(class WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  // ticker.attach(0.1, tick);
}

/* callback notifying us of the need to save config */
void saveConfigCallback() {
  Serial.println("Should save config");
  Serial.println(WiFi.psk());
  Serial.println(WiFi.SSID());
  strcpy(password,WiFi.psk().c_str());
  strcpy(ssid,    WiFi.SSID().c_str());
  saveCredentials();
}

unsigned long NtpOffsetVsMillis(){
  struct tm timeinfo;
  //time_t now;
  //time(&now);
  millissOffset_sec= time(nullptr)-millis()/1000;

  return millissOffset_sec;
}

void setup() {
    Serial.begin(9600); /* EVSE DIVERTER */
    //Serial.begin(115200);
    //Serial.begin(1200, SERIAL_7E1); /* TELEINFO (1200 baud, 7bits, parité paire, 1 bit de stop) */
    if (LittleFS.begin()) {
      Serial.println(F("LittleFS system mounted with success"));
    } else {
      Serial.println(F("An Error has occurred while mounting LittleFS"));
    }
    delay(3000);
    Serial.println("starting wifimanager");
    Serial.printf("VERSION:\"%s\"\n",VERSION);
    Serial.printf(" ESP8266 Chip id = %08X\n", ESP.getChipId());
    char s[255] = "";
    SET_HTTP_SERVER_PUSH_URL_DEF(s);
    Serial.printf(" ESP8266 URL POST : \"%s\"\n", s);
    
  loadCredentials(); // Load WLAN credentials from network
  //saveLocalHostName2Eeprom(HOSTNAME_DEF);
  //loadLocalHostNameFromEEProm();
  //saveLocalHostName2LittleFs(HOSTNAME_DEF);
  //loadLocalHostNameFromLittleFs();
  loadConfigFromLittleFs();
  //saveStringAsPrgStrFromLittleFs(String(F("[.]_00h00_23h59_DIVERTER_GETJSON_TEST\n")));
  
  //WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  //WiFi.begin(ssid, password);
  // put your setup code here, to run once:
//  Serial.begin(115200);
  
  //set led pin as output
  pinMode(GPIO_0_BUTTON, INPUT);
  pinMode(GPIO_12_RELAY, OUTPUT);
  digitalWrite(GPIO_12_RELAY, LOW);
  pinMode(LED_BUILTIN, OUTPUT);
  // start ticker with 0.5 because we start in AP mode and try to connect
  //ticker.attach(0.6, tick);
  //ticker.detach();
  //keep LED on
  digitalWrite(LED_BUILTIN, LOW);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;
  #ifdef DEBUG_WIFIMANAGER
  wm.setDebugOutput(true);
  #else  // NO DEBUG_WIFIMANAGER
  wm.setDebugOutput(false);
  #endif
  wm.setPassSsid(password,ssid);
  //set config save notify callback
  wm.setSaveConfigCallback(saveConfigCallback);
  //reset settings - for testing
  // wm.resetSettings();
  Serial.println("starting wifimanager s2");
    
  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  //wm.setTimeout(30);
  // always start configportal for 30sec while
  wm.setConfigPortalTimeout(30); 

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wm.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
//  if (!wm.autoConnect()) {
//    Serial.println("failed to connect and hit timeout");
//    //reset and try again, or maybe put it to deep sleep
//    ESP.restart();
//    delay(1000);
//  }




  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey LED1 :)");

#ifdef HOSTNAME_AP
  if (wm.startConfigPortal(HOSTNAME_AP)){  /* https://www.instructables.com/id/ESP8266-and-ESP32-With-WiFiManager/ */
#else
  if (wm.startConfigPortal()){  /* https://www.instructables.com/id/ESP8266-and-ESP32-With-WiFiManager/ */
#endif /* HOSTNAME_AP */
    Serial.println("portal config timeout, continue.");
  } else {
    Serial.println("portal config success.");
  }
//  wm.connectWifi(ssid,password);
  Serial.println("wm PassSsid");
  Serial.println(wm.getPass());
  Serial.println(wm.getSsid());
  Serial.print(F("hostname:"));
  Serial.println(HOSTNAME);
  Serial.print(F("Get_local_host_name:"));
  Serial.println(Get_local_host_name());
  
  Serial.println("wifi PassSsid");
  Serial.println(WiFi.psk());
  Serial.println(WiFi.SSID());

  if (!MDNS.begin(HOSTNAME))   {
    Serial.println("Error setting up MDNS responder!");
  }
  else {
    // Add service to MDNS-SD
    MDNS.addService("http", "tcp", 80);
    Serial.println("mDNS responder started");

  }

  Serial.print("[IP ");              
  Serial.print(WiFi.localIP()); 
  Serial.println("]");

  webserver_setup();
  Serial.println ( "HTTP server started" );


  
  Serial.println(millis());
  Serial.println("end of setup");
  WiFi.softAPdisconnect(true);

//init and get the time  https://lastminuteengineers.com/esp32-ntp-server-date-time-tutorial/
  //configTime(3600, 3600, "europe.pool.ntp.org");
  configTime(TZ_Europe_Paris, "europe.pool.ntp.org");
  {
    time_t tnow = time(nullptr);
    Serial.print(String(ctime(&tnow)));
  }
  //printLocalTime();
  //NtpOffsetVsMillis();
  
  setupSerial();

// http://arduino.esp8266.com/Arduino/versions/2.0.0/doc/ota_updates/ota_updates.html
  #ifdef OTA_ACTIVE
  ArduinoOTA.setHostname(HOSTNAME); // on donne une petit nom a notre module
  

  #ifdef OTA_PASSWORD_ACTIVE
  // void setPassword(const char* password);
  // ArduinoOTA.setPasswordHash("");
  ArduinoOTA.setPassword(HOSTNAME);
  #endif // OTA_PASSWORD_ACTIVE
  
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });

  
  ArduinoOTA.begin(); // initialisation de l'OTA
  Serial.println("OTA initialized");
  #endif // OTA_ACTIVE

  loadPrgStrFromLittleFs();

}

boolean loop_60sec_processed(){


  char resp[41];
       //url[]="http://diverter-lc-01a.local/~div/controlers_2/tests/modules/test/diverter/log_json.php";
       //url[]   ="http://79.90.88.101:39023/~div/controlers_2/tests/modules/test/diverter/log_json.php";
       //url[]     ="http://79.90.88.101:39023/~div/controlers_2/tests/modules/sonoff_001/diverter/log_json.php";

  resp[0]=0;
  clearReceivedMessage();
  Serial.print(F("DIVERTER_GETJSON\n"));


  // serial_push2url( url, resp, sizeof(resp)-1 );
  if (strlen(httpServerPushUrl)>0){  
    serial_push2url( httpServerPushUrl, resp, sizeof(resp)-1 );
    SERIALPRINTLN(F("strlen(resp):"));
    SERIALPRINTLN(strlen(resp));
    SERIALPRINTLN(httpServerPushUrl);
    
    if (resp[0]!=0){
      SERIALPRINTLN(resp);
      
      char *p0=strstr(resp,"<prg>");
      if (p0!=null){
        char *p1=strstr(resp,"</prg>");
        if (p1!=null){
          *p1=0; /* end of str */
          if (p1>p0){
            saveStringAsPrgStrFromLittleFs( p0+5 );
          }
          *p1='<';
        }
      }

      p0=strstr(resp,"<cmd>");
      if (p0!=null){
        char *p1=strstr(resp,"</cmd>");
        if (p1!=null){
          *p1=0; /* end of str */
          if (p1>p0){
            Serial.print( p0+5 );
          }
          *p1='<';
        }
      }

    }
  }

  if (prgStr.length()>17){ /* "[.]_XXhXX_XXhXX_" */
    if ( prgStr.startsWith("[X]") ){
      //time_t rawtime;
      //struct tm * timeinfo;
      char hhmm_str[6];
      char hhmm_start_stop_str[17];
      boolean isTimeToRunNow=false;
      
      strncpy(hhmm_start_stop_str, prgStr.c_str() ,sizeof(hhmm_start_stop_str)-1);
      hhmm_start_stop_str[sizeof(hhmm_start_stop_str)-1]=0;

      /*
      rawtime=time(nullptr);
      timeinfo = localtime (&rawtime);
      strftime (hhmm_str,sizeof(hhmm_str),"%Hh%M",timeinfo);
      */
      getLocalStrftime(hhmm_str,sizeof(hhmm_str),"%Hh%M");

      isTimeToRunNow = isTimeToRun2(hhmm_start_stop_str, hhmm_str);
      
      if ( prgStr.endsWith(F("_RELAIS_ON\n")) ){
        if (isTimeToRunNow){
          set_relay_switch_state("1");
        }else{
          set_relay_switch_state("0");
        }
      
        
      } else if ( prgStr.endsWith(F("_TRIAC1_ON\n")) ) {
        if (isTimeToRunNow){
          Serial.print(F("001_TRIAC_REQUEST_ON\n"));
        }else{
          //Serial.println(F("002_TRIAC_REQUEST_OFF")); triac goes off by its own
        }
        
      } /* XXhXX_[ON_001_TRIAC_REQUEST_ON]_[OFF_002_TRIAC_REQUEST_OFF]  */
    }

      
    
  }
  
  return true;

}

boolean loop_10sec_processed(){

#ifdef RECURRENT_SERIAL_CALL_ACTIVE  

  if (serialGetTxMsgAvailable()){
    serialSendStringMsgAndLog(F("DIVERTER_GETJSON"));
    //serialSendStringMsgAndLog(F("EVSE_GETJSON_01"));
    return true;
  } else {
    return false;
  }

#endif // RECURRENT_SERIAL_CALL_ACTIVE

  return true;
}

boolean loop_250ms_processed(){
  
    if (++led_ctr_state>15){
      led_ctr_state=0;
    }
    if (WiFi.status()== WL_CONNECTED) {
      if ((0x0001<<led_ctr_state) & 0xFF00){
        digitalWrite(LED_BUILTIN, LOW); /* LED ON */
      } else {
        digitalWrite(LED_BUILTIN, HIGH);

      }
    } else{
      digitalWrite(LED_BUILTIN, LOW);
    }

    if          (digitalRead(GPIO_0_BUTTON)==0){
      if (gpio_0_button_ctrl>8) {
        digitalWrite(GPIO_12_RELAY, digitalRead(GPIO_12_RELAY)?LOW:HIGH);
        gpio_0_button_ctrl=0;
        set_relay_channel_active_number(0);
      }
      else {
        gpio_0_button_ctrl++;
      }
    } else {  /* digitalRead(GPIO_0_BUTTON)==1 */
      if (gpio_0_button_ctrl>0) { gpio_0_button_ctrl--; }
      else{   /* auto state management */
        if (get_relay_channel_active_number()==1){
          if (relay_auto_delay++>16) {
            relay_auto_delay=0;
            set_relay_switch_state(String("2"));
          }
        }
      }
    }
    
  return true;
}


boolean loop_02sec_processed(){
    SERIALPRINT("loop:");
    SERIALPRINT("[IP ");              
    SERIALPRINT(WiFi.localIP()); 
    SERIALPRINTLN("]");
    /*
    if (WiFi.status()== WL_CONNECTED) {
      ticker.attach(1.0, tick);
    } else {
      ticker.detach();
      //keep LED on
      digitalWrite(LED, LOW);
    }
    */
  return true;
}

void loop() {
  // put your main code here, to run repeatedly:
  #ifdef OTA_ACTIVE
  ArduinoOTA.handle();
  #endif // OTA_ACTIVE
  
  
  MDNS.update();
  server.handleClient();
  if (Serial.available() > 0){
    mngReceivedMessagesComplete();
  } else {
    unsigned long t_millis=millis();
  
    if      (t_millis-t250ms>250){
      
      if (loop_250ms_processed()){
        t250ms+=(unsigned long)250;
      }

    } else if (t_millis-t02s>2000){
      
      if (loop_02sec_processed()){
        t02s+=(unsigned long)2000;
      }

    } else if (t_millis-t10s>10000){
      
      if (loop_10sec_processed()){
        t10s+=(unsigned long)10000;
      }
      
    } else if (t_millis>t60s){
      if (loop_60sec_processed()){
        t60s+=(unsigned long)60000;
      }
    } else ;
    
  }

}
