//ATmega16
#define dXTAL 16000000

#include "ioavr.h"
#include "ina90.h"
#include "stdbool.h"

//������ ������ ����������� �� UART ������
#define MaxLenghtRecBuf 25
//������ ������ ������������ �� UART ������
#define MaxLenghtTrBuf 25


#define SetBit(Port,bit) Port|=(1<<bit)
#define ClrBit(Port,bit) Port&=~(1<<bit)
#define InvBit(Port,bit) Port^=(1<<bit)
#define Hi(Int) (char) (Int>>8)
#define Low(Int) (char) (Int)

//ModBus
char ModBus(char NumByte);
bool bModBus;  //���� ��������� �������
unsigned char cNumRcByte0; //�������� � ���������� ���-�� �������� ����
unsigned char cNumTrByte0;  //���-�� ������������ ������

//UART
void StartUART0(void);
void StartTrans0(void);
unsigned char cmRcBuf0[MaxLenghtRecBuf] ; //����� ����������� ������
unsigned char cmTrBuf0[MaxLenghtTrBuf] ; //����� ������������ ������

//��������� ���������
void Setup(void){
  DDRB=0xFF;  //���� �� �����
  PORTB=0xFF; //�� ������ 1


//  SetBit(DDRD, 4);  //��������� ���������� ��������������
//  ClrBit(PORTD,4);  //COM-����
//  SetBit(DDRD, 5);  //���������� ������ ��������������
//  ClrBit(PORTD,5);  //
}//end Setup()

int main( void ){
  Setup();  //��������� ��������� ��
  _SEI(); //���������� ����������
  StartUART0();

  while(1){
    if (bModBus){
      cNumTrByte0=ModBus(cNumRcByte0); //��������� ��������� ���������� ModBus
      if (cNumTrByte0!=0) StartTrans0();
      bModBus=false;
    }//end if (bModBus)
  }//end while(1)
}//end main()

//������� ��� �������� ������� ���� CRC-16
__flash const unsigned char srCRCHi[256]={
         0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
         0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
         0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
         0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
         0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
         0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
         0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
         0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
         0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
         0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
         0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
         0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
         0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
         0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
         0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
         0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
         0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
         0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
         0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
         0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
         0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
         0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
         0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
         0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
         0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
         0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};
