#include <18F46K22.h>
#device HIGH_INTS=TRUE

#include <main.h>


#include <stdint.h>
#include <string.h>
#include <stdlib.h> 
#include <math.h>

//!#define PROTEUS   // ***********************************************


#define COMPARATOR

#define  INT_TIMER_PULSE       INT_TIMER2
#define  INT_TIMER_PAUSE       INT_TIMER6
#define  INT_TIMER_NO_PULSE    INT_TIMER3
#define  INT_CMP_REV           INT_EXT1_H2L
#define  INT_BLOCK             INT_EXT_L2H

#ifdef  COMPARATOR
   #define INT_PULSE          INT_COMP2
#else
   #define INT_PULSE          INT_EXT2//_L2H
#endif



#define  set_timer_pulse   set_timer2
#define  set_timer_pause   set_timer6
#define  set_timer_noPulse set_timer3


#define pulse_disable()          disable_interrupts(INT_PULSE);            \
                                 disable_interrupts(INT_TIMER_PULSE);      \
                                 disable_interrupts(INT_TIMER_NO_PULSE);   \
                                 disable_interrupts(INT_TIMER_PAUSE);      \
                                 disable_interrupts(INT_CMP_REV);         
                                 
                           
#define  clear_interrupt_all()   clear_interrupt(INT_PULSE);         \
                                 clear_interrupt(INT_TIMER_PULSE);   \
                                 clear_interrupt(INT_TIMER_NO_PULSE);\   
                                 clear_interrupt(INT_TIMER_PAUSE);   \
                                 clear_interrupt(INT_BLOCK);         \
                                 clear_interrupt(INT_CMP_REV);                           


#ifndef  PROTEUS  
   #include <bootloader.h>
#endif

#include "drivers\status.h"
#include "drivers\EEPROM\myeeprom.h"
#include "drivers\GPIO\gpio.h"
#include "drivers\PORT_UART\port_uart.h"
#include "drivers\ATTENUATOR\attenuator.h"
#include "drivers\ADC\adc.h"
#include "drivers\VOLTMETER\voltmeter.h"
#include "drivers\SUPPLY\supply.h"
#include "drivers\TEMPERATURE\temperature.h"
#include "drivers\TIMERS\timers.h"
#include "drivers\COMPARATOR\comparator.h"




void testPin (uint8_t sw)  //
{
   if (!sw)  bufPortA &= ~(1<<2);  //OFF
   else      bufPortA |= (1<<2);                            //ON
   
   output_A (bufPortA);
}

      

//*****************************************************************************
//             G L O B A L   V A R I A B L E S          
//*****************************************************************************

struct 
{
   int1  UART_Received;          // 1 - приняты данные UART
   int1  timer_1s;               // 1 - прошла 1 секунда
   int1  debug_mode;             // 1 - давать расширенную инфу при авариях
   
   int1  fault_CMP_REV;
   int1  fault_BLOCK;
   int1  fault_5V;
   int1  fault_36V;
   int1  fault_PowOutMin;
   int1  fault_temperMax;
   
   int1  flag_temperMax;
   int1  input_changed;
   
   int1  att_correctDet_OFF;  // Флаг Выключить коррекцию аттенюатора (внешней командой)
   int1  att_correctTem_OFF; 
   
} semafor;

uint8_t  period_extended_status = 0;
   
#define SemaforSet(A)   semafor.A=1
#define SemaforReset(A) semafor.A=0 
#define SemaforToggle(A) semafor.A+=1 





uint8_t  faultTimer = 0;              // timer fault period, sec


#define TEST5
#define PULSE_IN_PROGRESS     C2OUT
#define BL_FAULT              input_state(PIN_BL)
#define LOAD_GOOD             input_state(PIN_CMP_REV)
#define LOW_VOLTAGE_GOOD      (!semafor.fault_5V)&&(!BL_FAULT)
#define VOLTAGE_GOOD          (!semafor.fault_5V)&&(!semafor.fault_36V)&&(!BL_FAULT)
#define TEMPERATURE_GOOD      (!semafor.fault_temperMax)&&(semafor.temper_start_ok)




//*****************************************************************************                                         
//              П Р Е Р Ы В А Н И Я             
//*****************************************************************************
//!#priority ccp3,timer2,timer0,rda



