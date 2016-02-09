/*
 * FreeModbus Libary: AVR Demo Application
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: demo.c,v 1.7 2006/06/15 15:38:02 wolti Exp $
 */

/* ----------------------- AVR includes -------------------------------------*/
#include "avr/io.h"
#include "avr/interrupt.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/
#define REG_INPUT_START 1000
#define REG_INPUT_NREGS 4

/* ----------------------- Static variables ---------------------------------*/
static USHORT   usRegInputStart = REG_INPUT_START;
static USHORT   usRegInputBuf[REG_INPUT_NREGS];

/* ----------------------- Start implementation -----------------------------*/
int
main( void )
{
    const UCHAR     ucSlaveID[] = { 0xAA, 0xBB, 0xCC };
    eMBErrorCode    eStatus;

    DDRA |= 0b00001110;
	 

    eStatus = eMBInit( MB_RTU, 0x0A, 0, 38400, MB_PAR_NONE );

    eStatus = eMBSetSlaveID( 0x34, TRUE, ucSlaveID, 3 );
    sei(  );

    /* Enable the Modbus Protocol Stack. */
    eStatus = eMBEnable(  );

    for( ;; )
    {
        ( void )eMBPoll(  );

        /* Here we simply count the number of poll cycles. */
        usRegInputBuf[0]++;
    }
}

#define INVBIT(port, bit) port = port ^ (1<<bit);

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{

	INVBIT(PORTA, 1);
	
  eMBErrorCode  eStatus = MB_ENOERR;
  if( ( usAddress == 1 ) && ( usNRegs == 1 ) ) {
      *pucRegBuffer++ = 0;
//      *pucRegBuffer++ = ~PINA;
      *pucRegBuffer++ = usRegInputBuf[0];


  }
  else {
    eStatus = MB_ENOREG;
  }
  return eStatus;
}
 
eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
	
  eMBErrorCode  eStatus = MB_ENOERR;
  if( ( usAddress == 1 ) && ( usNRegs == 1 ) ) {
    if( eMode == MB_REG_READ ) {
      *pucRegBuffer++ = 0;
      *pucRegBuffer++ = ~PINB;
    }
    else {
      pucRegBuffer++;
      //PORTB = ~(*pucRegBuffer++);
    }
  }
  else {
    eStatus = MB_ENOREG;
	//eStatus = MB_ENORES;
  }
  return eStatus;
}


eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
    return MB_ENOREG;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    return MB_ENOREG;
}
