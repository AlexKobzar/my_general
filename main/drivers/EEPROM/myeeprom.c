#include "myeeprom.h"

void eeprom_read_object(int ee_addr, void *obj_p, size_t obj_size)
{
  unsigned char *p = obj_p;

  while (obj_size--) {
      *p++ = read_eeprom (ee_addr++);
  }
}

void eeprom_write_object(int ee_addr, void *obj_p, size_t obj_size)
{
  unsigned char *p = obj_p;

  while (obj_size--) {
      write_eeprom (ee_addr++, *p++);
  }
}


void eeprom_init (uint8_t mode)
{
   eeprom.flagFirstStart   = 0x55;  
   eeprom.cmp_porog        = 6;
   eeprom.volt_36_min      = 50;
   eeprom.volt_36_max      = 250;
   eeprom.volt_5_min       = 50;
   eeprom.volt_5_max       = 250;
   eeprom.PowerOutMin      = 3000;     
   eeprom.PowerOutMinCount = 20; 
   eeprom.attenuator_start = 52;
   eeprom.temper_max       = 75;  
   eeprom.temper_start     = 50;    // пправильно
   eeprom.adc_vref         = 2.47;
   eeprom.reserve          = 0; // ++++++++++++++++++++++++++++++++++++++++++++++++++++
   eeprom.temper_event     = 2;
   eeprom.atten_event      = 2;
   eeprom.fault_count      = 5;
   eeprom.volt_5_shift     = 2.56;
   
   eeprom.det_koef[0].A   = 0.019;
   eeprom.det_koef[0].B   = -40.82;
   eeprom.det_koef[0].C   = 19.4;
   eeprom.det_koef[1].A   = 0.02;
   eeprom.det_koef[1].B   = -13.0;
   eeprom.det_koef[1].C   = 50;
   eeprom.det_koef[2].A   = 0.0205;
   eeprom.det_koef[2].B   = -16.3;
   eeprom.det_koef[2].C   = 53.65;
   eeprom.det_koef[3].A   = 0.025;
   eeprom.det_koef[3].B   = -22.8;
   eeprom.det_koef[3].C   = 73;
   eeprom.det_koef[4].A   = 0.0217;
   eeprom.det_koef[4].B   = -13.85;
   eeprom.det_koef[4].C   = 73;
   
   eeprom.volt_36_Ka      = 0.162072;
   eeprom.volt_36_Kb      = 0.117029;
 
   eeprom.serialnum       = 1;

   eeprom.temper_table.num_of_points = 24;
   
   eeprom.temper_table.point[0].temper = -40;
   eeprom.temper_table.point[0].att_corr_unit = 8;
   eeprom.temper_table.point[1].temper = -35;
   eeprom.temper_table.point[1].att_corr_unit = 8;
   eeprom.temper_table.point[2].temper = -30;
   eeprom.temper_table.point[2].att_corr_unit = 8;   
   eeprom.temper_table.point[3].temper = -25;
   eeprom.temper_table.point[3].att_corr_unit = 8;  
   eeprom.temper_table.point[4].temper = -20;
   eeprom.temper_table.point[4].att_corr_unit = 8;  
   eeprom.temper_table.point[5].temper = -15;
   eeprom.temper_table.point[5].att_corr_unit = 8;  
   eeprom.temper_table.point[6].temper = -10;
   eeprom.temper_table.point[6].att_corr_unit = 7; 
   eeprom.temper_table.point[7].temper = -5;
   eeprom.temper_table.point[7].att_corr_unit = 6;   
   eeprom.temper_table.point[8].temper = 0;
   eeprom.temper_table.point[8].att_corr_unit = 5;  
   eeprom.temper_table.point[9].temper = 5;
   eeprom.temper_table.point[9].att_corr_unit = 4; 
   eeprom.temper_table.point[10].temper = 10;
   eeprom.temper_table.point[10].att_corr_unit = 3;   
   eeprom.temper_table.point[11].temper = 15;
   eeprom.temper_table.point[11].att_corr_unit = 2;    
   eeprom.temper_table.point[12].temper = 20;
   eeprom.temper_table.point[12].att_corr_unit = 1;    
   eeprom.temper_table.point[13].temper = 25;
   eeprom.temper_table.point[13].att_corr_unit = 0;  
   eeprom.temper_table.point[14].temper = 30;
   eeprom.temper_table.point[14].att_corr_unit = -1;  
   eeprom.temper_table.point[15].temper = 35;
   eeprom.temper_table.point[15].att_corr_unit = -2; 
   eeprom.temper_table.point[16].temper = 40;
   eeprom.temper_table.point[16].att_corr_unit = -3; 
   eeprom.temper_table.point[17].temper = 45;
   eeprom.temper_table.point[17].att_corr_unit = -4; 
   eeprom.temper_table.point[18].temper = 50;
   eeprom.temper_table.point[18].att_corr_unit = -5;  
   eeprom.temper_table.point[19].temper = 55;
   eeprom.temper_table.point[19].att_corr_unit = -6;    
   eeprom.temper_table.point[20].temper = 60;
   eeprom.temper_table.point[20].att_corr_unit = -7; 
   eeprom.temper_table.point[21].temper = 65;
   eeprom.temper_table.point[21].att_corr_unit = -8;       
   eeprom.temper_table.point[22].temper = 70;
   eeprom.temper_table.point[22].att_corr_unit = -9;       
   eeprom.temper_table.point[23].temper = 75;
   eeprom.temper_table.point[23].att_corr_unit = -11;    
   
   eeprom.Prev_timer_1        = 1; 
   eeprom.Prev_timer_2        = 10;
   eeprom.Prev_num_attempts   = 10; 
   
   eeprom.Pin_ALPmin          = 2359;  // 
   eeprom.Pin_TZmin           = 2517;
   eeprom.Pin_TZmax           = 2727;
   eeprom.Pin_ALPmax          = 2833;
   
   eeprom.status_powerEN       = '0';
   eeprom.status_commutatorEN  = '0';
   
   eeprom.fin                 = 'F';
   
   
   if ((read_eeprom (&eepromAddr->flagFirstStart) != 0x55) || (mode == MODE_FORSED))
   {
      eeprom_write_object(&eepromAddr->flagFirstStart, &eeprom, sizeof (eeprom_t));
   }
   else
   {
      eeprom_read_object(&eepromAddr->flagFirstStart, &eeprom, sizeof (eeprom_t));
   }
}
