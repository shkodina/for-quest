#include "main.h" 
#define __UartUtils_h 
#include "UartUtils.h" 

static unsigned char Tx_cntr;  // ������� ������������ ������ 
static unsigned char Rx_cntr;  // ������� �������� ������ 
static unsigned char TxSecond[MAX_BYTE_TX];  // ����� �������� ��������� ��������� 
static unsigned char TxFirst[6+CRC_BYTE]; // ����� �������� ��������� ������ � ������ 
static unsigned char *pRXbegin;  // ��������� �� ����� ������ 
static unsigned char *pTXbegin; // ��������� �� ����� �������� 
static unsigned char RxFirst[MAX_BYTE_RX + CRC_BYTE]; // ��������� ����� (������ ������) 
static unsigned char RxSecond[MAX_BYTE_RX]; // ���������  ����� (��������� ������) 
static unsigned char RxSecondCnt; // ����� c������� �������� ������ ��� ��������� ������ 
static unsigned char TxSecondCnt; // ����� ���� �� �������� ��������� ��������� ������ 
static unsigned char ByteTx; // ���������� ������������ ���� � ������ CRC 
static unsigned char ByteRx; // ���������� ����������� ���� � ������ CRC 

union { 
  unsigned char UARTstat; 
  struct { 
    unsigned char 
  Tx_now:1,  // 1 - ���������� �������� 
  Rx_now:1,  // 1 - ���������� ����� 
  Tx_Rx:1,    // 1/0 - ��������� �� ��������/����� 
  Rx_OK:1,    // 1 - ��������� ������, ������������� ��������� ������� �������� ������ 
  Rx_Rst:1,  // 1 - ���� �������� �������, ����� �������� ����� ����� ����� 
  DataRx:1,  // 1 - ������ ������� � ���������, 0 - ������ ���������� 
  Rx_Pause:1, // 1 - ����� �� ������ ���������, 0 - ����� ����� 3-� ������� 
  Tx_En:1;    // 1 - ������ ���������� �� ��������, 0 - �������� ���� ����������� 
  }; 
}; 

void CheckUSART(void) 
{ 
  if (DataRx) RxAnaliz(); 
} 

void USART_Init(void) 
{ 
  UCSR0A = (_U2X<<U2X0); // Doble Speed, Multi-Processor Communication Mode Disable 
  UCSR0B = 0x00;  // Rx, Tx, Data Register Empty Interrupts Disable, Rx, Tx Disabled, 8 bit 
  UCSR0C = (1<<UCSZ01)|(1<<UCSZ00); // Asynchronous Operation, Parity Mode Disabled, 1 Stop, 8 bit 
  __out_word(UBRR0,_UBRR); 
  LED_Conn_Dir = 1; 
  Tx_Rx_485_Dir = 1; 
  RxData(MAX_BYTE_RX,RxFirst); // ��������� �� ����� 
} 

void ReinitRx(void) 
{ 
  unsigned char temp; 
  TIMSK_TOIE0 = 0; // ������ ���������� �� ������� 
  UCSR0B = 0x00;  // Rx, Tx, Data Register Empty Interrupts Disable, Rx, Tx Disabled, 8 bit 
#if CRC_BYTE == 1 
  *(pRXbegin+ByteRx) = 0x00; 
#elif CRC_BYTE == 2 
  *((int*)(pRXbegin+ByteRx)) = 0xFFFF; 
#endif 
  Tx_now = Rx_now = Tx_Rx = Rx_Pause = Rx_OK = 0; 
  Rx_cntr = 0x00; 
  Tx_Rx_485 = 0; // ��������� �������� RS485 �� ����� 
  UCSR0B |= (1<<RXEN0)|(1<<RXCIE0); // Reciver Enable, Rx Complete Interrupt Enable 
  temp = UDR0;  // For Clear Buffer USART 
  temp = UDR0; // For Clear Buffer USART 
  TIFR_TOV0 = 1; 
  TCNT0 = 0xFF-_WAIT_TIME; 
  TCCR0 = TIME_PRESC; 
  TIMSK_TOIE0 = 1; 
} 

