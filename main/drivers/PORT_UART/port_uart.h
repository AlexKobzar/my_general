#ifndef PORT_UART_H   
#define PORT_UART_H   

#bit error_frame_ctl = GETENV("BIT:FERR")
#bit error_ovflv_ctl = GETENV("BIT:OERR")

#define PORT_UART_RET_OK         0
#define PORT_UART_RET_RECURSION  1


//!#define reset_UART()   setup_uart(0,PORT_UART);setup_uart(115200,PORT_UART)
  

#define BUFFER_UART_SIZE  128
uint8_t  pointer_BufferRX_UART = 0;
uint8_t  BufferRX_UART[BUFFER_UART_SIZE+2];


signed int8 strncmp_rom(rom char *s1, char *s2, size_t n);
unsigned int atoi_b16_my(char *s);

uint8_t CommandProcessor(void);



#endif   
