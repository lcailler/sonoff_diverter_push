#ifndef main_loop_h
#define main_loop_h

#include <stdio.h>


//#define OTA_ACTIVE
//#define RECURRENT_SERIAL_CALL_ACTIVE
//#define OTA_PASSWORD_ACTIVE


//#define HOSTNAME_DEF     "sonoff-diverter-push-001"
//#define HOSTNAME_AP_DEF  "sonoff-diverter-push-001-ap"

#define HOSTNAME_DEF     "sonoff-diverter-push"
#define HOSTNAME_AP_DEF  "sonoff-diverter-push-ap"

//#define HOSTNAME_DEF     "sonoff-diverter-push-test00"
//#define HOSTNAME_AP_DEF  "sonoff-diverter-push-test00-ap"

#define SET_HTTP_SERVER_PUSH_URL_DEF(s) sprintf(s,"http://xxxxxxxxxx/modules/%08X/diverter/log_json.php5",ESP.getChipId() )

#define Get_local_host_name()   (local_host_name)
#define Get_httpServerPushUrl() (httpServerPushUrl)

//#define HOSTNAME loadLocalHostNameFromEEProm()
#define HOSTNAME     Get_local_host_name()
//#define HOSTNAME     HOSTNAME_DEF
#define HOSTNAME_AP  HOSTNAME_AP_DEF
//#define Get_local_host_name() (local_host_name)
//#define Get_local_host_name() (loadLocalHostNameFromEEProm())
//#define Get_local_host_name() (loadLocalHostNameFromLittleFs())
//#define Get_httpServerPushUrl() (loadHttpServerPushUrlFromLittleFs())


#include <ESP8266WebServer.h>

//#define DEBUG_APP
//#define DEBUG_WIFIMANAGER

#ifdef DEBUG_APP
#define SERIALPRINTLN(x) Serial.println(x)
#define SERIALPRINT(x) Serial.print(x)
#else  // DEBUG_APP
#define SERIALPRINTLN(x) {;}
#define SERIALPRINT(x) {;}
#endif // DEBUG_APP


/* Don't set this wifi credentials. They are configurated at runtime and stored on EEPROM */
char ssid[33]     = "";
char password[65] = "";

unsigned long millissOffset_sec=0;

ESP8266WebServer server ( 80 );


#define LED_BUILTIN 13   // SONOFF LED   PIN 13
#define GPIO_12_RELAY 12 // SONOFF RELAY PIN 12
#define GPIO_0_BUTTON 0 // SONOFF RELAY PIN 12
unsigned char gpio_0_button_ctrl=0,
              relay_auto_delay=0;

//#ifndef LED_BUILTIN
//#define LED_BUILTIN 13 // ESP32 DOES NOT DEFINE LED_BUILTIN
//#endif

//int LED = LED_BUILTIN;
unsigned char led_ctr_state=0;
unsigned long t250ms=0;
unsigned long t02s=0;
unsigned long t10s=0;
unsigned long t60s=0;

char *getLocalStrftime(char *buff, int buffSize, char *strftime_format);

#endif // main_loop_h