void ReinitTx(void) 
{ 
  TIMSK_TOIE0 = 0; // ������ ���������� �� ������� 
  UCSR0B = 0x00;  // Rx, Tx, Data Register Empty Interrupts Disable, Rx, Tx Disabled, 8 bit 
#if CRC_BYTE == 1 
  *(pTXbegin+ByteTx-CRC_BYTE) = 0x00; 
#elif CRC_BYTE == 2 
  *((int*)(pTXbegin+ByteTx-CRC_BYTE)) = 0xFFFF; 
#endif 
  Tx_now = Tx_Rx = Rx_Pause = 1; Rx_now = Rx_OK = 0; 
  Tx_cntr = 0x00; 
  Tx_Rx_485 = 1; // ��������� �������� RS485 �� �������� 
  UCSR0B |= (1<<TXEN0); // Transmiter Enable 
  TIFR_TOV0 = 1; 
  TCNT0 = 0xFF-_WAIT_TIME; 
  TCCR0 = TIME_PRESC; 
  TIMSK_TOIE0 = 1; 
} 

#pragma inline = forced 
void CRC8(unsigned char in,unsigned char *crc) 
{ 
#define POLYNOM_8 0x18 // ������� ������� ��� 8 ������� CRC 
  for (unsigned char i=0; i<8; i++) 
  { 
    if ((in ^ *crc) & 0x01) 
    { 
      *crc = ((*crc ^ POLYNOM_8)>>1)|0x80; 
      in = (in>>1)|0x80; 
    } 
    else 
    { 
      in >>= 1; 
      *crc >>= 1; 
    } 
  } 
} 

#pragma inline = forced 
void CRC16(unsigned char in, unsigned int *crc) 
{ 
#define POLYNOM_16 0xA001 // ������� ������� ��� 16 ������� CRC 
  *crc ^= (int)in; 
  for (unsigned char i=0; i<8; i++) 
  { 
    *crc = (*crc & 0x01)? (*crc >> 1) ^ POLYNOM_16 : (*crc >> 1); 
  } 
} 

void TxData(unsigned char TxByte, unsigned char *TxSourse) 
{ 
  if ((TxByte > CRC_BYTE)&&(TxByte <= MAX_BYTE_TX)&&Tx_En) 
  { 
    TIMSK_TOIE0 = 0;  // disable timer interrupt 
    if (Tx_En) 
    { 
      ByteTx = TxByte;  // ��������� ������������� ����� ������������ ���� 
      pTXbegin = TxSourse; // �������� ��������� �� ����� �������� 
      ReinitTx(); // ������������� �� �������� 
    } 
  } 
} 

void RxData(unsigned char RxByte, unsigned char *RxRecept) 
{ 
  if ((RxByte > CRC_BYTE)&&(RxByte <= MAX_BYTE_TX)) 
  { 
    ByteRx = RxByte; // ��������� ������������� ����� ����������� ���� 
    pRXbegin = RxRecept; // �������� ��������� �� ����� ������ 
    ReinitRx(); // ������������� �� ����� 
  } 
} 

void ErrorTx(unsigned char ErrCode) 
{ 
  unsigned char *pFirst;  // ��������� �� ��������� ����� 
  unsigned char *pSecond; // ��������� �� ��������� ����� 

  pFirst = TxFirst; // ��������� �� ����� �������� ��������� �� ������� � ������ 
  pSecond = RxSecond; // ��������� �� ����� �������������� ������ 
  for (signed char i = 3+CRC_BYTE ; i != 0; i--) 
  { 
    *pFirst++ = *pSecond++; 
  } 
  *(TxFirst+2) = ErrCode; 
  *(TxFirst+1) |= 0x80; 
  TxData(3+CRC_BYTE, TxFirst); 
} 

