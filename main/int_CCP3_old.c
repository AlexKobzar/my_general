//*****************************************************************************
//***************************   �������� ������������   *********************
//*****************************************************************************
//!#INT_CCP3 HIGH//FAST //
#INT_CCP3 FAST //
void ccp3_interrupt ()
{
   static int16 curr_time_old = 0;
   
//**************************   �������� ����� ��������   *********************
   if (input_state (PIN_CCP3))
   {   
      if (C1OUT) commutator (OFF);
   
//������ ������� �� ����.������������ 150���:   
      set_timer2(0);              
      clear_interrupt(INT_TIMER2);
      enable_interrupts(INT_TIMER2);
      
//������ �������  ��� ���:   
      set_timer4(0);              
      clear_interrupt(INT_TIMER4);
      enable_interrupts(INT_TIMER4);      
      
//������ ������������ �����:   
      if (!interrupt_active (INT_TIMER1))   duration_pause =  CCP_3 - curr_time_old;  
      else                                  duration_pause =  (0xffff - curr_time_old) + CCP_3;
         
      curr_time_old = CCP_3;
      clear_interrupt (INT_TIMER1);
      
//��������� ��������� ������ ��������      
      if (input_state(PIN_CCP3))          //���� ������� �� ����������:
      {      
         setup_ccp3(CCP_CAPTURE_FE);      
      }
      else                                //���� ������� ����������:
      {    
//!         disable_interrupts(INT_TIMER4); 
         disable_interrupts(INT_TIMER2);
         
         duration_pulse = 12; //6us
         curr_time_old = get_timer1();
         
         commutator (ON);

         setup_ccp3(CCP_CAPTURE_RE);      
      }
   }
//**************************   ������ ����� ��������   *********************   
   else
   {
//������ ������������ ��������:   
      if (!interrupt_active (INT_TIMER1))   duration_pulse =  CCP_3 - curr_time_old;  
      else                                  duration_pulse =  (0xffff - curr_time_old) + CCP_3;
      
      curr_time_old = CCP_3;
      clear_interrupt (INT_TIMER1);
      
//!      disable_interrupts(INT_TIMER4);
      disable_interrupts(INT_TIMER2); 
      commutator (ON);
      
      if (!input_state (PIN_CCP3))  //����� ������������
      {       
         setup_ccp3(CCP_CAPTURE_RE);      //���� ������ ��������
      }
      else
      {
         duration_pause = 12; //6us
         curr_time_old = get_timer1(); 
         
//������ �������  ��� ���:   
         set_timer4(0);              
         clear_interrupt(INT_TIMER4);
         enable_interrupts(INT_TIMER4);                           
            
//������ ������� �� ����.������������ 150���:   
         set_timer2(0);              
         clear_interrupt(INT_TIMER2);
         enable_interrupts(INT_TIMER2);
         
         setup_ccp3(CCP_CAPTURE_FE);      //���� ����� ��������
         
      }
   }
}


//*****************************************************************************
//************   ������ ���������� ������������ ��������   *******************
//*****************************************************************************
#INT_TIMER2
void TIMER2_isr (void)
{
   if (input_state (PIN_CCP3)) //������� ��� ��� �� ����������
   {
      commutator (OFF);
      disable_interrupts(INT_TIMER2);
   }
   
}


//**********    ���������� � ������:        *******************
//�� ����� ������ - ������� ������� - �������. ���.������ � ���� �����:
   if (input_state(PIN_CCP3))  
   {
//!      flag.pulse_state = 1;
//!      OUT_Alarm();
      setup_ccp3(CCP_CAPTURE_FE);      //���� ����� ��������
      
   }
//�� ����� ������ - ������ ������� - �������. ����.������ � ���� �������:   
   else
   {
//!      flag.pulse_state = 0;
//!      OUT_Work();
      setup_ccp3(CCP_CAPTURE_RE);      //���� ������ ��������
   }
