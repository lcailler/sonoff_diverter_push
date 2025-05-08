#include "main_loop.h"
#include "relayjsonphp.h"

void set_relay_switch_state(String argv){
  if        (argv == String("1")) {
    digitalWrite(GPIO_12_RELAY, HIGH);   // Turn the relay on
    
  } else if (argv == String("0")) {
    digitalWrite(GPIO_12_RELAY, LOW);   // Turn the relay on
    
  } else if (argv == String("2")) {
    if (digitalRead(GPIO_12_RELAY)==0) {
      digitalWrite(GPIO_12_RELAY, HIGH);   // Turn the relay on
    } else{
      digitalWrite(GPIO_12_RELAY, LOW);   // Turn the relay off
    }
  }
}