#pragma inline = forced 
void ResetDiag(void) 
{ 
  unsigned char *pFirst;  // ��������� �� ��������� ����� 
  unsigned char *pSecond; // ��������� �� ��������� ����� 
  unsigned char __flash *pFlash; 
  __flash static unsigned char ResetFrame[]={0x08,0x01,0x00,0x00,0x00}; 

  if ((RxFirst[0] != 0x00)&&(Rx_cntr == (sizeof(ResetFrame)+1+CRC_BYTE))) 
  { // ���� �� ����� �����, � ����� ��������� 
    pFirst = RxFirst + 1; // ��������� �� ����� �������� ������ 
    pFlash = ResetFrame; // ��������� �� ResetFrame 
    for (signed char i = sizeof(ResetFrame); i >= 0; i--) 
    { 
      if (i == 0x00) 
      { // ���� ������� ������ �� ����� 
        pFirst = RxFirst; // ��������� �� ����� �������� ������ 
        pSecond = TxFirst; // ��������� �� ����� �������� ��������� �� ������� � ������ 
        for (signed char j = sizeof(ResetFrame); j >= 0; j--) 
        { 
          *pSecond++ = *pFirst++; 
        } 
        TxData((sizeof(ResetFrame)+1+CRC_BYTE),TxFirst); // �������� ����� �� ����� // sizeof(ResetFrame)+1+CRC_BYTE 
        DataRx = 0; // ������ ���������� 
        Rx_Rst = 1; // ������������� � ������ 
        break; 
      } 
      if (*pFirst++ != *pFlash++) break; 
    } 
  } 
} 
#pragma inline = forced 
void First2Second(void) 
{ 
  unsigned char *pFirst;  // ��������� �� ��������� ����� 
  unsigned char *pSecond; // ��������� �� ��������� ����� 

  pFirst = RxFirst; // ��������� �� ����� �������� ������ 
  pSecond = RxSecond; // ��������� �� ����� ������������ ������ 
  RxSecondCnt = Rx_cntr;  // �������� ����� �������� ���� 
  for (unsigned char i = Rx_cntr; i != 0; i-- ) 
  { 
    *pSecond++ = *pFirst++; 
  } 
} 
#pragma inline = forced 
void RxComplete(void) 
{ 
  unsigned char *pFirst;  // ��������� �� ��������� ����� 
  unsigned char *pSecond; // ��������� �� ��������� ����� 

  UCSR0B = 0x00;  // Rx, Tx, Data Register Empty Interrupts Disable, Rx, Tx Disabled, 8 bit 
  Rx_now = 0; // ����� �������� 
  pFirst = pRXbegin + Rx_cntr - CRC_BYTE; // ��������� �� �������� CRC 
  pSecond = pRXbegin + ByteRx; // ��������� �� ��������� CRC 
  for (signed char i = CRC_BYTE; i >= 0; i--) 
  { 
    if (i == 0x00) // CRC ������, ��� ����� ��� CRC 
      { 
        if ((*pRXbegin > 0xF7)||(*pRXbegin != DEV_ADDR)) 
        { 
          if (*pRXbegin != 0x00)  // ��������� � ����� ������� 
          { 
          ReinitRx(); // �������������� ��� ���������������� �����, ������ ������ 
          } 
          else 
          { 
            Rx_OK = 1; // ����� ������ ������� �������� 
          } 
        } 
        else // ����� � ������� ������������� ������ ���������� 
        { 
          Rx_OK = 1; // ����� ������ ������� �������� 
        } 
        break; 
      } 
    if (*pFirst++ != *pSecond++) 
    { 
      ReinitRx(); // �������� CRC �� ������������� ����������, ������ ������ 
      break; 
    } 
  } 
} 