//*****************************************************************************
//                           Входной импульс  
//              Инициализация таймеров, измерение амплитуды
//*****************************************************************************
#ifdef  COMPARATOR
   #INT_COMP2 HIGH //
#else
   #INT_EXT2 HIGH
#endif   
void pulse_isr ()
{   
   uint8_t i;
//Проверка отраженного сигнала:
   if (!LOAD_GOOD)
   { 
      commutator_set (OFF);
      SemaforSet (fault_CMP_REV);
      return;
   }
   
   if (PULSE_IN_PROGRESS)
   {         
//Запуск таймера на макс.длительность импульса 150мкс:   
      set_timer_pulse(0);              
      clear_interrupt(INT_TIMER_PULSE);
      enable_interrupts(INT_TIMER_PULSE);  
      
//Запуск таймера контроля наличия вх.импульса 4мс
      set_timer_noPulse(0); 
      clear_interrupt(INT_TIMER_NO_PULSE);
      enable_interrupts(INT_TIMER_NO_PULSE);
 
// Задержка:
      for (i=0;i<8;i++)
      {
         delay_us (1);
         
         if (!LOAD_GOOD)
         { 
            commutator_set (OFF);
            SemaforSet (fault_CMP_REV);
         }
      }
      
// Измерить амплитуду текущего импульса        
      adc_mesure ();
      
//Проверка отраженного сигнала:
   if (!LOAD_GOOD)
   { 
      commutator_set (OFF);
      SemaforSet (fault_CMP_REV);
   }      
      
// вЫключить перывание по обнаружению инпульса.
      comparator_init ();
      disable_interrupts(INT_PULSE);   
      clear_interrupt(INT_PULSE); 
   }
   else
   {
      commutator_set (OFF);
      clear_interrupt(INT_PULSE);
   }
}


//*****************************************************************************
//                          Пропадание питания -5В 
//*****************************************************************************
#INT_EXT HIGH
void  signal_BL_isr(void) 
{
   if (BL_FAULT)
   {
      supply_emergency_shutdown();
      SemaforSet (fault_BLOCK);
   }
}

//*****************************************************************************
//                          Превышение отраженной мощности 
//*****************************************************************************
#INT_EXT1
void  signal_CMP_REV_isr(void) 
{
   if (!LOAD_GOOD)
   {
      commutator_set (OFF);
      SemaforSet (fault_CMP_REV);
   }
}


//*****************************************************************************
//                  Таймер длительности импульса    150 мкс
//*****************************************************************************
#INT_TIMER2
void timer_pulse_isr (void)
{
   commutator_set (OFF);
    
   disable_interrupts(INT_TIMER_PULSE); 
   
   set_timer_pause(0);                   //Запуск таймера на макс.длительность 600мкс:    
   clear_interrupt(INT_TIMER_PAUSE);
   enable_interrupts(INT_TIMER_PAUSE);  
   
   clear_interrupt(INT_PULSE);
}


//*****************************************************************************
//                   Таймер защитного интервала - 600мкс 
//*****************************************************************************
#INT_TIMER6
void timer_pause_isr (void)
{
   if (status.commutatorON == STATUS_COMMUTATOR_ON) commutator_set (ON);
   
   comparator_init ();
   clear_interrupt(INT_PULSE);
   enable_interrupts(INT_PULSE);
   
   disable_interrupts(INT_TIMER_PAUSE);    
}


//*****************************************************************************
//                 Таймер контроля наличия вх.импульса - 4мс  
//*****************************************************************************
#INT_TIMER3
void timer_noPulse_isr (void)
{
   status.inputOk = STATUS_INPUTOK_NOPULSE;
   adc_countOutMin = 0;
   SemaforSet(input_changed);
   disable_interrupts(INT_TIMER_NO_PULSE);    
}


//*****************************************************************************
//                         Системный таймер   
//                   Отсчитывает секундный интервал
//*****************************************************************************
#INT_TIMER0
void TIMER0_isr (void)
{  
   SemaforSet (timer_1s);
 
   if ((error_frame_ctl) || (error_ovflv_ctl))
   {
      setup_uart(0,PORT_UART);
      setup_uart(115200,PORT_UART);
      pointer_BufferRX_UART = 0;
   }
}


