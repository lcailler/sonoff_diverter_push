
def writeEvseJson_test():
   import json
   data_ref_json = json.loads('{"InputPilotState":"F","ErrCtr":10,"uptime_ms":434948990,"output_pilot_voltage_min_v":9.09,"log_date":"20201109T182907","input_pilot_voltage_min_v":0.62,"output_pilot_voltage_max_v":9.14,"EOM":"-ENDOFMESSAGE-","InputPilotStatus":1,"InputPilotStateNum":6,"OutputPilotStatus":2,"input_pilot_voltage_max_v":8.5,"power_table_index":0,"power_table_index_sp":0,"OutputPilotState":"C","OutputPilotStateNum":3}')
   writeEvseJson(data_ref_json)

def writeEvseJson(data_ref_json):
   serialport = "/dev/cu.usbserial-A8007Zfw"
   #serialport = "/dev/cu.wchusbserial1410"
   import serial, time, json #, urllib2, json


   ser = serial.Serial(serialport, 9600, timeout=3)
   while True:
      s=ser.readline(100).decode('utf-8')
      if len(s)>0 :
         print(s)
         print (type(s))
         if s.startswith("EVSE_GETJSON_01"):
            print("send json")
            ser.write( (json.dumps(data_ref_json)+"\n").encode())
   ser.close()

def readEvseJson():

   import urllib.request, json
   data_ref_json = json.loads('{"InputPilotState":"F","ErrCtr":10,"uptime_ms":434948990,"output_pilot_voltage_min_v":9.09,"log_date":"20201109T182907","input_pilot_voltage_min_v":0.62,"output_pilot_voltage_max_v":9.14,"EOM":"-ENDOFMESSAGE-","InputPilotStatus":1,"InputPilotStateNum":6,"OutputPilotStatus":2,"input_pilot_voltage_max_v":8.5,"power_table_index":0,"power_table_index_sp":0,"OutputPilotState":"C","OutputPilotStateNum":3}')
   # {"uptime_ms":420493,"time_sec":1604946969,"InputPilotState":"F","OutputPilotState":"C","output_pilot_voltage_min_v":9.09,"output_pilot_voltage_max_v":9.14,"input_pilot_voltage_min_v":0.62,"input_pilot_voltage_max_v":8.50,"power_table_index":0,"power_table_index_sp":0,"CtrErr":-1}
   #f = urllib.request.urlopen('http://sonoff_evse_b.local/datalog.json')
   f = urllib.request.urlopen('http://sonoff_evse_b.local/data.json')

   data_json = json.loads(f.read().decode('utf-8'))
   print(data_json)
   for k in data_json:
      if k in data_ref_json.keys():
         print(k,data_ref_json[k]==data_json[k])
   f.close()

def test00():
   print ("12345ooo".startswith("1234") )
   print ("X12345ooo".startswith("1234") )

#readEvseJson()
writeEvseJson_test()
#test00()