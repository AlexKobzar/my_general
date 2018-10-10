//*****************************************************************************
//***************************   Контроль длительности   *********************
//*****************************************************************************
//!#INT_CCP3 HIGH//FAST //
#INT_CCP3 FAST //
void ccp3_interrupt ()
{
   static int16 curr_time_old = 0;
   
//**************************   Передний фронт импульса   *********************
   if (input_state (PIN_CCP3))
   {   
      if (C1OUT) commutator (OFF);
   
//Запуск таймера на макс.длительность 150мкс:   
      set_timer2(0);              
      clear_interrupt(INT_TIMER2);
      enable_interrupts(INT_TIMER2);
      
//Запуск таймера  для АЦП:   
      set_timer4(0);              
      clear_interrupt(INT_TIMER4);
      enable_interrupts(INT_TIMER4);      
      
//Расчет длительности ПАУЗЫ:   
      if (!interrupt_active (INT_TIMER1))   duration_pause =  CCP_3 - curr_time_old;  
      else                                  duration_pause =  (0xffff - curr_time_old) + CCP_3;
         
      curr_time_old = CCP_3;
      clear_interrupt (INT_TIMER1);
      
//Завершаем отработку ФРОНТА импульса      
      if (input_state(PIN_CCP3))          //Если импульс НЕ закончился:
      {      
         setup_ccp3(CCP_CAPTURE_FE);      
      }
      else                                //Если импульс ЗАКОНЧИЛСЯ:
      {    
//!         disable_interrupts(INT_TIMER4); 
         disable_interrupts(INT_TIMER2);
         
         duration_pulse = 12; //6us
         curr_time_old = get_timer1();
         
         commutator (ON);

         setup_ccp3(CCP_CAPTURE_RE);      
      }
   }
//**************************   Задний фронт импульса   *********************   
   else
   {
//Расчет длительности ИМПУЛЬСА:   
      if (!interrupt_active (INT_TIMER1))   duration_pulse =  CCP_3 - curr_time_old;  
      else                                  duration_pulse =  (0xffff - curr_time_old) + CCP_3;
      
      curr_time_old = CCP_3;
      clear_interrupt (INT_TIMER1);
      
//!      disable_interrupts(INT_TIMER4);
      disable_interrupts(INT_TIMER2); 
      commutator (ON);
      
      if (!input_state (PIN_CCP3))  //Пауза продолжается
      {       
         setup_ccp3(CCP_CAPTURE_RE);      //Ждем начала импульса
      }
      else
      {
         duration_pause = 12; //6us
         curr_time_old = get_timer1(); 
         
//Запуск таймера  для АЦП:   
         set_timer4(0);              
         clear_interrupt(INT_TIMER4);
         enable_interrupts(INT_TIMER4);                           
            
//Запуск таймера на макс.длительность 150мкс:   
         set_timer2(0);              
         clear_interrupt(INT_TIMER2);
         enable_interrupts(INT_TIMER2);
         
         setup_ccp3(CCP_CAPTURE_FE);      //Ждем конца импульса
         
      }
   }
}


//*****************************************************************************
//************   Таймер превышения длительности импульса   *******************
//*****************************************************************************
#INT_TIMER2
void TIMER2_isr (void)
{
   if (input_state (PIN_CCP3)) //Импульс все еще не закончился
   {
      commutator (OFF);
      disable_interrupts(INT_TIMER2);
   }
   
}


//**********    Подготовка к работе:        *******************
//На входе модуля - высокий уровень - импульс. ВКЛ.аварию и ждем паузу:
   if (input_state(PIN_CCP3))  
   {
//!      flag.pulse_state = 1;
//!      OUT_Alarm();
      setup_ccp3(CCP_CAPTURE_FE);      //Ждем конца импульса
      
   }
//На входе модуля - низкий уровень - импульс. ВЫкл.аварию и ждем импульс:   
   else
   {
//!      flag.pulse_state = 0;
//!      OUT_Work();
      setup_ccp3(CCP_CAPTURE_RE);      //Ждем начала импульса
   }
