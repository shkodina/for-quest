//***************************************************************************
//
//  Author(s)...: Pashgan    http://ChipEnable.Ru   
//
//  Target(s)...: ����� AVR ����������
//
//  Compiler....: IAR 5.11A
//
//  Description.: ������� ������������ UART`a �� ����������������� AVR.
//                ��� ������� � �������������� appnote �����  IAR. 
//
//  Data........: 17.01.12 
//  Data........: 05.03.13   �������� ������ ��������� ���������, 
//                           �������� ����������� ����������� ����������,
//                           ������� �������� ��������� ��������� � ������ ��������
//***************************************************************************
#ifndef SOFT_UART_H
#define SOFT_UART_H

//#include "compilers_3.h"

/*___________________________���������_____________________________________*/

/*�������� ������� ��*/
#ifndef F_CPU
   #define F_CPU          7372800L     
#endif

/*��������� ������� 1L, 8L, 64L, 256L, 1024L*/
#define SUART_PRESCALER   8L           

/*�������� ������*/
#define SUART_BAUD_RATE   9600L       

/*����������� ��������� ������*/
#define SUART_BUF_SIZE    16           

/*��� ���������*/
#define RX_PINX       PINB
#define RX_PORTX      PORTB
#define RX_DDRX       DDRB
#define RX_PIN        1

/*��� �����������*/
#define TX_PORTX      PORTB
#define TX_DDRX       DDRB
#define TX_PIN        0

/*������� ��� ������, ����������� �� �����
�������� ����������� ������ � �������� �����.
��� ����������� � ������� SUART_GetChar()*/ 
#define SUART_Idle()   

/*________________________���������������� �������___________________________*/

void SUART_Init(void);
char SUART_GetChar(int * timeout);
void SUART_PutChar(char ch);
void SUART_PutStr(char *str);
void SUART_FlushInBuf(void);
char SUART_Kbhit(void);
void SUART_TurnRxOn(void);
void SUART_TurnRxOff(void);

/*___________________________________________________________________________*/

#endif //SOFT_UART_H

