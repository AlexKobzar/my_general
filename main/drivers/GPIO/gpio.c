#include <gpio.h>



void gpio_init (void)
{
   set_tris_A           (1<<1 | 1<<2 | 1<<6 | 1<<7);   //pin6 & pin7 - crystal
   bufPortA = 0;
   output_A(bufPortA);     //

   set_tris_B           (1<<0 | 1<<1 | 1<<2); //
   bufPortB = 0;
   output_B(bufPortB);   
          
   set_tris_C           (1<<4 | 1<<6 | 1<<7);  //
   bufPortC =           (1<<1 | 1<<5 | 1<<6 | 1<<7);  //
   output_C(bufPortC); 

   set_tris_D           (1<<1); //
   bufPortD =           (1<<2);
   output_D(bufPortD);    

   set_tris_E           (1<<1 | 1<<2 | 1<<3);
   
//!   setup_adc_ports(sAN6 | sAN7 | VSS_VDD);
   setup_adc_ports(sAN6 | sAN7 | VSS_FVR);
}