//*****************************************************************************
//                             Основной UART    
//*****************************************************************************
#INT_RDA
void  uart_rx_isr(void)                             
{  
   static char buffer[BUFFER_UART_SIZE];
   
   char symbol = fgetc (PORT_UART); 

   //Если не обработана ранее принятая команда:
//!   if (semafor.UART_Received) return; 
   
   if (symbol == '\r')
   {
//!      BufferRX_UART[pointer_BufferRX_UART] = 0; //Терминируем принятую строку.
      buffer[pointer_BufferRX_UART] = 0; //Терминируем принятую строку.
      strcpy (BufferRX_UART, buffer);
      pointer_BufferRX_UART = 0;
      SemaforSet (UART_Received);     
      return;
   }
   else
   {
      buffer[pointer_BufferRX_UART] = symbol;
      if (++pointer_BufferRX_UART >= BUFFER_UART_SIZE-1) 
      {
         pointer_BufferRX_UART = 0;
      }
   }        
}  //End of  RDA_isr **********************************************************




//*****************************************************************************                                         
//                              M   A   I   N             
//*****************************************************************************
void main()
{
   uint8_t  i, ret;
   
   static uint8_t  counter_extended_status = 0;
   static uint8_t  temperature_count = 0;       // period of temperature measurement, sec
   static uint8_t  attenuator_count = 0;        // period of attenuator ajustement (x temperature_count), sec
   static uint8_t  p_rev_fault_counter = 0;
   

  
   timers_init ();
   gpio_init ();
   
   setup_spi (SPI_MASTER/* | SPI_XMIT_L_TO_H*/ | SPI_L_TO_H | SPI_CLK_DIV_16);   // TMP
   setup_spi2(SPI_MASTER/* | SPI_XMIT_L_TO_H*/ | SPI_L_TO_H | SPI_CLK_DIV_16);   // ADC
   
   setup_uart(0,PORT_UART);
   setup_uart(115200,PORT_UART);

   
//*****   Инициализация   **************************************************   
   supply_emergency_shutdown();
   
   eeprom_init (MODE_DEFAULT); 
   attenuator_calc_koeff ();
   comparator_init ();
   voltmeter_init ();
    
   for (i=0;i<3;i++)
   {
      output_high (PIN_LED);
      delay_ms (100);
      output_low (PIN_LED);
      delay_ms (100);
   }
   
   SemaforSet(att_correctDet_OFF);
   SemaforReset(att_correctTem_OFF); 
   SemaforReset(UART_Received); 
   SemaforReset(timer_1s);     
   SemaforReset(fault_CMP_REV);
   SemaforReset(fault_BLOCK);
   SemaforReset(fault_5V);
   SemaforReset(fault_36V);
   SemaforReset(fault_PowOutMin);
   SemaforReset(fault_temperMax);
   SemaforReset(input_changed);
   SemaforReset(debug_mode);
   SemaforReset(flag_temperMax);
   
   
   status.inputOk       = STATUS_INPUTOK_NOPULSE;
   status.temperOk      = STATUS_TEMPEROK_NORMA;
   status.loadOk        = '1';
   status.voltageOk     = '0';
   status.outputOk      = '1';
   status.powerON       = STATUS_POWER_OFF;
   status.commutatorON  = STATUS_COMMUTATOR_OFF;
   
   if (eeprom.status_powerEN == STATUS_POWER_ENABLE)  status.powerEN = STATUS_POWER_ENABLE;
   else                                               status.powerEN = STATUS_POWER_DISABLE;
   
   if (eeprom.status_commutatorEN == STATUS_COMMUTATOR_ENABLE)  status.commutatorEN = STATUS_COMMUTATOR_ENABLE;
   else                                                         status.commutatorEN = STATUS_COMMUTATOR_DISABLE;

   status.zero          = 0;

   
   voltmeter_test();    // 2 раза!
   voltmeter_test();
   temperature_test();
   attenuator_correct ();  // first correction - without DET correction
   
   if (LOW_VOLTAGE_GOOD)   status.voltageOk  = '1';
   
   status_send(); 
  

//*****   Диспетчер прерываний:  *********************************************
   enable_interrupts(INT_PULSE);          // Обнаружение импульса
   enable_interrupts(INT_TIMER0);         // Системный таймер
   enable_interrupts(INT_RDA);            // UART
   enable_interrupts(INT_BLOCK);          // Пропадание питания затворов
   enable_interrupts(INT_CMP_REV);        // Превышение отраженной мощности
   enable_interrupts(GLOBAL); 

//***************************   СУПЕРЛУП    *********************************** 
   setup_wdt(WDT_ON);

   while(TRUE)
   {   
      restart_wdt();
      
//*****************  Авария: ОТРАЖЕННАЯ МОЩНОСТЬ   ****************************      
      if (semafor.fault_CMP_REV)
      {
         SemaforReset (fault_CMP_REV);
         
         commutator_set (OFF);
         
         status.loadOk  = '0';
         status.commutatorON = STATUS_COMMUTATOR_OFF;
         
         output_high (PIN_LED);
         if (!faultTimer)  
         {
            status_send();
            
            if (++p_rev_fault_counter < eeprom.Prev_num_attempts)
            {
               faultTimer = eeprom.Prev_timer_1;
            }
            else
            {
               p_rev_fault_counter = 0;
               faultTimer = eeprom.Prev_timer_2;
            }
         }
      }
      
//*****************  ИЗМЕНИЛАСЬ ВХОДНАЯ МОЩНОСТЬ   ****************************      
      if (semafor.input_changed)
      {
         SemaforReset (input_changed);
         
         switch (status.inputOk)
         {
            case STATUS_INPUTOK_TOOBIG:
               commutator_set (OFF);
               status.commutatorON = STATUS_COMMUTATOR_OFF;
               output_high (PIN_LED);
               if (!faultTimer)  
               {
                  status_send();
                  if (semafor.debug_mode)
                  {
                     strcpy (BufferRX_UART, "ADC?");
                     CommandProcessor ();
                  }
               }
               faultTimer = eeprom.fault_count;
               break;
               
            case STATUS_INPUTOK_NOPULSE:
               status.loadOk        = '1';
               SemaforSet(att_correctDet_OFF);
               attenuator_correct (); 

               status_send();
               if (semafor.debug_mode)
               {
                  strcpy (BufferRX_UART, "AGC?");
                  CommandProcessor ();
               }
            
               break;
               
            default:
               SemaforReset(att_correctDet_OFF);
               attenuator_correct ();
               if (!faultTimer)  
               {
                  status_send();
                  if (semafor.debug_mode)
                  {
                     strcpy (BufferRX_UART, "ADC?");
                     CommandProcessor ();
                     strcpy (BufferRX_UART, "AGC?");
                     CommandProcessor ();
                  }
               }
               break;
         }
      }      
      
//*****************  Авария: ПИТАНИЕ   ****************************************  
      if (semafor.fault_BLOCK)
      {
         SemaforReset (fault_BLOCK);
      
         supply_emergency_shutdown();
         
         status.voltageOk  = '0';
         status.powerON = '0'; 
         status.commutatorON = STATUS_COMMUTATOR_OFF;
         
         output_high (PIN_LED);
         if (!faultTimer)  
         {
            status_send();
            if (semafor.debug_mode)
            {
               strcpy (BufferRX_UART, "VLG?");
               CommandProcessor ();
            }
         }
         
         faultTimer = eeprom.fault_count;
      }   
      
      if (semafor.fault_5V)
      {
         SemaforReset (fault_5V);
      
         supply_emergency_shutdown();
         
         status.voltageOk  = '0';
         status.powerON = '0'; 
         status.commutatorON = STATUS_COMMUTATOR_OFF;
         
         output_high (PIN_LED);
         if (!faultTimer)  
         {
            status_send();
            if (semafor.debug_mode)
            {
               strcpy (BufferRX_UART, "VLG?");
               CommandProcessor ();
            }
         }
         
         faultTimer = eeprom.fault_count;
      }      
      
      if (semafor.fault_36V)
      {
         SemaforReset (fault_36V);
         
         if (status.powerEN == '1')
         {
            supply_emergency_shutdown();
            
            status.voltageOk  = '0';
            status.powerON = '0'; 
            status.commutatorON = STATUS_COMMUTATOR_OFF;
            
            output_high (PIN_LED);
            if (!faultTimer)  
            {
               faultTimer = eeprom.fault_count;
               status_send();
               if (semafor.debug_mode)
               {
                  strcpy (BufferRX_UART, "VLG?");
                  CommandProcessor ();
               }
            }
         }
      }       
      
//****************  Авария:  ЕСТЬ ВХОДНАЯ НЕТ ВЫХОДНОЙ МОЩНОСТИ   ********************
      if (semafor.fault_PowOutMin)
      {
         SemaforReset (fault_PowOutMin);
         status.outputOk = '0';
      
         if ((status.inputOk != STATUS_INPUTOK_NOPULSE) && (status.commutatorON == '1'))
         {
            supply_emergency_shutdown();
            
            status.powerON = STATUS_POWER_OFF;  
            status.commutatorON = STATUS_COMMUTATOR_OFF;
            
            output_high (PIN_LED);
            if (!faultTimer)  
            {
               status_send();
               if (semafor.debug_mode)
               {
                  strcpy (BufferRX_UART, "ADC?");
                  CommandProcessor ();
               }
            }
            
            faultTimer = eeprom.fault_count;
         }
      }  
      
//*****************  Авария: ТЕМПЕРАТУРА   ****************************************      
      if (semafor.fault_temperMax)
      {
         SemaforReset (fault_temperMax);
         supply_emergency_shutdown();
                
         status.temperOk  = STATUS_TEMPEROK_TOOBIG;
         status.powerON = STATUS_POWER_OFF;  
         status.commutatorON = STATUS_COMMUTATOR_OFF;
         
         output_high (PIN_LED);
         if (!faultTimer)  status_send(); 
         
         faultTimer = eeprom.fault_count;
      }      
      
//*******************   Секундный интервал   *********************************      
      if (semafor.timer_1s)
      {
         SemaforReset (timer_1s);
         
         voltmeter_test();
         
         if (temperature_count++ >= eeprom.temper_event)   // Temperature measurement
         {      
            temperature_count = 0;
            temperature_test();   
             
            if ((attenuator_count++) >= eeprom.atten_event)  // Attenuator ajustment
            {
               attenuator_count = 0;
               attenuator_correct ();   
            }
         }
         
         
         if (faultTimer)
         {
            if ((--faultTimer) == 0)
            {           
               voltmeter_test();
               temperature_test(); 
               
               if (status.powerEN == '1')
               {
                  if (VOLTAGE_GOOD)       status.voltageOk  = '1';
                  else                    status.voltageOk  = '0';
               }
               else
               {            
                  if (LOW_VOLTAGE_GOOD)   status.voltageOk  = '1';
                  else                    status.voltageOk  = '0';
               }
               
               status.outputOk   = '1';
               status.loadOk     = '1';
               adc_countOutMin = 0;
               
               if ((status.voltageOk == '1')                   && 
                   (status.temperOk == STATUS_TEMPEROK_NORMA)  && 
                   (status.inputOk != STATUS_INPUTOK_TOOBIG))
               {
                  output_low (PIN_LED);
                  
                  if (status.powerEN == STATUS_POWER_ENABLE)       
                  {
                     supply_manager ('A','1');
                     if (status.commutatorEN == '1')  commutator_manager ('1');
                  }
                  
                  if (semafor.debug_mode)   status_send_extended();
               }
               else
               {
                  output_high (PIN_LED);
                  faultTimer = eeprom.fault_count;
               }
            }
         }
         
         output_toggle (PIN_LED);
         
         // ******************************   Print status
         if ((period_extended_status) && (++counter_extended_status >= period_extended_status))
         {
//!            if (++counter_extended_status >= period_extended_status)
//!            {
               counter_extended_status = 0;
               status_send_extended();
//!            }
//!            else  status_send();
         }
         else status_send();
         
         output_toggle (PIN_LED);
         
      } 
      
//*****************   Принята команда   ***************************************     
      if (semafor.UART_Received)  //
      {
         SemaforReset (UART_Received);
      
         do
         {
            ret = CommandProcessor ();
            restart_wdt();
            
         }  while (ret == PORT_UART_RET_RECURSION);
         
//!         setup_uart(0,PORT_UART);
//!         setup_uart(115200,PORT_UART);
//!         pointer_BufferRX_UART = 0;
      }

   }//End Superloop.
   
}  //End main()



//***************************************************************************




//***************************************************************************
#include "drivers\PORT_UART\port_uart.c"
#include "drivers\ATTENUATOR\attenuator.c"
#include "drivers\ADC\adc.c"
#include "drivers\VOLTMETER\voltmeter.c"
#include "drivers\SUPPLY\supply.c"
#include "drivers\TEMPERATURE\temperature.c"
#include "drivers\GPIO\gpio.c"
#include "drivers\EEPROM\myeeprom.c"
#include "drivers\TIMERS\timers.c"
#include "drivers\COMPARATOR\comparator.c"
