#include "comparator.h"


void comparator_init (void)
{  
   setup_vref (VREF_4v096);
   setup_dac (DAC_VSS_FVR | DAC_OUTPUT);          //диапазон земля - 4.096В
   dac_write (eeprom.cmp_porog);  
   
//!#ifdef   PROTEUS
//!   setup_comparator(CP2_A1_FVR | CP2_OUT_ON_A5 | CP2_FAST | CP2_INVERT);   
//!#else
   setup_comparator(CP2_A1_DAC | CP2_OUT_ON_A5 | CP2_FAST | CP2_INVERT);   
//!#endif   
}
