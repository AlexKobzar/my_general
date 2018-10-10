#include <temperature.h>

// http://www.ti.com/lit/ds/symlink/tmp124.pdf

void TMP122_read_temper (void)
{
   uint8_t     hi, lo;
   uint16_t    temp;

   output_low (PIN_TMP_CS);
   hi = spi_read(0);  
   lo = spi_read(0);
   output_high (PIN_TMP_CS);
   
   temp = make16 (hi,lo); 
   temper_tmp122 = (int8_t)((temp>>7) & 0x00FF);
}

//***************   Температура   *******************************************

void temperature_test (void)
{
   TMP122_read_temper ();
   
#ifdef PROTEUS
   temper_tmp122 = proteus_temper_tmp122;
#endif   
   
   if ((int8_t)temper_tmp122 > (int8_t)eeprom.temper_max) 
   {  
      supply_emergency_shutdown();
      SemaforSet (fault_temperMax);
      SemaforSet (flag_temperMax);
      status.temperOk  = STATUS_TEMPEROK_TOOBIG;
   }
   else if ((int8_t)temper_tmp122 > (int8_t)eeprom.temper_start)  
   {
      if (semafor.flag_temperMax)   status.temperOk  = STATUS_TEMPEROK_NOSTART;
      else                          status.temperOk  = STATUS_TEMPEROK_WARNING;
   }
   else  
   {
      status.temperOk  = STATUS_TEMPEROK_NORMA;
      SemaforReset (flag_temperMax);
   }
}


