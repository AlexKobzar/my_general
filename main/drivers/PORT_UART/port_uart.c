#include "port_uart.h"

#define ERR_CMD_UNCN     10

void delay_my (void)
{
   volatile uint8_t a = 255, b = 255, c = 10;
   
   do
   {
      do
      {
         do 
         {
         
         }  while (--a);
         
         a = 255;
        
      }  while (--b);
      
      b = 255;
      restart_wdt();
      
   }  while (--c);
   
}

signed int8 strncmp_rom(rom char *s1, char *s2, size_t n)
{
   for (; n > 0; s1++, s2++, n--)
      if (*s1 != *s2)
         return((*s1 <*s2) ? -1: 1);
      else if (*s1 == '\0')
         return(0);
   return(0);
}

unsigned int atoi_b16_my(char *s) {  // Convert two hex characters to a int8
   unsigned int result = 0;
   int i;

   for (i=0; i<2; i++,s++)  {
      if (*s >= 'A')
         result = 16*result + (*s) - 'A' + 10;
      else                     
         result = 16*result + (*s) - '0';
   }         
     
   return(result);
}



uint8_t CommandProcessor(void)                                                                             
{  
   uint8_t     i;
   uint16_t    j_u16;
   uint8_t     u8Temp;
   char        tempChar;
   uint16_t    u16Temp;
   uint8_t     *pTemp;
   uint8_t     ret = PORT_UART_RET_OK;
   
   static uint8_t set_pointer = 0;
   
   restart_wdt();
                                              

   if      (!strncmp_rom("OFF",BufferRX_UART,3)) // OFFA0   OFFA1  OFF11
   {   
      if (BufferRX_UART[3] != '?')
      {
         supply_manager (BufferRX_UART[3], BufferRX_UART[4]);
         
         if (BufferRX_UART[3] == 'A')
         {
            if (eeprom.status_powerEN != BufferRX_UART[4])
            {
               eeprom.status_powerEN = BufferRX_UART[4];
               write_eeprom (&eepromAddr->status_powerEN, eeprom.status_powerEN);
            }
         }
      }
      
      if (BufferRX_UART[3] == 'A')   status_send();
      else  fprintf (PORT_UART, "OFF %u \r", en_state); //Для отладочного режима
   }   
   
   else if (!strncmp_rom("SWE",BufferRX_UART,3)) // Enable / disable switch
   {   
      pTemp = strchr (BufferRX_UART, ' ');
      tempChar = *(pTemp+1);
      if ((tempChar == STATUS_COMMUTATOR_DISABLE) || (tempChar == STATUS_COMMUTATOR_ENABLE))   
      {
         commutator_manager (tempChar);
         
         for (j_u16=0;j_u16<5000;j_u16++)  delay_us(1);  // delay for outputOK was set
         
         status_send();
         
         if (eeprom.status_commutatorEN != tempChar)
         {
            eeprom.status_commutatorEN = tempChar;
            write_eeprom (&eepromAddr->status_commutatorEN, eeprom.status_commutatorEN);
         }
      }
   } 
   
   else if (!strncmp_rom("ATT",BufferRX_UART,3)) // Установка старт.значения аттенюатора
   {   
      if (BufferRX_UART[3] != '?')
      {
         pTemp = strchr (BufferRX_UART, ' ');
         eeprom.attenuator_start = atoi (pTemp+1);
         
         write_eeprom (&eepromAddr->attenuator_start, eeprom.attenuator_start);
        
         attenuator_calc_koeff ();
      }
      eeprom.attenuator_start = read_eeprom (&eepromAddr->attenuator_start);
      fprintf (PORT_UART, "ATT %u\r", eeprom.attenuator_start);
   } 
   
   else if (!strncmp_rom("ATD",BufferRX_UART,3)) // Установка/опрос текущего значения аттенюатора
   {   
      if (BufferRX_UART[3] != '?')
      {
         pTemp = strchr (BufferRX_UART, ' ');
         attenuator_current = atoi (pTemp+1);
         attenuator_set(attenuator_current);
      }
      fprintf (PORT_UART, "ATD %u\r", attenuator_current);
   }  
   
   else if (!strncmp_rom("AGC",BufferRX_UART,3)) // Включение/вЫключение петли регулирования
   {   
      if (BufferRX_UART[3] != '?')
      {
         pTemp = strchr (BufferRX_UART, ' ');
         
         if (*(pTemp+1) == '0')   SemaforSet(att_correctDet_OFF);
         else 
         {
            if (status.inputOk == STATUS_INPUTOK_NOPULSE)   SemaforSet(att_correctDet_OFF);
            else                                            SemaforReset(att_correctDet_OFF);
         }
         
         if (*(pTemp+2) == '0')   SemaforSet(att_correctTem_OFF);
         else                     SemaforReset(att_correctTem_OFF);
      }

      fprintf (PORT_UART, "AGC %u %u\r", ~semafor.att_correctDet_OFF, ~semafor.att_correctTem_OFF);
   } 
   
   else if (!strncmp_rom("VLT",BufferRX_UART,3)) //****************************
   {   
      if (BufferRX_UART[3] == '5')
      {
         if (BufferRX_UART[4] != '?')
         {
            pTemp = strchr (BufferRX_UART, ' ');
            eeprom.volt_5_min = atoi (pTemp+1);
            pTemp = strchr (pTemp+1, ' ');
            eeprom.volt_5_max = atoi (pTemp+1);
            pTemp = strchr (pTemp+1, ' ');
            eeprom.volt_5_shift = atof (pTemp+1);
            
            write_eeprom (&eepromAddr->volt_5_min, eeprom.volt_5_min);
            write_eeprom (&eepromAddr->volt_5_max, eeprom.volt_5_max);
            eeprom_write_object((uint8_t*)(&eepromAddr->volt_5_shift), (uint8_t*)(&eeprom.volt_5_shift), sizeof (float));
         }
         fprintf (PORT_UART, "VLT5 %u %u %5.3f\r", eeprom.volt_5_min, eeprom.volt_5_max, eeprom.volt_5_shift); 
      }
      else if (BufferRX_UART[3] == '3')
      {
         if (BufferRX_UART[4] != '?')
         {
            pTemp = strchr (BufferRX_UART, ' ');
            eeprom.volt_36_min = atoi (pTemp+1);
            pTemp = strchr (pTemp+1, ' ');
            eeprom.volt_36_max = atoi (pTemp+1);
            pTemp = strchr (pTemp+1, ' ');
            eeprom.volt_36_Ka = atof (pTemp+1);
            pTemp = strchr (pTemp+1, ' ');
            eeprom.volt_36_Kb = atof (pTemp+1);
            
            write_eeprom (&eepromAddr->volt_36_min, eeprom.volt_36_min);
            write_eeprom (&eepromAddr->volt_36_max, eeprom.volt_36_max);
            eeprom_write_object((uint8_t*)(&eepromAddr->volt_36_Ka), (uint8_t*)(&eeprom.volt_36_Ka), sizeof (float));
            eeprom_write_object((uint8_t*)(&eepromAddr->volt_36_Kb), (uint8_t*)(&eeprom.volt_36_Kb), sizeof (float));
         }
         fprintf (PORT_UART, "VLT3 %u %u %3.6f %3.6f\r", eeprom.volt_36_min, eeprom.volt_36_max, eeprom.volt_36_Ka, eeprom.volt_36_Kb); 
      }      
   }
   
   else if (!strncmp_rom("TPM",BufferRX_UART,3)) //****************************
   {   
      if (BufferRX_UART[3] != '?')
      {
         pTemp = strchr (BufferRX_UART, ' ');
         eeprom.temper_max = atoi (pTemp+1);
         pTemp = strchr (pTemp+1, ' ');
         eeprom.temper_start =  atoi (pTemp+1);         

         write_eeprom (&eepromAddr->temper_max, eeprom.temper_max);
         write_eeprom (&eepromAddr->temper_start, eeprom.temper_start);
      }
      
      eeprom.temper_max = read_eeprom (&eepromAddr->temper_max);
      eeprom.temper_start = read_eeprom (&eepromAddr->temper_start);
      fprintf (PORT_UART, "TPM %d %d\r", eeprom.temper_max,  eeprom.temper_start); 
   }   
   
   else if (!strncmp_rom("TPG",BufferRX_UART,3)) //****************************
   {
         temperature_test(); 
         fprintf (PORT_UART, "TPG %d\r", temper_tmp122);    
   }   
   
   else if (!strncmp_rom("EVN",BufferRX_UART,3)) //****************************
   {   
      if (BufferRX_UART[3] != '?')
      {
         pTemp = strchr (BufferRX_UART, ' ');
         eeprom.temper_event = atoi (pTemp+1);
         pTemp = strchr (pTemp+1, ' ');
         eeprom.atten_event = atoi (pTemp+1);
         pTemp = strchr (pTemp+1, ' ');
         eeprom.fault_count = atoi (pTemp+1);        

         write_eeprom (&eepromAddr->temper_event, eeprom.temper_event);
         write_eeprom (&eepromAddr->atten_event, eeprom.atten_event);
         write_eeprom (&eepromAddr->fault_count, eeprom.fault_count);
      }
      
      eeprom.temper_event = read_eeprom (&eepromAddr->temper_event);
      eeprom.atten_event = read_eeprom (&eepromAddr->atten_event);
      eeprom.fault_count = read_eeprom (&eepromAddr->fault_count);
      fprintf (PORT_UART, "EVN %u %u %u\r", eeprom.temper_event, eeprom.atten_event, eeprom.fault_count); 
   } 
   
   else if (!strncmp_rom("CMP",BufferRX_UART,3)) //****************************
   {   
      if (BufferRX_UART[3] != '?')
      {
         pTemp = strchr (BufferRX_UART, ' ');
         eeprom.cmp_porog = atoi(pTemp+1);
         write_eeprom (&eepromAddr->cmp_porog, eeprom.cmp_porog);
         dac_write(eeprom.cmp_porog); 
      }
      eeprom.cmp_porog = read_eeprom (&eepromAddr->cmp_porog);
      fprintf (PORT_UART, "CMP %u \r", eeprom.cmp_porog); 
   }  
   
   else if (!strncmp_rom("PRT",BufferRX_UART,3)) //****************************
   {   
      if (BufferRX_UART[3] != '?')
      {
         pTemp = strchr (BufferRX_UART, ' ');
         eeprom.Prev_timer_1 = atoi(pTemp+1);
         pTemp = strchr (pTemp+1, ' ');
         eeprom.Prev_timer_2 = atoi(pTemp+1);  
         pTemp = strchr (pTemp+1, ' ');
         eeprom.Prev_num_attempts = atoi(pTemp+1); 
         
         write_eeprom (&eepromAddr->Prev_timer_1, eeprom.Prev_timer_1);
         write_eeprom (&eepromAddr->Prev_timer_2, eeprom.Prev_timer_2); 
         write_eeprom (&eepromAddr->Prev_num_attempts, eeprom.Prev_num_attempts);
      }
      eeprom.Prev_timer_1 = read_eeprom (&eepromAddr->Prev_timer_1);
      eeprom.Prev_timer_2 = read_eeprom (&eepromAddr->Prev_timer_2);
      eeprom.Prev_num_attempts = read_eeprom (&eepromAddr->Prev_num_attempts);
      fprintf (PORT_UART, "PRT %u %u %u\r", eeprom.Prev_timer_1, eeprom.Prev_timer_2, eeprom.Prev_num_attempts); 
   }     

   else if (!strncmp_rom("COM",BufferRX_UART,3)) //****************************
   {   
      if (BufferRX_UART[3] != '?')
      {
         pTemp = strchr (BufferRX_UART, ' ');
         if (*(pTemp+1) == '1') commutator_set (ON);
         else commutator_set (OFF);
      }
      fprintf (PORT_UART, "COM %u \r", input_state (PIN_SW_AB)); 
   }     
   
   else if (!strncmp_rom("ADC",BufferRX_UART,3)) //****************************
   {
      fprintf (PORT_UART, "ADC %Lu %Lu %Lu %Lu %Lu\r", adc_value[0], adc_value[1], adc_value[2], adc_value[3], adc_value[4]);    
   }
   
   else if (!strncmp_rom("ADT",BufferRX_UART,3)) //****************************
   {
      pTemp = strchr (BufferRX_UART, ' ');
      u8Temp = atoi(pTemp+1);
      u16Temp = adc_test (u8Temp);
      fprintf (PORT_UART, "ADT %u %Lu\r", u8Temp, u16Temp);
   }
   
   else if (!strncmp_rom("ADV",BufferRX_UART,3)) //****************************
   {
      if (BufferRX_UART[3] != '?')
      {
         pTemp = strchr (BufferRX_UART, ' ');
         eeprom.adc_vref = atof(pTemp+1);
         eeprom_write_object ((uint8_t*)(&eepromAddr->adc_vref), (uint8_t*)(&eeprom.adc_vref), sizeof (float));
      }
      eeprom_read_object ((uint8_t*)(&eepromAddr->adc_vref), (uint8_t*)(&eeprom.adc_vref), sizeof (float));
      fprintf (PORT_UART, "ADV %f\r", eeprom.adc_vref);
   }
   
   else if (!strncmp_rom("PTH",BufferRX_UART,3)) //****************************
   {
      if (BufferRX_UART[3] != '?')
      {
         pTemp = strchr (BufferRX_UART, ' ');
         eeprom.Pin_ALPmin = atol(pTemp+1);
         pTemp = strchr (pTemp+1, ' ');
         eeprom.Pin_TZmin = atol(pTemp+1);
         pTemp = strchr (pTemp+1, ' ');
         eeprom.Pin_TZmax = atol(pTemp+1);
         pTemp = strchr (pTemp+1, ' ');
         eeprom.Pin_ALPmax = atol(pTemp+1);
         
         eeprom_write_object ((uint8_t*)(&eepromAddr->Pin_ALPmin), (uint8_t*)(&eeprom.Pin_ALPmin), sizeof (uint16_t));
         eeprom_write_object ((uint8_t*)(&eepromAddr->Pin_TZmin), (uint8_t*)(&eeprom.Pin_TZmin), sizeof (uint16_t));
         eeprom_write_object ((uint8_t*)(&eepromAddr->Pin_TZmax), (uint8_t*)(&eeprom.Pin_TZmax), sizeof (uint16_t));
         eeprom_write_object ((uint8_t*)(&eepromAddr->Pin_ALPmax), (uint8_t*)(&eeprom.Pin_ALPmax), sizeof (uint16_t));
      }
      
      eeprom_read_object ((uint8_t*)(&eepromAddr->Pin_ALPmin), (uint8_t*)(&eeprom.Pin_ALPmin), sizeof (uint16_t));
      eeprom_read_object ((uint8_t*)(&eepromAddr->Pin_TZmin), (uint8_t*)(&eeprom.Pin_TZmin), sizeof (uint16_t));
      eeprom_read_object ((uint8_t*)(&eepromAddr->Pin_TZmax), (uint8_t*)(&eeprom.Pin_TZmax), sizeof (uint16_t));
      eeprom_read_object ((uint8_t*)(&eepromAddr->Pin_ALPmax), (uint8_t*)(&eeprom.Pin_ALPmax), sizeof (uint16_t));
      
      fprintf (PORT_UART, "PTH %Lu %Lu %Lu %Lu\r", eeprom.Pin_ALPmin, eeprom.Pin_TZmin, eeprom.Pin_TZmax, eeprom.Pin_ALPmax);
   }
   
   else if (!strncmp_rom("VLG",BufferRX_UART,3)) //****************************
   {
         voltmeter_test();
         fprintf (PORT_UART, "VLG %u %u %u\r", volt_36v, volt_5v, BL_FAULT);    
   }
   
   else if (!strncmp_rom("POM",BufferRX_UART,3)) //****************************
   {
      if (BufferRX_UART[3] != '?')
      {
         pTemp = strchr (BufferRX_UART, ' ');
         eeprom.PowerOutMin = atol (pTemp+1);
         pTemp = strchr (pTemp+1, ' ');
         eeprom.PowerOutMinCount = atol (pTemp+1);
         eeprom_write_object((uint8_t*)(&eepromAddr->PowerOutMin), (uint8_t*)(&eeprom.PowerOutMin), 2);
         eeprom_write_object((uint8_t*)(&eepromAddr->PowerOutMinCount), (uint8_t*)(&eeprom.PowerOutMinCount), 2);
      }
      eeprom_read_object((uint8_t*)(&eepromAddr->PowerOutMin), (uint8_t*)(&eeprom.PowerOutMin), 2);
      eeprom_read_object((uint8_t*)(&eepromAddr->PowerOutMinCount), (uint8_t*)(&eeprom.PowerOutMinCount), 2);
      fprintf (PORT_UART, "POM %Lu %Lu \r", eeprom.PowerOutMin, eeprom.PowerOutMinCount);
   }   
   
   else if (!strncmp_rom("STA",BufferRX_UART,3)) //****************************
   {
       status_send();
   }    
   
   else if (!strncmp_rom("STE",BufferRX_UART,3)) //****************************
   {
        pTemp = strchr (BufferRX_UART, ' ');
        period_extended_status = atoi (pTemp+1);
   } 
   
   else if (!strncmp_rom("WDT",BufferRX_UART,3)) //****************************
   {
      if (BufferRX_UART[3] == '!')
      {
         fprintf (PORT_UART, "WDT TEST \r");
         disable_interrupts(GLOBAL); 
         while (true)  {}
      }
   }  
   
   else if (!strncmp_rom("LIN",BufferRX_UART,3))
   {
      if (BufferRX_UART[3] != '?')
      {
         pTemp = strchr (BufferRX_UART, ' ');
         u8Temp = atoi (pTemp+1) - 1;
         pTemp = strchr (pTemp+1, ' ');
         eeprom.det_koef[u8Temp].A = atof (pTemp+1);
         pTemp = strchr (pTemp+1, ' ');
         eeprom.det_koef[u8Temp].B = atof (pTemp+1);
         pTemp = strchr (pTemp+1, ' ');
         eeprom.det_koef[u8Temp].C = atof (pTemp+1);         
        
      
         eeprom_write_object((uint8_t*)(&eepromAddr->det_koef[u8Temp].A), (uint8_t*)(&eeprom.det_koef[u8Temp].A), sizeof (float));
         eeprom_write_object((uint8_t*)(&eepromAddr->det_koef[u8Temp].B), (uint8_t*)(&eeprom.det_koef[u8Temp].B), sizeof (float));
         eeprom_write_object((uint8_t*)(&eepromAddr->det_koef[u8Temp].C), (uint8_t*)(&eeprom.det_koef[u8Temp].C), sizeof (float));
         
         attenuator_calc_koeff ();
      }
      
      pTemp = strchr (BufferRX_UART, ' ');
      u8Temp = atoi (pTemp+1) - 1;
      
      eeprom_read_object((uint8_t*)(&eepromAddr->det_koef[u8Temp].A), (uint8_t*)(&eeprom.det_koef[u8Temp].A), sizeof (float));
      eeprom_read_object((uint8_t*)(&eepromAddr->det_koef[u8Temp].B), (uint8_t*)(&eeprom.det_koef[u8Temp].B), sizeof (float));
      eeprom_read_object((uint8_t*)(&eepromAddr->det_koef[u8Temp].C), (uint8_t*)(&eeprom.det_koef[u8Temp].C), sizeof (float));
      fprintf (PORT_UART, "LIN%u %3.6f %3.6f %3.6f \r", 
                                    u8Temp+1, eeprom.det_koef[u8Temp].A, eeprom.det_koef[u8Temp].B, eeprom.det_koef[u8Temp].C);
   }  
   
   else if (!strncmp_rom("AAA",BufferRX_UART,3))
   {
      fprintf (PORT_UART, "AAA %3.6f %3.6f\r", attenuator_A, attenuator_B);
   }

   else if (!strncmp_rom("EPR",BufferRX_UART,3))
   {
      eeprom_init (MODE_FORSED);
      fprintf (PORT_UART, "EPR\r");
   }
   
   else if (!strncmp_rom("Flash", BufferRX_UART,5)) //Перевод в режим бутлоадера 
   {  
      write_eeprom (1023, 0);
      delay_ms (10);
      reset_cpu ();  
   }       
   
   else if (!strncmp_rom("RST",BufferRX_UART,3)) //Сбросы
   {   
      reset_cpu ();       
   }  
   
   else if (!strncmp_rom("VER",BufferRX_UART,3)) //Опрос версии ПО
   {                                                        
      fprintf (PORT_UART, "VER INDOOR " __DATE__"   " __TIME__ "\r"); 
   }  
   
   else if (!strncmp_rom("SRN",BufferRX_UART,3)) // Чтение/запись серийного номера
   {                         
      if (BufferRX_UART[3] != '?')
      {
         pTemp = strchr (BufferRX_UART, ' ');
         eeprom.serialnum  = atol (pTemp+1);
         eeprom_write_object((uint8_t*)(&eepromAddr->serialnum), (uint8_t*)(&eeprom.serialnum), 2);
      }
      eeprom_read_object((uint8_t*)(&eepromAddr->serialnum), (uint8_t*)(&eeprom.serialnum), 2);
      fprintf (PORT_UART, "SRN %Lu\r", eeprom.serialnum); 
   } 
   
   else if (!strncmp_rom("TTA",BufferRX_UART,3)) // Read/write temperature table
   {                        
      if (BufferRX_UART[3] == 'R')
      {
         disable_interrupts (GLOBAL);
         for (i=0;i<eeprom.temper_table.num_of_points;i++)
         {
            fprintf (PORT_UART, "TTA %u %u %d %d \r", 
                                 eeprom.temper_table.num_of_points, i,
                                 eeprom.temper_table.point[i].temper, 
                                 eeprom.temper_table.point[i].att_corr_unit);
            delay_ms (200);
         }
         enable_interrupts (GLOBAL);
      }
      
      else if (BufferRX_UART[3] == 'W')
      {
         pTemp = strchr (BufferRX_UART, ' ');
         eeprom.temper_table.num_of_points = atoi (pTemp+1);
         pTemp = strchr (pTemp+1, ' ');
         i = atoi (pTemp+1);
         pTemp = strchr (pTemp+1, ' ');
         eeprom.temper_table.point[i].temper = atoi (pTemp+1);
         pTemp = strchr (pTemp+1, ' ');
         eeprom.temper_table.point[i].att_corr_unit = atoi (pTemp+1);
         
         write_eeprom (&eepromAddr->temper_table.num_of_points, eeprom.temper_table.num_of_points);
         write_eeprom (&eepromAddr->temper_table.point[i].temper, eeprom.temper_table.point[i].temper);
         write_eeprom (&eepromAddr->temper_table.point[i].att_corr_unit, eeprom.temper_table.point[i].att_corr_unit);
         
//!         fprintf (PORT_UART, "TTA %u %u %d %d\r", 
//!                     eeprom.temper_table.num_of_points, i,
//!                     eeprom.temper_table.point[i].temper, 
//!                     eeprom.temper_table.point[i].att_corr_unit);
      }

   }    
   
   else if (!strncmp_rom("SET",BufferRX_UART,3)) // Опрос глобальный
   {             
      set_pointer = 1;
      SemaforSet(debug_mode);
      disable_interrupts (GLOBAL);
   }
   
#ifdef  PROTEUS 
   else if (!strncmp_rom("P_1",BufferRX_UART,3)) //****************************
   {
      pTemp = strchr (BufferRX_UART, ' ');
      proteus_adc[0] = atol(pTemp+1);
      pTemp = strchr (pTemp+1, ' ');
      proteus_adc[1] = atol(pTemp+1);
      pTemp = strchr (pTemp+1, ' ');
      proteus_adc[2] = atol(pTemp+1);
      pTemp = strchr (pTemp+1, ' ');
      proteus_adc[3] = atol(pTemp+1);
      pTemp = strchr (pTemp+1, ' ');
      proteus_adc[4] = atol(pTemp+1);
   }
   
   else if (!strncmp_rom("P_2",BufferRX_UART,3)) //****************************
   {
      pTemp = strchr (BufferRX_UART, ' ');
      proteus_temper_tmp122 = atoi(pTemp+1);
   }   
#endif
//*******************************************************************************
//!   else if (!strncmp_rom("T_1",BufferRX_UART,3)) //****************************
//!   {
//!         return ret;
//!   } 
//!   
//!   else if (!strncmp_rom("T_2",BufferRX_UART,3)) //****************************
//!   {
//!         fprintf (PORT_UART, "123456789ABCDEFGHIJKLMNOPQRSTU\r");
//!   }   


//*******************************************************************************
   
   else  fprintf (PORT_UART, "ERR %u\r", ERR_CMD_UNCN); 
   
   if (!set_pointer)    delay_ms (10);
   else
   {           
      delay_ms (100);
      restart_wdt();
   
      switch (set_pointer)
      {
         case 1:
            strcpy (BufferRX_UART, "VER");
            set_pointer++;
            return PORT_UART_RET_RECURSION;
         case 2:
            strcpy (BufferRX_UART, "STA");
            set_pointer++;
            return PORT_UART_RET_RECURSION;  
         case 3:
            strcpy (BufferRX_UART, "TPM?"); //"TPM?"
            set_pointer++;
            return PORT_UART_RET_RECURSION;
         case 4:
            strcpy (BufferRX_UART, "VLT3?");// "VLT3?"
            set_pointer++;
            return PORT_UART_RET_RECURSION;  
         case 5:
            strcpy (BufferRX_UART, "VLT5?"); // "VLT5?"
            set_pointer++;
            return PORT_UART_RET_RECURSION;  
         case 6:
            strcpy (BufferRX_UART, "CMP?"); // "CMP?"
            set_pointer++;
            return PORT_UART_RET_RECURSION;  
         case 7:
            strcpy (BufferRX_UART, "ATT?"); // "ATT?"
            set_pointer++;
            return PORT_UART_RET_RECURSION;    
         case 8:
            strcpy (BufferRX_UART, "OFF?"); // off
            set_pointer++;
            return PORT_UART_RET_RECURSION;   
         case 9:
            strcpy (BufferRX_UART, "POM?"); // POM
            set_pointer++;
            return PORT_UART_RET_RECURSION;  
         case 10:
            strcpy (BufferRX_UART, "EVN?"); // "EVN?"
            set_pointer++;
            return PORT_UART_RET_RECURSION;    
         case 11:
            strcpy (BufferRX_UART, "ADV?");
            set_pointer++;
            return PORT_UART_RET_RECURSION;  
         case 12:
            strcpy (BufferRX_UART, "LIN? 1");
            set_pointer++;
            return PORT_UART_RET_RECURSION;  
         case 13:
            strcpy (BufferRX_UART, "LIN? 2");
            set_pointer++;
            return PORT_UART_RET_RECURSION;
         case 14:
            strcpy (BufferRX_UART, "LIN? 3");
            set_pointer++;
            return PORT_UART_RET_RECURSION;  
         case 15:
            strcpy (BufferRX_UART, "LIN? 4");
            set_pointer++;
            return PORT_UART_RET_RECURSION; 
         case 16:
            strcpy (BufferRX_UART, "LIN? 5");
            set_pointer++;
            return PORT_UART_RET_RECURSION;  
         case 17:
            strcpy (BufferRX_UART, "AGC?");
            set_pointer++;
            return PORT_UART_RET_RECURSION;             
         case 18:
            strcpy (BufferRX_UART, "SRN?"); 
            set_pointer++;
            return PORT_UART_RET_RECURSION;
         case 19:
            strcpy (BufferRX_UART, "PRT?"); 
            set_pointer++;
            return PORT_UART_RET_RECURSION;
         case 20:
            strcpy (BufferRX_UART, "PTH?"); 
            set_pointer++;
            return PORT_UART_RET_RECURSION;            
            
            
         default:
            fprintf (PORT_UART, "_SET\r");
            delay_ms (10);
            set_pointer = 0;
            
            SemaforReset (timer_1s);
            
            clear_interrupt(INT_PULSE);   
            clear_interrupt(INT_TIMER_PULSE); 
            clear_interrupt(INT_TIMER_NO_PULSE); 
            clear_interrupt(INT_TIMER_PAUSE);
            clear_interrupt(INT_BLOCK);
            clear_interrupt(INT_CMP_REV);
            enable_interrupts (GLOBAL);
            
            return PORT_UART_RET_OK;
      }
   }
   
   return ret;
                          
}  //





