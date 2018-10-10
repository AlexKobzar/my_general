#ifndef GPIO_H   
#define GPIO_H

#define  PIN_EN2         PIN_A0  // ���� �������
#define  PIN_CMP_DET1    PIN_A1  // CMP_DET1 (input comparator 2)
#define  PIN_NU3         PIN_A2  // ����� Vref
#define  PIN_EN1         PIN_A3  // ���� �������
#define  PIN_PG          PIN_A4  // PowerGood
#define  PIN_TP1         PIN_A5  // TP1 (out comparator 2)
#define  PIN_OSC1        PIN_A6
#define  PIN_OSC2        PIN_A7 

#define  PIN_BL         PIN_B0   // ������ BL (����)
#define  PIN_CMP_REV    PIN_B1   // ������ CMP_REV (���� � �������� ����������� ���������� ��������)
#define  PIN_ON         PIN_B2   // ������ ON (���� �����)
#define  PIN_SW_AB      PIN_B3   // ������ SW_A/B (����� ���������� ������������)
#define  PIN_EN6        PIN_B4   // ���� �������
#define  PIN_EN5        PIN_B5   // ���� �������
#define  PIN_EN4        PIN_B6   // ���� �������
#define  PIN_EN3        PIN_B7   // ���� ������� 

#define  PIN_LED        PIN_C0   // LED_WORK
#define  PIN_TMP_CS     PIN_C1   // TMP  - CS
#define  PIN_ATT_CS     PIN_C2   // ATT  - CS
#define  PIN_SCK        PIN_C3   // SPI1 - SCK
#define  PIN_MISO       PIN_C4   // SPI1 - MISO
#define  PIN_MOSI       PIN_C5   // SPI1 - MOSI
#define  PIN_CPUTX      PIN_C6   // UART 
#define  PIN_CPURX      PIN_C7   // UART

#define  PIN_ADC_SCK    PIN_D0   // ADC_SCK
#define  PIN_ADC_SDI    PIN_D1   // ADC_SDI
#define  PIN_ADC_CS     PIN_D2   // ADC_CS
#define  PIN_NU1        PIN_D3   // Not used
#define  PIN_ADC_SDO    PIN_D4   // ADC_SDO (Not used)
#define  PIN_4051_A     PIN_D5   // 4051_A
#define  PIN_4051_B     PIN_D6   // 4051_B
#define  PIN_4051_C     PIN_D7   // 4051_C

#define  PIN_NU2        PIN_E0   // 
#define  PIN_ADC_5V     PIN_E1   // ADC +/-5V
#define  PIN_ADC_36V    PIN_E2   // ADC +36V
#define  PIN_MCLR       PIN_E3   // RESET



uint8_t  bufPortA;
uint8_t  bufPortB;
uint8_t  bufPortC;
uint8_t  bufPortD;
//!uint8_t  bufPortE;

// ���������:
#define  BUF_PORT_LED      bufPortC
#define  outputPort_LED    output_C
#define  LED_MASK          (1<<0)   

// ������ Power Good
#define BUF_PORT_PG        bufPortA
#define outputPort_PG      output_A
#define PG_MASK            (1<<4)  

// ������ SW_A/B
#define BUF_PORT_SW_AB     bufPortB
#define outputPortSW_AB    output_B
#define SW_AB_MASK         (1<<3) 

// TMP122:
#define  BUF_PORT_TMP      bufPortC
#define  outputPort_TMP    output_C
#define  TMP_CS_MASK       (1<<4)  
#define  TMP_SCK_MASK      (1<<3) 

void gpio_init (void);


#endif 
