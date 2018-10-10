#include <voltmeter.h>

//*******************   Напряжение   *********************************************
void voltmeter_init (void)
{
//!    setup_adc(ADC_CLOCK_INTERNAL|ADC_TAD_MUL_0);
   setup_adc(ADC_CLOCK_DIV_64 | ADC_TAD_MUL_20 );//Tad = ((1/40MHz)*64)*2 = 3,2us
   set_adc_channel (6);
}

void voltmeter_test (void)
{  
      set_adc_channel (7);
      volt_36v = read_adc(ADC_START_AND_READ);
      set_adc_channel (6);
      if ((volt_36v < eeprom.volt_36_min) || (volt_36v > eeprom.volt_36_max))  SemaforSet(fault_36V);
      else                                                                     SemaforReset(fault_36V);

      volt_5v = read_adc(ADC_START_AND_READ);
      set_adc_channel (7);
      if ((volt_5v < eeprom.volt_5_min) || (volt_5v > eeprom.volt_5_max))      SemaforSet(fault_5V);
      else                                                                     SemaforReset(fault_5V); 
}