#pragma vector = USART0_UDRE_vect, USART0_TXC_vect 
#pragma inline = forced 
__interrupt static void UDRE_TXC_INT(void) 
{ 
  LED_Conn = 1; 
  if (Tx_cntr < ByteTx) 
  { 
    UDR0 = *(pTXbegin + Tx_cntr); // �������� USART �� �������� 
    if ((ByteTx - CRC_BYTE) > Tx_cntr) 
    { 
#if CRC_BYTE == 2 
      CRC16(*(pTXbegin + Tx_cntr),(unsigned int*)(pTXbegin + ByteTx - CRC_BYTE)); 
#elif CRC_BYTE == 1 
      CRC8(*(pTXbegin + Tx_cntr),(pTXbegin + ByteTx - CRC_BYTE)); 
#endif 
    } 
    Tx_cntr++;  // ��� ���� ���� ������� 
  } 
  else 
  { 
    if (!Tx_now) // ����� ��������, �������� �������� ����� 
    { 
      TIFR_TOV0 = 1; 
      TCNT0 = 0xFF-_WAIT_TIME; 
      TIMSK_TOIE0 = 1; 
      LED_Conn = 0; 
    } 
    else // ������������� �� ���������� �� ��������� �������� 
    { 
      UCSR0B &= ~(1<<UDRIE0); 
      UCSR0B |= (1<<TXCIE0); 
      Tx_now = 0; // ������ ���� ��������� 
    } 
  } 
} 

