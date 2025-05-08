/*

* http://norm.edf.fr/internet/catalogue/e74a_d82.htm
* http://vesta.homelinux.free.fr/site/wiki/demodulateur_teleinformation_edf.html 

exemepl trame version heures creuses / heures pleines
ADCO 030222778385 F
OPTARIF HC.. <
ISOUSC 45 ?
HCHC 057254096 ,
HCHP 044220532 )
PTEC HC.. S
IINST 002 Y
IMAX 053 G
PAPP 00550 +
HHPHC A ,
ADPS 045 A
MOTDETAT 000000 B
 *


 */
#include <Arduino_JSON.h>
#include <stdio.h>
#include <string.h>
#include "data_logger.h"
#include "serial_controler.h"

#define DATA_LOGGER_MSG_RCV_TIME_OUT 2000 //  2000ms

int           current_log_index=0;
unsigned long last_log_time=0;

void initLog(){
}

void DataLogger_logNextRcvMsg(){
  last_log_time=millis()+DATA_LOGGER_MSG_RCV_TIME_OUT;
}

void storeInfo(){
  if (++current_log_index<LOG_SIZE){
    ;
  } else {
    current_log_index=0;
  }
  /*                            1604946262 */
  if (data_last_values.time_sec>(unsigned long) 1604912619UL){
    data_log[current_log_index]=data_last_values;  
  }
  
  //data_log[current_log_index]=data_last_values;
}

String info2json(String output, struct dataStruct *data_last_values_p){
  output = F("{\"uptime_ms\":");
  output += String(data_last_values_p->uptime);
  output += F(",\"time_sec\":");
  output += String(data_last_values_p->time_sec);
  output += F(",\"InputPilotState\":\"");
  output += String((char)data_last_values_p->InputPilotState);
  output += F("\",\"OutputPilotState\":\"");
  output += String((char)data_last_values_p->OutputPilotState);

  output += F("\",\"output_pilot_voltage_min_v\":");
  output += String(data_last_values_p->output_pilot_voltage_min_v);
  output += F(",\"output_pilot_voltage_max_v\":");
  output += String(data_last_values_p->output_pilot_voltage_max_v);
  output += F(",\"input_pilot_voltage_min_v\":");
  output += String(data_last_values_p->input_pilot_voltage_min_v);
  output += F(",\"input_pilot_voltage_max_v\":");
  output += String(data_last_values_p->input_pilot_voltage_max_v);

  output += F(",\"power_table_index\":");
  output += String(data_last_values_p->power_table_index);
  output += F(",\"power_table_index_sp\":");
  output += String(data_last_values_p->power_table_index_sp);
  output += F(",\"ErrCtr\":");
  output += String(data_last_values_p->ErrCtr);

/*
   unsigned char InputPilotState;
   unsigned char OutputPilotState;
   float         output_pilot_voltage_min_v,
                 output_pilot_voltage_max_v,
                 input_pilot_voltage_min_v,
                 input_pilot_voltage_max_v;
   unsigned char power_table_index;
   unsigned char power_table_index_sp;
   int           ErrCtr;
*/
 
/*
  output += F(",\"RAW\":\"");
  output += String(data_last_values_p.raw);
  output += "\"";
*/
  output += "}";

  return output;

}

String info2json2ElementLog(String output, struct dataStruct *data_last_values_p){

  /*
  char buff[21];
  struct tm *info_tm;
  time_t rawtime;
  rawtime=(time_t)data_last_values_p->time_sec;
  info_tm = localtime(&rawtime);
  strftime(buff, 20, "%Y%m%dT%H%M%S" , data_last_values_p->time_sec );
  */
  char buff[12];
  
  sprintf(buff,"\"%lu\"",data_last_values_p->time_sec);
  output = buff;
  output += F(":");
  output += info2json(output, data_last_values_p);
  //output += F(",\n");
  
  return output;
}


char *ultoa(unsigned long l, char *f, char *s){
  /* char s[10]; */
  sprintf(s,f,l);
  return s;
}
int checksumControl_OK(char *s){
  return true;
}

void parseLineInputString(String line){
  /*
  char *s,s2[10];
  char lineBuff[INPUT_STRING_SIZE];
  line.toCharArray(lineBuff, INPUT_STRING_SIZE-1);
  if (!checksumControl_OK(lineBuff)){
    return;
  }
  s=strchr(lineBuff,' ')+1;
  unsigned long v;
  */

  //https://arduino.stackexchange.com/questions/42922/get-hour-with-ctime-time-library-with-esp8266
  data_last_values.uptime   = millis();
  data_last_values.time_sec = time(nullptr);

  //https://github.com/arduino-libraries/Arduino_JSON/blob/master/examples/JSONObject/JSONObject.ino
  JSONVar myObject = JSON.parse(line);

  if (myObject.hasOwnProperty("InputPilotState")){ data_last_values.InputPilotState =((const char *)myObject["InputPilotState"])[0];
  } else {                                         data_last_values.InputPilotState ='?'; /* numerical convertion value -1 */
  }
  
  if (myObject.hasOwnProperty("OutputPilotState")){ data_last_values.OutputPilotState=((const char *)myObject["OutputPilotState"])[0];
  } else {                                          data_last_values.OutputPilotState ='?'; /* numerical convertion value -1 */
  }
  
  if (myObject.hasOwnProperty("output_pilot_voltage_min_v")){ data_last_values.output_pilot_voltage_min_v=(double)myObject["output_pilot_voltage_min_v"];
  } else                                                    { data_last_values.output_pilot_voltage_min_v=(float)30.0;
  }
  if (myObject.hasOwnProperty("output_pilot_voltage_max_v")){ data_last_values.output_pilot_voltage_max_v=(double)myObject["output_pilot_voltage_max_v"];
  } else                                                    { data_last_values.output_pilot_voltage_max_v=(float)-30.0;
  }
  if (myObject.hasOwnProperty("input_pilot_voltage_min_v" )){ data_last_values.input_pilot_voltage_min_v=(double)myObject["input_pilot_voltage_min_v"];
  } else                                                    { data_last_values.input_pilot_voltage_min_v=(float)30.0;
  }
  if (myObject.hasOwnProperty("input_pilot_voltage_max_v" )){ data_last_values.input_pilot_voltage_max_v=(double)myObject["input_pilot_voltage_max_v"];
  } else                                                    { data_last_values.input_pilot_voltage_max_v=(float)-30.0;
  }

  if (myObject.hasOwnProperty("power_table_index"    )){ data_last_values.power_table_index    =(int)myObject["power_table_index"];
  } else                                               { data_last_values.power_table_index    =(int)30;
  }
  if (myObject.hasOwnProperty("power_table_index_sp" )){ data_last_values.power_table_index_sp =(int)myObject["power_table_index_sp"];
  } else                                               { data_last_values.power_table_index_sp =(int)30;
  }
  if (myObject.hasOwnProperty("ErrCtr"               )){ data_last_values.ErrCtr               =(int)myObject["ErrCtr"];
  } else                                               { data_last_values.ErrCtr               =(int)-1;
  }

/*
   unsigned char InputPilotState;
   unsigned char OutputPilotState;
   float         output_pilot_voltage_min_v,
                 output_pilot_voltage_max_v,
                 input_pilot_voltage_min_v,
                 input_pilot_voltage_max_v;
   unsigned char power_table_index;
   unsigned char power_table_index_sp;
   int           ErrCtr;
*/
  

  if (last_log_time>millis()){ // DATA_LOGGER_RECURRENCE data_logger_recurrence
    last_log_time=0;
    storeInfo();
  }

  /*  */
  
}
