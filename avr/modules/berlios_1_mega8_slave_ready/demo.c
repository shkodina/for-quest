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
#define REG_HOLDING_START 10
#define REG_HOLDING_NREGS 2
#define REG_HOLDING_NREGS_IN_BYTES 4

#define REG_HOLDING_DATA_START 20
#define REG_HOLDING_DATA_NREGS 62
#define REG_HOLDING_DATA_NREGS_IN_BYTES 124


#define CLIENT_ID 2
#define BAUDRATE 38400


/* ----------------------- Static variables ---------------------------------*/
volatile static unsigned char	g_reg_holding_device [REG_HOLDING_NREGS_IN_BYTES];
volatile static unsigned char	g_reg_holding_data [REG_HOLDING_DATA_NREGS_IN_BYTES];

/* ----------------------- Start implementation -----------------------------*/
int
main( void )
{
    const UCHAR     ucSlaveID[] = { 0xAA, 0xBB, 0xCC };
    eMBErrorCode    eStatus;

    //DDRA |= 0b00001110;
	 

    eStatus = eMBInit( MB_RTU, CLIENT_ID, 0, BAUDRATE, MB_PAR_NONE );

    //eStatus = eMBSetSlaveID( 0x34, TRUE, ucSlaveID, 3 );
    sei(  );

    /* Enable the Modbus Protocol Stack. */
    eStatus = eMBEnable(  );

    for( ;; )
    {
        ( void )eMBPoll(  );

        /* Here we simply count the number of poll cycles. */
        g_reg_holding_device[0]++;
    }
}

#define INVBIT(port, bit) port = port ^ (1<<bit);

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{



  eMBErrorCode  eStatus = MB_ENOERR;

	// READ REGESTERS
	if (eMode == MB_REG_READ){
		if (( usAddress == REG_HOLDING_START ) && 
			( usNRegs == REG_HOLDING_NREGS )){

			// test debug
			if (g_reg_holding_device[2]++ == 255)
				g_reg_holding_device[3]++;

			for (char i = 0; i < usNRegs * 2; i++){
				pucRegBuffer[i] = g_reg_holding_device[i];
			}
		}else{
			eStatus = MB_ENORES;
		}
	}

	// WRITE REGESTERS
	if (eMode == MB_REG_WRITE){
		if (( usAddress == REG_HOLDING_DATA_START ) && 
			( usNRegs <= REG_HOLDING_DATA_NREGS)){

			for (char i = 0; i < usNRegs * 2; i++){
				g_reg_holding_data[i] = pucRegBuffer[i];
			}
		}else{
			eStatus = MB_ENORES;
		}
	}

  return eStatus;
}
