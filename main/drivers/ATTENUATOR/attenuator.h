#ifndef ATTENUATOR_H   
#define ATTENUATOR_H  


#define TEMPER_POINT_MAX 64


uint8_t  attenuator_current;
float attenuator_A, attenuator_B;



typedef struct
{
   int8_t   temper;
   int8_t   att_corr_unit;
   
}  temper_point_t;

typedef struct
{
   uint8_t           num_of_points;
   temper_point_t    point[TEMPER_POINT_MAX];

}  attenuator_temper_table_t;


void attenuator_set (uint8_t value);
void attenuator_correct (void);
void attenuator_calc_koeff (void);





#endif  
