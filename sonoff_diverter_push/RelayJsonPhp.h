#ifndef relayjsonphp_h
#define relayjsonphp_h

unsigned char relay_switch_state=0,
              relay_channel_active_number=0; /* 0:manual 1:auto */

void set_relay_switch_state(String argv);

#define set_relay_channel_active_number(x) {relay_channel_active_number=x;}
#define get_relay_channel_active_number()  (relay_channel_active_number)

#endif // relayjsonphp_h