__flash const unsigned char srCRCLo[256]={
         0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
         0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
         0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
         0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
         0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
         0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
         0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
         0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
         0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
         0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
         0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
         0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
         0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
         0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
         0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
         0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
         0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
         0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
         0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
         0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
         0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
         0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
         0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
         0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
         0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
         0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

//������� ��������� ��� CRC-16
//�� ����� ��������� �� ������ ������
//� ���������� ���� ��������� (��� ��������� ���� CRC-16)
int GetCRC16(unsigned char *buf, char bufsize)
{
  char CRC_Low = 0xFF;
  char CRC_High = 0xFF;
  char k;
  char carry;
  for (k=0; k<bufsize; k++)
   {
    carry = CRC_Low ^ buf[k];
    CRC_Low = CRC_High ^ srCRCHi[carry];
    CRC_High = srCRCLo[carry];
   };
  //return (CRC_High);
  return((CRC_High<<8)|CRC_Low);
}//end GetCRC16()

//������������ ������ �� ������
char ErrorMessage(char Error){
  char TempI;
  cmTrBuf0[1]=cmRcBuf0[1]+0x80;;//������� � �������
  cmTrBuf0[2]=Error;
  TempI=GetCRC16(cmTrBuf0,3);//������� �� �������
  cmTrBuf0[3]=Low(TempI);
  cmTrBuf0[4]=Hi(TempI);
  return 5;
}//end ErrorMessage()

int CRC16;
char ModBus(char NumByte){
  int TempI;
   //����� ������� �� �����
  cmRcBuf0[NumByte]=0x00;

  //��������� �������
  if (cmRcBuf0[0]!=0x20) return 0x00; //����� ����������  //����� �� �����
  CRC16=GetCRC16(cmRcBuf0,NumByte-2);//������� CRC � �������� �������
  TempI=(int) (cmRcBuf0[NumByte-1]<<8) + cmRcBuf0[NumByte-2];
  if (CRC16!=TempI) return 0x00;  //����������� ����� //����� �� �����
  cmTrBuf0[0]=0x20;//����� ����������
  //��� �������
  switch(cmRcBuf0[1]){
    case 0x03:{//������ ���������
      TempI=(int) (cmRcBuf0[2]<<8) + cmRcBuf0[3];
      if (TempI!=1){ //�������� ������ ��������, ���� ������ 1 �������
        return ErrorMessage(0x02); //������ ����� �� ����� ���� ���������
      }
      TempI=(int) (cmRcBuf0[4]<<8) + cmRcBuf0[5];
      if (TempI!=1){//�������� ���-�� ������������� ���������, ���� ������ 1 �������
         return ErrorMessage(0x02); //������ ����� �� ����� ���� ���������
      }
      cmTrBuf0[1]=0x03;//�������
      cmTrBuf0[2]=0x02;//���-�� ���� ������
      cmTrBuf0[3]=0x00;//������� ����
      TempI=PINB;
      cmTrBuf0[4]=Low(TempI);//������ ����� F
      TempI=GetCRC16(cmTrBuf0,5);//������� �� �������
      cmTrBuf0[5]=Low(TempI);
      cmTrBuf0[6]=Hi(TempI);
      return 7;
    }
    case 0x06:{//������ � ��������� �������
      TempI=(int) (cmRcBuf0[2]<<8) + cmRcBuf0[3];
      if (TempI!=1){ //�������� ������ ��������, ���� ������ 1 �������
         return ErrorMessage(0x02); //������ ����� �� ����� ���� ���������
      }
      TempI=(int) (cmRcBuf0[4]<<8) + cmRcBuf0[5];
      if (TempI>0xFF){  //�������� �����, ������� ���� �������� � ����
         return ErrorMessage(0x03); //������������ ������ � �������
      }
      PORTB=Low(TempI);
      cmTrBuf0[1]=cmRcBuf0[1];//�������
      cmTrBuf0[2]=cmRcBuf0[2];//�����
      cmTrBuf0[3]=cmRcBuf0[3];//
      cmTrBuf0[4]=cmRcBuf0[4];//������
      cmTrBuf0[5]=cmRcBuf0[5];//
      cmTrBuf0[6]=cmRcBuf0[6];//��
      cmTrBuf0[7]=cmRcBuf0[7];//
      return 8;
    }
    default:{
       return ErrorMessage(0x01); //������������ �������
    }
  }
}//end ModBus()

//��������� ���������� ������
#define DisableReceive0 ClrBit(UCSRB,RXEN); ClrBit(UCSRB,RXCIE)
//��������� ���������� ������
#define EnableReceive0 SetBit(UCSRB,RXEN); SetBit(UCSRB,RXCIE)
//�������� �� ���������� ���������� ������
#define TestReceive0 TestBit(UCSRB,RXCIE)
//��������� ���������� �� ������������ ������ ��������, ������ ��������
#define GoTrans0 SetBit(UCSRB,TXEN); SetBit(UCSRB,UDRIE)
//��������� ���������� �� ������������ ������ ��������, ��������� ��������
#define StopTrans0 ClrBit(UCSRB,TXEN); ClrBit(UCSRB,UDRIE)
//�������� �� ���������� ���������� ��������
#define TestTrans0 TestBit(UCSRB,UDRIE)


#define StartTimer0 TCNT0=dTCNT0;TCCR0=0x03; //������ ������� 0
#define InitTimer0 TIFR&=0xFD;TIMSK|=(1<<TOIE0); //������������� �������0

//MaxPause = 1.5*(8+1+2)/bod = 0.85�� -> TCNT0~40
#define dTCNT0 40

unsigned char RcCount, TrCount;  //������� ��������/���������� ������ �������
bool StartRec=false;// false/true ������/����� �������
unsigned char DataPause;  //����� ����� �������

extern bool bModBus;  //���� ��������� �������
extern unsigned char cNumRcByte0; //�������� � ���������� ���-�� �������� ����
extern unsigned char cNumTrByte0;

//��������� UART
void StartUART0(void){
  UBRRH=Hi(((dXTAL/16)/19200)-1);
  UBRRL=Low(((dXTAL/16)/19200)-1);

UCSRA=0x00;
UCSRB=0xD8;
UCSRC=0x86;

UBRRH=0x00;
UBRRL=0x33;

  EnableReceive0;
  InitTimer0;
  StartTimer0;
}//end void StartUART0()

char cTempUART;
#pragma vector=USART_RXC_vect //����� ���������� ������ UART
__interrupt void UART_RX_interrupt(void){
  cTempUART=UDR;

  if  (UCSRA&(1<<FE)) return;   //FE-������ �����, OVR - ������������ ������ (�����)

  if (!StartRec){ //���� ��� ������ ����, �� �������� �����
      StartRec=true;
      RcCount=0;
      DataPause=0;
      cmRcBuf0[RcCount++]=cTempUART;
      StartTimer0;
  }else{// end if (StartRec==0) //���������� �����
    if (RcCount<MaxLenghtRecBuf){//���� ��� �� ����� ������
      cmRcBuf0[RcCount++]=cTempUART;
    }else{//����� ����������
      cmRcBuf0[MaxLenghtRecBuf-1]=cTempUART;
    }
    DataPause=0;
    TCNT0=dTCNT0;//���������� �������
  }//end else if (StartRec==0)
}//end  __interrupt UART0_RX_interrupt()

#pragma vector=USART_UDRE_vect //����� ���������� �������� � ����� UART
__interrupt void UART_UDRE_interrupt(void){
  if (TrCount<cNumTrByte0){
    UDR=cmTrBuf0[TrCount];
    TrCount++;
  }else{
    StopTrans0;
    TrCount=0;
  }
}//end __interrupt UART0_UDRE_interrupt()

//���������� �������� �� UART, � ��������� ��-�� ������������ ������
void StartTrans0(void){
  TrCount=0;
  GoTrans0;
}//end  void StartTrans1()

#pragma vector=TIMER0_OVF_vect  //����� ���������� �������/�������� 0 �� ������������
__interrupt void Timer0_overflowed_interrupt(void){
  if (StartRec){
    StartRec=false; //������� �������
    cNumRcByte0=RcCount;  //���-�� �������� ����
    bModBus=true;//
    TCCR0=0;//��������� ������
  }
}//end __interrupt void Timer0_overflowed_interrupt()
