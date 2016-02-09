#ifdef __UartUtils_h 

#define _BAUD 9600 // Скорость обмена в бодах 
#define WAIT_PRESC 256 // Значение предделителя таймера 
#define CRC_BYTE 1 // Количество байт CRC (без CRC - 0, для 8 битного - 1, для 16 битного - 2) 
#define MAX_BYTE_RX (10+CRC_BYTE) // Максимальное число байт на прием c учетом CRC 
#define MAX_BYTE_TX (10+CRC_BYTE) // Максимальное число передаваемых байт c учетом CRC 
#define DEV_ADDR 0x55 // Адрес Устройства 
#define LED_Conn PORTF_Bit1 //Светодиод наличия связи 
#define LED_Conn_Dir DDRF_Bit1 //направление светодиода наличия связи 
#define Tx_Rx_485 PORTD_Bit4 // 1/0 - Переключение передача/прием 
#define Tx_Rx_485_Dir DDRD_Bit4 // направление переключения передача/прием 

/* Расчет предделителя USART */ 
#define _U2X 0 // Doble Speed 0 or 1 
#define _UBRR (( ClkFreq-(_BAUD*8L/(1+_U2X)) )/(_BAUD*16L/(1+_U2X))) 
#define __BAUD (ClkFreq/((_UBRR*16L/(1+_U2X))+16L/(1+_U2X))) 
#define _BAUD_ERROR (((__BAUD*10000L)-(_BAUD*10000L))/_BAUD) 

#if (_BAUD_ERROR >= 100)||(_BAUD_ERROR <= -100) // сотые доли процента 
#warning ошибка установки составляет более 1% 
#endif 
/* Расчет предделителя */ 
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
#error выбранного значения предделителя не существует 
#endif 

/* Время ожидания ответа ведущим для формирования ошибки - "превышено время ожидания" 
  Расчет паузы ожидания как удвоенное время на передачу 3-х байт */ 
#define TIME_OUT ((3/*паузы*/*(1/*старт*/+8/*байт*/+1/*стоп*/)*2000000L)/_BAUD)_us 
#if (TIME_OUT/WAIT_PRESC) > 0xFF 
#error следует установить большее значение предделителя 
#elif ((WAIT_PRESC==8)&&((TIME_OUT/WAIT_PRESC)<32))||((WAIT_PRESC>8)&&((TIME_OUT/WAIT_PRESC)<64)) 
#error следует установить меньшее значение предделителя 
#else 
#define _TIME_OUT (TIME_OUT/WAIT_PRESC) 
#define _WAIT_TIME _TIME_OUT/2  // Время для синхронизации пакета 
#endif 

void (*resetptr)(void) = 0x0000; //Переход по адресу 0x0000 (Reset) 
void USART_Init(void); // Инициализация BAUD и выводов управления 
static void ReinitRx(void); // Инициализация на прием 
static void ReinitTx(void); // Инициализация на передачу 
static void CRC8(unsigned char in, unsigned char *crc); // Расчет байта CRC8 
static void CRC16(unsigned char in, unsigned int *crc); // Расчет байта CRC16 
static void TxData(unsigned char TxByte, unsigned char *TxSourse); // Передает указанное число байт из буфера 
static void RxData(unsigned char RxByte, unsigned char *RxRecept); // принимает макс. число байт в буфер 
static void ErrorTx(unsigned char ErrCode); // Передача сообщения об ошибке или сбросе 
static void ResetDiag(void); // производит диагностику на прием команды Сброса 
static void First2Second(void); // Копирует данные из буфера приема в буфер обработки 
static void RxComplete(void); // Проверяет принятую посылку на соответствие адреса и CRC 
void RxAnaliz(void); // Анализирует принятые посылки 

#else 
void USART_Init(void); 
void CheckUSART(void); // проверка и обработка принятых данных 
#endif 
текст main.c 
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
текст заголовка main.h 
#include <iom128.h> 
#include <avr_macros.h> 
#include <inavr.h> 
#define ClkFreq 10000000 // частота кварцевого резонатора 
#define _us *(ClkFreq / 1000000) 
#define _ms *(ClkFreq / 1000) 