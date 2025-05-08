#ifndef credentials_h
#define credentials_h

/** Load WLAN credentials from EEPROM */
char ssid_ini[sizeof(ssid)]         = "";
char password_ini[sizeof(password)] = "";
char local_host_name[64] = "";
char local_host_name_ap[sizeof(local_host_name)] = "";
char httpServerPushUrl[255] = "";

String prgStr=String("");

void loadPrgStrFromLittleFs();
void savePrgStrFromLittleFs();
void saveStringAsPrgStrFromLittleFs(String s);
void loadConfigFromLittleFs();
void saveConfig2LittleFs(char *httpServerPushUrl_p, char *local_host_name_p);
void loadCredentials();
void saveCredentials();

#endif // credentials_h