#pragma vector = USART0_RXC_vect 
#pragma inline = forced 
__interrupt static void USART_RXC_INT(void) 
{ 
  LED_Conn = 1; 
  TIMSK_TOIE0 = 0;  // disable timer interrupt 
  if (!Rx_Pause) 
  { 
    ReinitRx(); // ������ ������������� �� �����, ������ ������ 
  } 
  else 
  { 
    if (Rx_now) 
    { 
      if (TCNT0 >= (0xFF - _WAIT_TIME/2)) 
      { 
        Rx_cntr = 0x00; // ���� ����� � ������ ����� _WAIT_TIME/2, ����� ����� � ���� 
      } 
    } 
    else 
    { 
      Rx_now = 1; // ����� ���������, ����� ������� 
    } 
    if (UCSR0A_FE0 || UCSR0A_DOR0) 
    { 
      ReinitRx(); // ������ �� ������, ������ ������ 
    } 
    else 
    { 
      TCNT0 = 0xFF-_WAIT_TIME; // �������� ����� ��� ������������� 
      TIFR_TOV0 = 1; 
      TIMSK_TOIE0 = 1;  // Enable Timer interrupt 
      if (Rx_cntr >= ByteRx) 
      { 
        ReinitRx(); // ������� ���� ������ ����������, ������ ������ 
      } 
      else 
      { 
        *(pRXbegin + Rx_cntr) = UDR0; // ����� ������ �� USART 
        if (CRC_BYTE <= Rx_cntr) 
        { 
#if CRC_BYTE == 2 
          CRC16(*(pRXbegin + Rx_cntr - CRC_BYTE),(unsigned int*)(pRXbegin + ByteRx)); 
#elif CRC_BYTE == 1 
          CRC8(*(pRXbegin + Rx_cntr - CRC_BYTE),(pRXbegin + ByteRx)); 
#endif 
        } 
        Rx_cntr++; // ��� ���� ���� ������ 
      } 
    } 
  } 
} 
#pragma vector = TIMER0_OVF_vect 
#pragma inline = forced 
__interrupt static void TIMER_INT(void) 
{ 
  unsigned char *pFirst;  // ��������� �� ��������� ����� 
  unsigned char *pSecond; // ��������� �� ��������� ����� 

  if (Rx_now) 
  { 
    RxComplete(); // ����� ��������, ��������� �������� ������ 
    if (Rx_OK) 
    { 
      Tx_En = 1; // ���������� �� ������������� �������� 
      ResetDiag(); // �������� �� ����� ������� "�������� ����������" 
      if (!Rx_Rst) 
      { // ���� "�����" �� ������ 
        if (DataRx) 
        { 
          ErrorTx(0x06); // ������ ������� "���������� ������" 
        } 
        else 
        { 
          Rx_OK = 0; // ����� ���� "������ �������" 
          /* ��������� �������� ������ � ���������� �������� */ 
          if (Rx_cntr != RxSecondCnt) 
          { 
            First2Second(); // ���������� ������ ��� ��������� 
            DataRx = 1; // ������ ������� � ��������� 
            RxData(MAX_BYTE_RX,RxFirst); // �������������� �� ����� ����� �������� 
          } 
          else // ��������� ��������� � ������������ ������� 
          { 
            pFirst = RxFirst + Rx_cntr; // ��������� �� ����� �������� ������ 
            pSecond = RxSecond + RxSecondCnt; // ��������� �� ����� ������������ ������ 
            for(unsigned char i = RxSecondCnt; i > 0x00; i--) 
            { 
              if (*(--pFirst) != *(--pSecond)) // ���� ������ �� ��������� 
              { 
                First2Second(); // ���������� ������ ��� ��������� 
                DataRx = 1; // ������ ������� � ��������� 
                RxData(MAX_BYTE_RX,RxFirst); // �������������� �� ����� ����� �������� 
                break; 
              } 
              else 
              { 
                if (i == 0x01) 
                { 
                  RxSecondCnt = 0x00; // ��������� ������ ��� ��������� ���������� ������ �� ������� 
                  TxData(TxSecondCnt,TxSecond); // �������� ����� ������������ �� ����� ������� ������ 
                } 
              } 
            } 
          } 
        } 
      } 
    } 
  } 
  else 
  { 
    if (Rx_Pause) 
    { 
      if (!Tx_now) 
      { 
        if (!Tx_Rx) 
        { 
          ErrorTx(0x05); // ������� "��������" 
        } 
        else 
        { 
          if (Rx_Rst) // ������� �� ������ 0x0000 (Reset) 
          { 
            __disable_interrupt(); 
            resetptr(); // ������� �� ������ "Reset vector" 
          } 
          else 
          { 
            RxData(MAX_BYTE_RX,RxFirst); // �������������� �� ����� 
          } 
        } 
      } 
      else // �������� ����� ���������, �������� �������� 
      { 
        UCSR0B |= (1<<UDRIE0); 
        TIMSK_TOIE0 = 0;  // disable timer interrupt 
        Tx_En = 0; // ������ ����� ������������� �������� 
      } 
    } 
    else 
    { 
      Rx_Pause = 1; // ��������� ���� "����� ���������" 
      if (Tx_En) 
      { 
        TCNT0 = 0xFF-_TIME_OUT; // �������� ������� �� ����� 
      } 
      else 
      { 
        TIMSK_TOIE0 = 0;  // disable timer interrupt 
      } 
    } 
  } 
} 
#pragma inline = forced 
void RxAnaliz(void) 
{ 
  unsigned char *pFirst;  // ��������� �� ��������� ����� 
  unsigned char *pSecond; // ��������� �� ��������� ����� 

  if (*(RxSecond+1) == 0x08) 
  { 
    if (!*RxSecond) 
    { 
      RxData(MAX_BYTE_RX,RxFirst); // �� ����� ������ �� �������� 
    } 
    else 
    { 
      if ((unsigned int*)*(RxSecond+2) == 0x0000) 
      { 
        pFirst = RxSecond; // ��������� �� ����� ��������� ������ 
        pSecond = TxSecond; // ��������� �� ����� ������������ ������ 
        for (signed char i = RxSecondCnt; i != 0; i--) 
        { 
          *pSecond++ = *pFirst++; 
        } 
        TxSecondCnt = RxSecondCnt; 
        TxData(TxSecondCnt, TxSecond); // ��������� ��� ������� 
      } 
      else 
      { 
        ErrorTx(0x02); // ���������������� ����� 
      } 
    } 
  } 
  else 
  { 
    ErrorTx(0x01); // ���������������� ������� 
  } 
  DataRx = 0; // ������ ���������� 
} 
