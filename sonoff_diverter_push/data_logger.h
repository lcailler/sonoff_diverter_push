#ifndef data_logger_h
#define data_logger_h

//#define LOG_SIZE 10   /* 34784 octets (42%) de mémoire dynamique */
//#define LOG_SIZE 100 /* 38016 octets (46%) de mémoire dynamique, ce qui laisse 40320 octets pour les variables locales. Le maximum est de 81920 octets. */
//define LOG_SIZE 150 /* 40108 octets (48%) de mémoire dynamique, */
#define LOG_SIZE 300 /* 45216 octets (55%) de mémoire dynamique, crash !!*/
//#define LOG_SIZE 600 /*600=(60*10) 56016 octets (68%) de mémoire dynamique, */
//#define LOG_SIZE 420 /*420=(60*7) 49536 octets (60%) de mémoire dynamique, */
//#define LOG_SIZE 720 /*720=(60*12) 60336 octets (73%) de mémoire dynamique, crash !!!! */
//#define LOG_SIZE 1440 /*1440=(60*24) break compilation de mémoire dynamique, */


struct dataStruct {
   unsigned long  uptime;
   unsigned long  time_sec;
   
   unsigned char InputPilotState;
   unsigned char OutputPilotState;
   float         output_pilot_voltage_min_v,
                 output_pilot_voltage_max_v,
                 input_pilot_voltage_min_v,
                 input_pilot_voltage_max_v;
   unsigned char power_table_index;
   unsigned char power_table_index_sp;
   int           ErrCtr;
   
//   char raw[17+30];
};

struct dataStruct data_last_values;
dataStruct data_log[LOG_SIZE];
#define DataLogger_GetLastDataValues() (data_last_values)
#define DataLogger_GetLogDataValues(n) (data_log[n])
void DataLogger_logNextRcvMsg();

void initLog();


String info2json(String output, struct dataStruct *data_last_values_p);

String info2json2ElementLog(String output, struct dataStruct *data_last_values_p);

void parseLineInputString(String line);

#endif // data_logger_h
