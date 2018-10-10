#ifndef STATUS_H   
#define STATUS_H 

// Статус контроллера - Слово состояния:
struct 
{
   char     inputOk;          // Входные импульсы        Ок.
   char     temperOk;         // Температура модуля      Ок
   char     loadOk;           // Нагрузка (КСВ)          Ок
   char     voltageOk;        // +.-5В, 36В, BL          Ок
   char     outputOk;         // вЫходная мощность       Ок
   char     powerON;          // power ON
   char     commutatorON;     // rf_switch ON
   char     powerEN;          // power enable
   char     commutatorEN;     // rf_switch enable
   uint8_t  zero;             // Терминатор строки

}  status;

typedef enum
{
   STATUS_INPUTOK_TOOBIG      = '0',
   STATUS_INPUTOK_NORMA       = '1',
   STATUS_INPUTOK_NOPULSE     = '2',
   STATUS_INPUTOK_LOWER       = '3',
   STATUS_INPUTOK_UPPER       = '4',
   STATUS_INPUTOK_TOOLOW      = '5'
   
}  status_inputOk_t;

typedef enum
{
   STATUS_TEMPEROK_TOOBIG      = '0',
   STATUS_TEMPEROK_NORMA       = '1',
   STATUS_TEMPEROK_WARNING     = '2',
   STATUS_TEMPEROK_NOSTART     = '3'
   
}  status_temperOk_t;

typedef enum
{
   STATUS_COMMUTATOR_DISABLE   = '0',
   STATUS_COMMUTATOR_ENABLE    = '1' 
   
}  status_commutatorEN_t;

typedef enum
{
   STATUS_COMMUTATOR_OFF   = '0',
   STATUS_COMMUTATOR_ON    = '1' 
   
}  status_commutatorON_t;

typedef enum
{
   STATUS_POWER_DISABLE   = '0',
   STATUS_POWER_ENABLE    = '1' 
   
}  status_powerEN_t;

typedef enum
{
   STATUS_POWER_OFF   = '0',
   STATUS_POWER_ON    = '1' 
   
}  status_powerON_t;


#define status_send()   fprintf(PORT_UART,"STA %s %d\r",&status, temper_tmp122)

#define status_send_extended()   fprintf(PORT_UART,                           \
                                       "STE %s "                              \
                                       "%Lu %Lu %Lu %Lu %Lu "                 \
                                       "%u %d %u %u %u\r",                        \
                                       &status,                               \
                                       adc_value[0], adc_value[1], adc_value[2], adc_value[3], adc_value[4],   \
                                       attenuator_current, temper_tmp122, volt_36v, volt_5v, BL_FAULT)    

#endif
