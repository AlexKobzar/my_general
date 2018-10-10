#ifndef VOLTMETER_H   
#define VOLTMETER_H  

#include <stdint.h>

// *****************   Измерение напряжений:
uint8_t  volt_36v;
uint8_t  volt_5v;


#inline void voltmeter_init (void);
#inline void voltmeter_test (void);

#endif
