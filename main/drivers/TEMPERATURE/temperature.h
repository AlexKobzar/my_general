#ifndef TEMPERATURE_H   
#define TEMPERATURE_H   

int8_t     temper_tmp122  = 0;

#ifdef PROTEUS
   int8_t proteus_temper_tmp122 = 25;
#endif

#inline void temperature_test (void);


#endif 
