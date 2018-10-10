#include "timers.h"


void timers_init (void)
{
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_256);   // Таймер системный,    1024 ms overflow
   setup_timer_1(T1_DISABLED);                  //                  
   setup_timer_2(T2_DIV_BY_16,72,2);            // 75,0 us overflow, 150 us interrupt
   setup_timer_3(T3_INTERNAL | T3_DIV_BY_8);    // Таймер контроля наличия вх.импульса - 32,7мс
   setup_timer_5(T5_DISABLED | T5_DIV_BY_1);    //
   setup_timer_6(T6_DIV_BY_16,200,3);           //Таймер защитного интервала - 600мкс
}
