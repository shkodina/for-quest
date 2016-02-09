#ifdef __UartUtils_h 

#define _BAUD 9600 // �������� ������ � ����� 
#define WAIT_PRESC 256 // �������� ������������ ������� 
#define CRC_BYTE 1 // ���������� ���� CRC (��� CRC - 0, ��� 8 ������� - 1, ��� 16 ������� - 2) 
#define MAX_BYTE_RX (10+CRC_BYTE) // ������������ ����� ���� �� ����� c ������ CRC 
#define MAX_BYTE_TX (10+CRC_BYTE) // ������������ ����� ������������ ���� c ������ CRC 
#define DEV_ADDR 0x55 // ����� ���������� 
#define LED_Conn PORTF_Bit1 //��������� ������� ����� 
#define LED_Conn_Dir DDRF_Bit1 //����������� ���������� ������� ����� 
#define Tx_Rx_485 PORTD_Bit4 // 1/0 - ������������ ��������/����� 
#define Tx_Rx_485_Dir DDRD_Bit4 // ����������� ������������ ��������/����� 

/* ������ ������������ USART */ 
#define _U2X 0 // Doble Speed 0 or 1 
#define _UBRR (( ClkFreq-(_BAUD*8L/(1+_U2X)) )/(_BAUD*16L/(1+_U2X))) 
#define __BAUD (ClkFreq/((_UBRR*16L/(1+_U2X))+16L/(1+_U2X))) 
#define _BAUD_ERROR (((__BAUD*10000L)-(_BAUD*10000L))/_BAUD) 

#if (_BAUD_ERROR >= 100)||(_BAUD_ERROR <= -100) // ����� ���� �������� 
#warning ������ ��������� ���������� ����� 1% 
#endif 
/* ������ ������������ */ 
#if WAIT_PRESC == 1 
#define TIME_PRESC (0<<CS02)|(0<<CS01)|(1<<CS02) 
#elif WAIT_PRESC == 8 
#define TIME_PRESC (0<<CS02)|(1<<CS01)|(0<<CS02) 
#elif WAIT_PRESC == 32 
#define TIME_PRESC (0<<CS02)|(1<<CS01)|(1<<CS02) 
#elif WAIT_PRESC == 64 
#define TIME_PRESC (1<<CS02)|(0<<CS01)|(0<<CS02) 
#elif WAIT_PRESC == 128 
#define TIME_PRESC (1<<CS02)|(0<<CS01)|(1<<CS02) 
#elif WAIT_PRESC == 256 
#define TIME_PRESC (1<<CS02)|(1<<CS01)|(0<<CS02) 
#elif WAIT_PRESC == 1024 
#define TIME_PRESC (1<<CS02)|(1<<CS01)|(1<<CS02) 
#else 
#error ���������� �������� ������������ �� ���������� 
#endif 

/* ����� �������� ������ ������� ��� ������������ ������ - "��������� ����� ��������" 
  ������ ����� �������� ��� ��������� ����� �� �������� 3-� ���� */ 
#define TIME_OUT ((3/*�����*/*(1/*�����*/+8/*����*/+1/*����*/)*2000000L)/_BAUD)_us 
#if (TIME_OUT/WAIT_PRESC) > 0xFF 
#error ������� ���������� ������� �������� ������������ 
#elif ((WAIT_PRESC==8)&&((TIME_OUT/WAIT_PRESC)<32))||((WAIT_PRESC>8)&&((TIME_OUT/WAIT_PRESC)<64)) 
#error ������� ���������� ������� �������� ������������ 
#else 
#define _TIME_OUT (TIME_OUT/WAIT_PRESC) 
#define _WAIT_TIME _TIME_OUT/2  // ����� ��� ������������� ������ 
#endif 

void (*resetptr)(void) = 0x0000; //������� �� ������ 0x0000 (Reset) 
void USART_Init(void); // ������������� BAUD � ������� ���������� 
static void ReinitRx(void); // ������������� �� ����� 
static void ReinitTx(void); // ������������� �� �������� 
static void CRC8(unsigned char in, unsigned char *crc); // ������ ����� CRC8 
static void CRC16(unsigned char in, unsigned int *crc); // ������ ����� CRC16 
static void TxData(unsigned char TxByte, unsigned char *TxSourse); // �������� ��������� ����� ���� �� ������ 
static void RxData(unsigned char RxByte, unsigned char *RxRecept); // ��������� ����. ����� ���� � ����� 
static void ErrorTx(unsigned char ErrCode); // �������� ��������� �� ������ ��� ������ 
static void ResetDiag(void); // ���������� ����������� �� ����� ������� ������ 
static void First2Second(void); // �������� ������ �� ������ ������ � ����� ��������� 
static void RxComplete(void); // ��������� �������� ������� �� ������������ ������ � CRC 
void RxAnaliz(void); // ����������� �������� ������� 

#else 
void USART_Init(void); 
void CheckUSART(void); // �������� � ��������� �������� ������ 
#endif 
����� main.c 
#include "main.h" 
#include "UartUtils.h" 

__task void main( void ) 
{ 
  USART_Init(); 
  __enable_interrupt(); 
  while(1) 
  { 
    CheckUSART(); 
  } 
} 
����� ��������� main.h 
#include <iom128.h> 
#include <avr_macros.h> 
#include <inavr.h> 
#define ClkFreq 10000000 // ������� ���������� ���������� 
#define _us *(ClkFreq / 1000000) 
#define _ms *(ClkFreq / 1000) 