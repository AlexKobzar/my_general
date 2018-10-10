
#ifndef MAIN_H   
#define MAIN_H

#include "drivers\FUSE.h" 

#define ON 1
#define OFF 0

#use fast_io (A)
#use fast_io (B)
#use fast_io (C)
#use fast_io (D)
#use fast_io (E)


//*****************************************************************************
//             Порт SPI
//****************************************************************************
#USE SPI (FORCE_HW, MASTER, SPI2,  MODE=0, BITS=8, STREAM=SPI_2)  // ADC
#USE SPI (FORCE_HW, MASTER, SPI1,  MODE=0, BITS=8, STREAM=SPI_1)  // TMP + ATT



#endif // MAIN_H
