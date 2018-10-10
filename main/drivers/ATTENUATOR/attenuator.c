#include <attenuator.h>





//**********************   À Ò Ò Å Í Þ À Ò Î Ð   *****************************
// http://www.analog.com/media/en/technical-documentation/data-sheets/HMC1119.pdf




#inline void delay_adc (uint8_t count)
{
   while (--count);   
}

void attenuator_set (uint8_t value) // Value 0...127
{
   uint8_t i;

   setup_spi (SPI_DISABLED);   // TMP
   output_low  (PIN_MOSI);
   shift_left (&value,1,0);
   
   for (i=0;i<7;i++)
   {
      output_bit( PIN_MOSI,  shift_left(&value,1,0));
      delay_adc (1);
      output_high (PIN_SCK);
      delay_adc (1);
      output_low (PIN_SCK);
   }
   output_low  (PIN_MOSI);
   output_high (PIN_ATT_CS);
   delay_adc (2);
   output_low  (PIN_ATT_CS);

   setup_spi (SPI_MASTER/* | SPI_XMIT_L_TO_H*/ | SPI_L_TO_H | SPI_CLK_DIV_16);   // TMP
}


void attenuator_calc_koeff (void)
{
   attenuator_A = eeprom.det_koef[0].A * 4.0;
   attenuator_B = eeprom.det_koef[0].B * 4.0 + ((float)eeprom.attenuator_start - 40.0);
}


void attenuator_correct (void)
{  
   static   uint8_t  attenuator_Det_previous = eeprom.attenuator_start;
            uint8_t  attenuator_Det;
            int8_t   attenuator_Tem = 0;
            uint16_t adc_value_channel_1 = adc_value[0];
   
   int8_t   temperature_current;
   static int8_t temperature_previous = 25;

   uint8_t i;
   
   float  integral, fractional, att; 


// *******************************************   Detector correction:

   if (!semafor.att_correctDet_OFF)
   {
      att = ((float)adc_value_channel_1 * attenuator_A) + attenuator_B;
      fractional = modf (att, &integral);
      
      if (fractional < 0.5)  attenuator_Det = (uint8_t)integral;
      else                   attenuator_Det = (uint8_t)(integral + 1.0);
      
      if (adc_value_channel_1 < eeprom.Pin_ALPmin)
      {
         attenuator_Det = attenuator_Det_previous;
      }
      else
      {
         attenuator_Det_previous = attenuator_Det;
      }
      
   }
   else
   {
      attenuator_Det = eeprom.attenuator_start;
      attenuator_Det_previous = attenuator_Det;
   }
   

   
// *********************************************   Temperature correction:   
   if (!semafor.att_correctTem_OFF)
   {
//!      if ((int8_t)temper_tmp122 >= (int8_t)temperature_previous)  temperature_current = temper_tmp122;
//!      else                                                        temperature_current = temper_tmp122 + (int8_t)1;
//!      temperature_previous = temper_tmp122;

      temperature_current = temper_tmp122;
   
      i = 0;
      do
      {
         if ((int8_t)temperature_current <= (int8_t)eeprom.temper_table.point[i].temper)
         {
            attenuator_Tem = eeprom.temper_table.point[i].att_corr_unit;
            break;
         }
      }  while (++i <= eeprom.temper_table.num_of_points);
      
      if (i == eeprom.temper_table.num_of_points)  
                        attenuator_Tem = eeprom.temper_table.point[eeprom.temper_table.num_of_points-1].att_corr_unit;
   }
   
// *********************************************   Attenuator SET: 
   attenuator_current = (uint8_t)((int8_t)attenuator_Det + (int8_t)attenuator_Tem);
   
   if (attenuator_current > 127) attenuator_current = 127;
   
   attenuator_set (attenuator_current); 
}


