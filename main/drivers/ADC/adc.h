#ifndef ADC_H   
#define ADC_H

#ifdef PROTEUS
   uint16_t proteus_adc[5] = {2600, 2700, 2800, 2900, 3100};
#endif

#define A_4051 ((1<<5)|0x0F)
#define B_4051 ((1<<6)|0x0F)
#define C_4051 ((1<<7)|0x0F)

//DET1      X7    ABC
//DET2      X4    C
//DET3      X1    A
//DET4      X6    BC
//DET5      X2    B

//Код на 4051 для выбора канала детектора
int8     adc_channel_encoder[5] = {A_4051|B_4051|C_4051, C_4051, A_4051, B_4051|C_4051, B_4051};  
int16    adc_value[5] = {0};
uint16_t adc_countOutMin = 0;




#inline void adc_mesure (void);
uint16_t adc_test (uint8_t channel);


#endif  
