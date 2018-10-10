#device ADC=8

#define LOADER_END   0x7FF

#use delay(oscillator=16Mhz, clock=64Mhz, restart_wdt)

#FUSES PRIMARY_ON               //Primary clock is always enabled
#FUSES FCMEN                    //Fail-safe clock monitor enabled
#FUSES IESO                     //Internal External Switch Over mode enabled
#FUSES PLLEN          //1 =  4 x PLL always enabled, Oscillator multiplied by 4 Õ≈ –¿¡Œ“¿≈“!¬ÒÂ„‰‡ NOPLLEN
#FUSES HSM            //0011 = HS oscillator (medium power, 4 MHz-16 MHz
#FUSES PUT                      //Power Up Timer
#FUSES NOBROWNOUT               //No brownout reset
#FUSES BORV19                   //Brownout reset at 1.9V
#FUSES   WDT128        //  =Watch Dog Timer uses 1:2048 Postscale
#FUSES   WDT_SW          //10 = WDT controlled by the SWDTEN bit
#FUSES NOPBADEN                 //PORTB pins are configured as digital I/O on RESET
#FUSES CCP3E0                   //CCP3 input/output multiplexed with RE0
#FUSES HFOFST                   //High Frequency INTRC starts clocking CPU immediately
#FUSES TIMER3C0                 //Timer3 Clock In is on pin C0
#FUSES CCP2D2                   //CCP2 input/output multiplexed with RD2
#FUSES MCLR                     //Master Clear pin enabled
#FUSES STVREN                   //Stack full/underflow will cause reset
#FUSES NOLVP                    //No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O
#FUSES NOXINST                  //Extended set extension and Indexed Addressing mode disabled (Legacy mode)++++++++++++++++++++++  NO
#FUSES NOPROTECT                //Code not protected from reading
#FUSES CPB                    //Boot Block code protection
#FUSES NOCPD                    //No EE protection
#FUSES NOWRT                    //Program memory not write protected
#FUSES NOWRTC                   //Configuration registers not write protected
#FUSES WRTB                   //Boot block write protected
#FUSES NOWRTD                   //Data EEPROM not write protected
#FUSES NOEBTR                   //Memory not protected from table reads
#FUSES NOEBTRB                  //Boot block not protected from table reads



//*****************************************************************************
//              Fuse
//*****************************************************************************
                                     
//!//CONFIG1L: 0x00
//!//CONFIG1H: 0xF3 
//!#FUSES   IESO           //1 = Oscillator Switchover mode enabled
//!#FUSES   FCMEN          //1 = Fail-Safe Clock Monitor enabled
//!#FUSES   PRIMARY        //1 = Primary Clock is always enabled+++++++++++++++++++++++++++++++++++++++++++++++++
//!#FUSES   PLLEN          //1 =  4 x PLL always enabled, Oscillator multiplied by 4 Õ≈ –¿¡Œ“¿≈“!¬ÒÂ„‰‡ NOPLLEN
//!#FUSES   HSM            //0011 = HS oscillator (medium power, 4 MHz-16 MHzÕ≈ –¿¡Œ“¿≈“! ¬ÒÂ„‰‡ ECH_IO 
//!
//!//CONFIG2L: 0b 0000 0110 - 0x06
//!#FUSES   BORV29         //00 = VBOR set to 2.9V nominal
//!#FUSES   BROWNOUT       //11 = Brown-out Reset enable
//!#FUSES   PUT            //0 = PWRT enabled
//!//CONFIG2H: 0b 0001 1100 - 0x1C
//!#FUSES   WDT1024        //  =Watch Dog Timer uses 1:2048 Postscale
//!#FUSES   WDT_SW          //10 = WDT controlled by the SWDTEN bit
//!//CONFIG3L: 0x00
//!//CONFIG3H: 0b 1011 1101 - 0xBD
//!#FUSES   MCLR           //1 = MCLR pin enabled; RE3 input pin disabled
//!#FUSES   CCP2B5         //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!#FUSES   TIMER3C0       //
//!#FUSES   HFOFST         //1 = HFINTOSC starts clocking the CPU without waiting for the oscillator to stabilize
//!#FUSES   CCP3B5         //1 = CCP3 input/output is multiplexed with RB5
//!#FUSES   NOPBADEN       //0 = PORTB pins are configured as digital I/O on RESET
//!//CONFIG4L:0b 1000 0001 - 0x81
//!#FUSES   NODEBUG        //1 = Background debugger disabled, RB6 and RB7 configured as general purpose I/O pins
//!#FUSES   XINST          //1 = Instruction set extension and Indexed Addressing mode enabled  Õ≈ –¿¡Œ“¿≈“ - 0
//!#FUSES   NOLVP          //No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O 
//!#FUSES   STVREN         //1 = Stack full/underflow will cause Reset
//!//CONFIG4H: 0x00 
//!//CONFIG5L: 0x0F     
//!#FUSES   NOPROTECT
//!//CONFIG5H:0xC0 
//!#FUSES   NOCPB
//!#FUSES   NOCPD
//!//CONFIG6L: 0x0F
//!#FUSES  NOWRT          //1111 = Block 0-3 not write-protected 
//!//CONFIG6H: 0xE0
//!#FUSES  NOWRTC
//!#FUSES  NOWRTB
//!#FUSES  NOWRTD
//!//CONFIG7L: 0x0F
//!#FUSES   NOEBTR         //Table read NO protected
//!//CONFIG7H: 0x40
//!#FUSES   NOEBTRB        //Boot Block Table read NO protection


//*****************************************************************************
//              œÓÚ CP2102
//***************************************************************************** 
#use rs232(baud=115200,parity=N,UART1,bits=8,stream=PORT_UART) 

