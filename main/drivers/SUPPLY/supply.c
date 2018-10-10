#include <supply.h>



#define power_good_on()    BUF_PORT_PG |= PG_MASK;outputPort_PG (BUF_PORT_PG)  
#define power_good_off()   BUF_PORT_PG &= (~PG_MASK);outputPort_PG (BUF_PORT_PG)


void supply_emergency_shutdown()  
{
   output_A (0);         
   output_B (0);       
   
   bufPortA       = 0;        // вЫключить EN1, EN2, Снять Forse Voltage 
   bufPortB       = 0;        // вЫключить EN3,EN4,EN5,EN6, SW_A/B
  
   en_state = 0;
}                                 
                                                             
void supply_manager (char num, char state)
{    
   char status_inputOk_buffer = status.inputOk;

   int1 sw = (state == '0') ? 0 : 1;

   if (num == 'A')    
   {
      if (state == '0')
      {
         status.powerON = '0';
         status.powerEN = '0';
         status.commutatorON = '0';
      }
      else if (state == '1')
      {
         status.powerEN = '1';
         
         if ((faultTimer) || (status.temperOk != STATUS_TEMPEROK_NORMA) || (status.voltageOk == '0'))   
         {
            faultTimer = eeprom.fault_count;
            return;
         }

         status.powerON = '1';
         if (status.commutatorEN == '1')   status.commutatorON = '1';
      }
      else return;
   }

   switch (num)   
   {
      case '1':   EN1 = EN1_STATE = sw; break;
      case '2':   EN2 = EN2_STATE = sw; break;
      case '3':   EN3 = EN3_STATE = sw; break;
      case '4':   EN4 = EN4_STATE = sw; break;
      case '5':   EN5 = EN5_STATE = sw; break;
      case '6':   EN6 = EN6_STATE = sw; break;
      
      case 'P':   
         EN1 = EN2 = EN3 = EN4 = sw; 
         EN1_STATE = EN2_STATE = EN3_STATE = EN4_STATE = sw; 
         break;
      case 'A':   
         EN1 = EN2 = EN3 = EN4 = EN5 = EN6 = sw; 
         EN1_STATE = EN2_STATE = EN3_STATE = EN4_STATE = EN5_STATE = EN6_STATE = sw; 
         break;
         
      default: break;
   }
   
   disable_interrupts(GLOBAL); 
   
   commutator_set(OFF);

   
   if ((state == '0') && (num == 'A'))  power_good_off(); //Если команда "Выключить все", сначала подать ForseVoltage.

   delay_ms (50);
   output_A (bufPortA);
   output_B (bufPortB); 
   
   restart_wdt();
   
   delay_ms (10);             
   if (state ==  '1')   power_good_on();   //Если команда - Включение, снять ForseVoltage
   delay_ms (10);
   
//!   status.inputOk       = STATUS_INPUTOK_NOPULSE;
//!   status.temperOk      = '0';
//!   status.loadOk        = '1';
//!   status.voltageOk     = '0';
//!   status.outputOk      = '1';
//!   status.powerON       = '0';
//!   status.commutatorON  = '0';
//!   status.powerEN       = '0';
//!   status.commutatorEN  = '0';
//!   status.zero          = 0;
//!
//!   voltmeter_test();
//!   temperature_test();
//!   attenuator_correct ();  // first correction - without DET correction
//!   
//!   if (VOLTAGE_GOOD)       status.voltageOk  = '1';
//!   if (TEMPERATURE_GOOD)   status.temperOk   = '1';
   
   status.inputOk = status_inputOk_buffer;
   SemaforReset(input_changed);
   
   clear_interrupt_all();
   enable_interrupts(GLOBAL); 
}




//**********************   К О М М У Т А Т О Р   *****************************
void commutator_set (uint8_t sw)  //
{
   if ((sw) && (status.commutatorON == STATUS_COMMUTATOR_ON))     
   {
      BUF_PORT_SW_AB |= SW_AB_MASK;       //ON
   }
   else       
   {
      BUF_PORT_SW_AB &= (~SW_AB_MASK);    //OFF
   }
      
   outputPortSW_AB (BUF_PORT_SW_AB);
}

void commutator_manager (char state)
{
   status.commutatorEN = state;

   if (status.powerON ==  '0')   
   {
      status.commutatorON = STATUS_COMMUTATOR_OFF;
   }
   else                         
   {      
      if (!faultTimer)  status.commutatorON = state;
      else              status.commutatorON = STATUS_COMMUTATOR_OFF;
   }
}


