#include <adc.h>




#define adc_set_channel(C)    output_D(adc_channel_encoder[C]) 

#inline uint16_t adc_read_and_convert (void)
{
   uint8_t    hi, lo;
   uint16_t adc_value;
   #bit   sign = hi.3

   output_low (PIN_ADC_CS);
   hi = spi_read2(0);  
   lo = spi_read2(0);
   output_high (PIN_ADC_CS);
   
   adc_value = make16 (hi & 0x07,lo); 
   
   if (!sign) adc_value += 2048;   
   
   return adc_value;
}


uint16_t adc_test (uint8_t channel)
{
   adc_set_channel(channel); 
   delay_ms (2);
   
   return adc_read_and_convert ();
}


void adc_mesure (void)
{
   static uint8_t    adc_channel = 0;
          uint16_t   adc_current;
   static char       inputOk_current = STATUS_INPUTOK_NOPULSE;
   
//Текущее измерение амплитуды импульса в ТЕКУЩЕМ КАНАЛЕ:
   adc_set_channel(adc_channel); 

   if (PULSE_IN_PROGRESS)   adc_current = adc_read_and_convert();
   else return;
#ifdef PROTEUS
   adc_current = proteus_adc[adc_channel];
#endif

   
//Если имп.еще не закончился - измерение ампл.корректно.      
   if (PULSE_IN_PROGRESS)
   {  
      adc_value[adc_channel] = adc_current;
      
// Проверяем Pin:
      if (adc_channel == 0) 
      {        
         if      (adc_current >= eeprom.Pin_ALPmax)   inputOk_current = STATUS_INPUTOK_TOOBIG;
         else if (adc_current >= eeprom.Pin_TZmax)    inputOk_current = STATUS_INPUTOK_UPPER;
         else if (adc_current >= eeprom.Pin_TZmin)    inputOk_current = STATUS_INPUTOK_NORMA;
         else if (adc_current >= eeprom.Pin_ALPmin)   inputOk_current = STATUS_INPUTOK_LOWER;
         else                                         inputOk_current = STATUS_INPUTOK_TOOLOW;
         
         if (status.inputOk != inputOk_current)
         {
            status.inputOk = inputOk_current;
            SemaforSet(input_changed);
         }
      }
      
//Проверяем Pout:
      if (adc_channel == 4)   
      {
         if (adc_current < eeprom.PowerOutMin) 
         {
            if ((++adc_countOutMin) > eeprom.PowerOutMinCount)
            {
               SemaforSet(fault_PowOutMin);
               adc_countOutMin = 0;
            }
         }
         else 
         {
            status.outputOk  = '1';
            SemaforReset(fault_PowOutMin);
            adc_countOutMin = 0;
         }
      }

// переключаем канал:
      if (++adc_channel > 4) adc_channel = 0;
      
      adc_set_channel(adc_channel); 
   }
   else return;
}


