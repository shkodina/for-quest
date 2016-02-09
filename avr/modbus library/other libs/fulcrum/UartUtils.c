#include "main.h" 
#define __UartUtils_h 
#include "UartUtils.h" 

static unsigned char Tx_cntr;  // Счетчик отправленных байтов 
static unsigned char Rx_cntr;  // Счетчик принятых байтов 
static unsigned char TxSecond[MAX_BYTE_TX];  // Буфер передачи обработки сообщений 
static unsigned char TxFirst[6+CRC_BYTE]; // Буфер передачи обработки ошибок и сброса 
static unsigned char *pRXbegin;  // Указатель на буфер приема 
static unsigned char *pTXbegin; // Указатель на буфер передачи 
static unsigned char RxFirst[MAX_BYTE_RX + CRC_BYTE]; // Первичный буфер (приема данных) 
static unsigned char RxSecond[MAX_BYTE_RX]; // Вторичный  буфер (обработки данных) 
static unsigned char RxSecondCnt; // Копия cчетчика принятых байтов для обработки данных 
static unsigned char TxSecondCnt; // Число байт на передачу сообщения обработки данных 
static unsigned char ByteTx; // Количество передаваемых байт с учетом CRC 
static unsigned char ByteRx; // Количество принимаемых байт с учетом CRC 

union { 
  unsigned char UARTstat; 
  struct { 
    unsigned char 
  Tx_now:1,  // 1 - происходит передача 
  Rx_now:1,  // 1 - происходит прием 
  Tx_Rx:1,    // 1/0 - настройка на передачу/прием 
  Rx_OK:1,    // 1 - окончания приема, положительный результат анализа принятых данных 
  Rx_Rst:1,  // 1 - идет отправка посылки, после защитной паузы будет сброс 
  DataRx:1,  // 1 - данные приняты к обработке, 0 - данные обработаны 
  Rx_Pause:1, // 1 - Пауза по приему выдержана, 0 - пауза менее 3-х посылок 
  Tx_En:1;    // 1 - Выдано разрешение на передачу, 0 - передача была произведена 
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
  RxData(MAX_BYTE_RX,RxFirst); // Настройка на прием 
} 

void ReinitRx(void) 
{ 
  unsigned char temp; 
  TIMSK_TOIE0 = 0; // Запрет прерывания по таймеру 
  UCSR0B = 0x00;  // Rx, Tx, Data Register Empty Interrupts Disable, Rx, Tx Disabled, 8 bit 
#if CRC_BYTE == 1 
  *(pRXbegin+ByteRx) = 0x00; 
#elif CRC_BYTE == 2 
  *((int*)(pRXbegin+ByteRx)) = 0xFFFF; 
#endif 
  Tx_now = Rx_now = Tx_Rx = Rx_Pause = Rx_OK = 0; 
  Rx_cntr = 0x00; 
  Tx_Rx_485 = 0; // Настройка драйвера RS485 на прием 
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
  TIMSK_TOIE0 = 0; // Запрет прерывания по таймеру 
  UCSR0B = 0x00;  // Rx, Tx, Data Register Empty Interrupts Disable, Rx, Tx Disabled, 8 bit 
#if CRC_BYTE == 1 
  *(pTXbegin+ByteTx-CRC_BYTE) = 0x00; 
#elif CRC_BYTE == 2 
  *((int*)(pTXbegin+ByteTx-CRC_BYTE)) = 0xFFFF; 
#endif 
  Tx_now = Tx_Rx = Rx_Pause = 1; Rx_now = Rx_OK = 0; 
  Tx_cntr = 0x00; 
  Tx_Rx_485 = 1; // Настройка драйвера RS485 на передачу 
  UCSR0B |= (1<<TXEN0); // Transmiter Enable 
  TIFR_TOV0 = 1; 
  TCNT0 = 0xFF-_WAIT_TIME; 
  TCCR0 = TIME_PRESC; 
  TIMSK_TOIE0 = 1; 
} 

#pragma inline = forced 
void CRC8(unsigned char in,unsigned char *crc) 
{ 
#define POLYNOM_8 0x18 // Любимый полином для 8 битного CRC 
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
#define POLYNOM_16 0xA001 // Любимый полином для 16 битного CRC 
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
      ByteTx = TxByte;  // Установка максимального числа передаваемых байт 
      pTXbegin = TxSourse; // Загрузка указателя на буфер передачи 
      ReinitTx(); // Инициализация на передачу 
    } 
  } 
} 

