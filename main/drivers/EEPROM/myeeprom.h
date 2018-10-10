#ifndef MYEEPROM_H   
#define MYEEPROM_H 

#include "drivers\ATTENUATOR\attenuator.h"

#define MODE_DEFAULT 0
#define MODE_FORSED  1


#define DET1   0  // Индексы массива офсетов в eeprom
#define DET2   1
#define DET3   2
#define DET4   3
#define DET5   4

typedef struct
{
   float    A;
   float    B;
   float    C;
   
} det_koef_t;

typedef struct 
{
   uint8_t     flagFirstStart;
   uint8_t     cmp_porog;           // Порог входного компаратора С2 (обнаружение импульса)
   uint8_t     volt_36_min;         // Мин.порог  ADC +36V
   uint8_t     volt_36_max;         // Макс.порог ADC +36V
   uint8_t     volt_5_min;          // Мин.порог  ADC +5V/-5V
   uint8_t     volt_5_max;          // Макс.порог ADC +5V/-5V
   uint16_t    PowerOutMin;         // Минимальная вЫходная мощность 
   uint16_t    PowerOutMinCount;    // Счетчик импульсов Мин.вЫходной мощности
   uint8_t     attenuator_start;    // Стартовое значение аттенюатора
   int8_t      temper_max;          // Максимальная темпреатура
   int8_t      temper_start;        // Стартовая темпреатура
   float       adc_vref;            // Опорное напряжение внешнего АЦП         17

   float       reserve;           // Reserve
   uint8_t     temper_event;
   uint8_t     atten_event;
   uint8_t     fault_count;
   float       volt_5_shift;                                                  // 28
   
   det_koef_t  det_koef[5];         // Y = A*X +B                             // 88
   
   float       volt_36_Ka;
   float       volt_36_Kb;                                                    // 96
   
   uint16_t    serialnum;                                                     //97
   
   uint8_t     Prev_timer_1;        // Interval between short attempts
   uint8_t     Prev_timer_2;        // Interval between long attempts
   uint8_t     Prev_num_attempts;   // Number of short attempts               100
   
   uint16_t    Pin_ALPmin;
   uint16_t    Pin_ALPmax;
   uint16_t    Pin_TZmin; 
   uint16_t    Pin_TZmax;                                                     // 108
   
   attenuator_temper_table_t  temper_table;                                   // 128 + 108 = 236
   
   char        status_powerEN;
   char        status_commutatorEN;
   
   char        fin;

}  eeprom_t;

eeprom_t   *eepromAddr = 0x00;
eeprom_t   eeprom;


void eeprom_read_object(int ee_addr, void *obj_p, size_t obj_size);
void eeprom_write_object(int ee_addr, void *obj_p, size_t obj_size);

void eeprom_init (uint8_t mode);




#endif