void RxData(unsigned char RxByte, unsigned char *RxRecept) 
{ 
  if ((RxByte > CRC_BYTE)&&(RxByte <= MAX_BYTE_TX)) 
  { 
    ByteRx = RxByte; // Установка максимального числа принимаемых байт 
    pRXbegin = RxRecept; // Загрузка указателя на буфер приема 
    ReinitRx(); // Инициализация на прием 
  } 
} 

void ErrorTx(unsigned char ErrCode) 
{ 
  unsigned char *pFirst;  // Указатель на Первичный буфер 
  unsigned char *pSecond; // Указатель на Вторичный буфер 

  pFirst = TxFirst; // указатель на буфер передачи сообщений об ошибках и сброса 
  pSecond = RxSecond; // указатель на буфер обрабатываемых данных 
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
  unsigned char *pFirst;  // Указатель на Первичный буфер 
  unsigned char *pSecond; // Указатель на Вторичный буфер 
  unsigned char __flash *pFlash; 
  __flash static unsigned char ResetFrame[]={0x08,0x01,0x00,0x00,0x00}; 

  if ((RxFirst[0] != 0x00)&&(Rx_cntr == (sizeof(ResetFrame)+1+CRC_BYTE))) 
  { // Если не общий адрес, и длина совпадает 
    pFirst = RxFirst + 1; // указатель на буфер принятых данных 
    pFlash = ResetFrame; // указатель на ResetFrame 
    for (signed char i = sizeof(ResetFrame); i >= 0; i--) 
    { 
      if (i == 0x00) 
      { // Если получен запрос на сброс 
        pFirst = RxFirst; // указатель на буфер принятых данных 
        pSecond = TxFirst; // указатель на буфер передачи сообщений об ошибках и сброса 
        for (signed char j = sizeof(ResetFrame); j >= 0; j--) 
        { 
          *pSecond++ = *pFirst++; 
        } 
        TxData((sizeof(ResetFrame)+1+CRC_BYTE),TxFirst); // передать ответ на Сброс // sizeof(ResetFrame)+1+CRC_BYTE 
        DataRx = 0; // Данные обработаны 
        Rx_Rst = 1; // Подготовиться к сбросу 
        break; 
      } 
      if (*pFirst++ != *pFlash++) break; 
    } 
  } 
} 
#pragma inline = forced 
void First2Second(void) 
{ 
  unsigned char *pFirst;  // Указатель на Первичный буфер 
  unsigned char *pSecond; // Указатель на Вторичный буфер 

  pFirst = RxFirst; // указатель на буфер принятых данных 
  pSecond = RxSecond; // указатель на буфер обработанных данных 
  RxSecondCnt = Rx_cntr;  // Загрузка числа принятых байт 
  for (unsigned char i = Rx_cntr; i != 0; i-- ) 
  { 
    *pSecond++ = *pFirst++; 
  } 
} 
#pragma inline = forced 
void RxComplete(void) 
{ 
  unsigned char *pFirst;  // Указатель на Первичный буфер 
  unsigned char *pSecond; // Указатель на Вторичный буфер 

  UCSR0B = 0x00;  // Rx, Tx, Data Register Empty Interrupts Disable, Rx, Tx Disabled, 8 bit 
  Rx_now = 0; // Прием завершен 
  pFirst = pRXbegin + Rx_cntr - CRC_BYTE; // указатель на принятый CRC 
  pSecond = pRXbegin + ByteRx; // указатель на расчетный CRC 
  for (signed char i = CRC_BYTE; i >= 0; i--) 
  { 
    if (i == 0x00) // CRC совпал, или прием без CRC 
      { 
        if ((*pRXbegin > 0xF7)||(*pRXbegin != DEV_ADDR)) 
        { 
          if (*pRXbegin != 0x00)  // Сравнение с общим адресом 
          { 
          ReinitRx(); // несуществующий или неподдерживаемый адрес, начать заново 
          } 
          else 
          { 
            Rx_OK = 1; // Прием данных успешно завершен 
          } 
        } 
        else // Адрес в посылке соответствует адресу устройства 
        { 
          Rx_OK = 1; // Прием данных успешно завершен 
        } 
        break; 
      } 
    if (*pFirst++ != *pSecond++) 
    { 
      ReinitRx(); // принятый CRC не соотыетствует расчетному, начать заново 
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
    UDR0 = *(pTXbegin + Tx_cntr); // Загрузка USART на передачу 
    if ((ByteTx - CRC_BYTE) > Tx_cntr) 
    { 
#if CRC_BYTE == 2 
      CRC16(*(pTXbegin + Tx_cntr),(unsigned int*)(pTXbegin + ByteTx - CRC_BYTE)); 
#elif CRC_BYTE == 1 
      CRC8(*(pTXbegin + Tx_cntr),(pTXbegin + ByteTx - CRC_BYTE)); 
#endif 
    } 
    Tx_cntr++;  // Еще один байт передан 
  } 
  else 
  { 
    if (!Tx_now) // Конец передачи, загрузка охранной паузы 
    { 
      TIFR_TOV0 = 1; 
      TCNT0 = 0xFF-_WAIT_TIME; 
      TIMSK_TOIE0 = 1; 
      LED_Conn = 0; 
    } 
    else // Перенастройка на прерывание по окончанию передачи 
    { 
      UCSR0B &= ~(1<<UDRIE0); 
      UCSR0B |= (1<<TXCIE0); 
      Tx_now = 0; // Выдача байт закончена 
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
    ReinitRx(); // ошибка синхронизации по паузе, начать заново 
  } 
  else 
  { 
    if (Rx_now) 
    { 
      if (TCNT0 >= (0xFF - _WAIT_TIME/2)) 
      { 
        Rx_cntr = 0x00; // была пауза в приеме более _WAIT_TIME/2, прием начат с нуля 
      } 
    } 
    else 
    { 
      Rx_now = 1; // пауза выдержана, прием начался 
    } 
    if (UCSR0A_FE0 || UCSR0A_DOR0) 
    { 
      ReinitRx(); // ошибка по приему, начать заново 
    } 
    else 
    { 
      TCNT0 = 0xFF-_WAIT_TIME; // Загрузка паузы для синхронизации 
      TIFR_TOV0 = 1; 
      TIMSK_TOIE0 = 1;  // Enable Timer interrupt 
      if (Rx_cntr >= ByteRx) 
      { 
        ReinitRx(); // принято байт больше возможного, начать заново 
      } 
      else 
      { 
        *(pRXbegin + Rx_cntr) = UDR0; // Прием данных от USART 
        if (CRC_BYTE <= Rx_cntr) 
        { 
#if CRC_BYTE == 2 
          CRC16(*(pRXbegin + Rx_cntr - CRC_BYTE),(unsigned int*)(pRXbegin + ByteRx)); 
#elif CRC_BYTE == 1 
          CRC8(*(pRXbegin + Rx_cntr - CRC_BYTE),(pRXbegin + ByteRx)); 
#endif 
        } 
        Rx_cntr++; // Еще один байт принят 
      } 
    } 
  } 
} 
#pragma vector = TIMER0_OVF_vect 
#pragma inline = forced 
__interrupt static void TIMER_INT(void) 
{ 
  unsigned char *pFirst;  // Указатель на Первичный буфер 
  unsigned char *pSecond; // Указатель на Вторичный буфер 

  if (Rx_now) 
  { 
    RxComplete(); // Прием завершен, обработка принятых данных 
    if (Rx_OK) 
    { 
      Tx_En = 1; // Разрешение на инициализацию передачи 
      ResetDiag(); // Проверка на прием посылки "Сбросить контроллер" 
      if (!Rx_Rst) 
      { // Если "Сброс" не принят 
        if (DataRx) 
        { 
          ErrorTx(0x06); // Выдыча посылки "Устройство занято" 
        } 
        else 
        { 
          Rx_OK = 0; // сброс бита "данные приняты" 
          /* Сравнение принятых данных с предыдущей посылкой */ 
          if (Rx_cntr != RxSecondCnt) 
          { 
            First2Second(); // Перегрузка данных для обработки 
            DataRx = 1; // Данные приняты к обработке 
            RxData(MAX_BYTE_RX,RxFirst); // Инициализируем на прием новых запросов 
          } 
          else // Сравнение принятого и сохраненного буферов 
          { 
            pFirst = RxFirst + Rx_cntr; // указатель на буфер принятых данных 
            pSecond = RxSecond + RxSecondCnt; // указатель на буфер обработанных данных 
            for(unsigned char i = RxSecondCnt; i > 0x00; i--) 
            { 
              if (*(--pFirst) != *(--pSecond)) // Если запрос не повторный 
              { 
                First2Second(); // Перегрузка данных для обработки 
                DataRx = 1; // Данные приняты к обработке 
                RxData(MAX_BYTE_RX,RxFirst); // Инициализируем на прием новых запросов 
                break; 
              } 
              else 
              { 
                if (i == 0x01) 
                { 
                  RxSecondCnt = 0x00; // Обнуление данных для обработки повторного такого же запроса 
                  TxData(TxSecondCnt,TxSecond); // Передача ранее обработанных по этому запросу данных 
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
          ErrorTx(0x05); // Послать "ожидайте" 
        } 
        else 
        { 
          if (Rx_Rst) // Переход по адресу 0x0000 (Reset) 
          { 
            __disable_interrupt(); 
            resetptr(); // Переход по адресу "Reset vector" 
          } 
          else 
          { 
            RxData(MAX_BYTE_RX,RxFirst); // Инициализируем на прием 
          } 
        } 
      } 
      else // Охранная пауза выдержана, начинаем передачу 
      { 
        UCSR0B |= (1<<UDRIE0); 
        TIMSK_TOIE0 = 0;  // disable timer interrupt 
        Tx_En = 0; // Запрет новой инициализации передачи 
      } 
    } 
    else 
    { 
      Rx_Pause = 1; // Установка бита "пауза выдержана" 
      if (Tx_En) 
      { 
        TCNT0 = 0xFF-_TIME_OUT; // Загрузка времени на ответ 
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
  unsigned char *pFirst;  // Указатель на Первичный буфер 
  unsigned char *pSecond; // Указатель на Вторичный буфер 

  if (*(RxSecond+1) == 0x08) 
  { 
    if (!*RxSecond) 
    { 
      RxData(MAX_BYTE_RX,RxFirst); // На общий запрос не отвечаем 
    } 
    else 
    { 
      if ((unsigned int*)*(RxSecond+2) == 0x0000) 
      { 
        pFirst = RxSecond; // указатель на буфер обработки данных 
        pSecond = TxSecond; // указатель на буфер отправляемых данных 
        for (signed char i = RxSecondCnt; i != 0; i--) 
        { 
          *pSecond++ = *pFirst++; 
        } 
        TxSecondCnt = RxSecondCnt; 
        TxData(TxSecondCnt, TxSecond); // Отправить эхо запроса 
      } 
      else 
      { 
        ErrorTx(0x02); // Неподдерживаемый адрес 
      } 
    } 
  } 
  else 
  { 
    ErrorTx(0x01); // Неподдерживаемая функция 
  } 
  DataRx = 0; // Данные обработаны 
} 
