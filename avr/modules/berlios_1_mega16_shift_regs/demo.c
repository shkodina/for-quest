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
#include "avr/delay.h"
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


#define CLIENT_ID 10
#define BAUDRATE 38400


/* ----------------------- Static variables ---------------------------------*/
volatile static unsigned char	g_reg_holding_device [REG_HOLDING_NREGS_IN_BYTES];
volatile static unsigned char	g_reg_holding_data [REG_HOLDING_DATA_NREGS_IN_BYTES];

/* ----------------------- Start implementation -----------------------------*/
#define INVBIT(port, bit) port = port ^ (1<<bit);
#define UPBIT(port, bit) port = port | (1<<bit);
#define DOWNBIT(port, bit) port = port & (~(1<<bit));
//-----------------------------------------------------------------------------
#define REGPORT PORTA
#define REGDDRPORT DDRA

#define DATAPIN 6
#define LATCH 5
#define CLOCK 4

#define REGCOUNT 1
volatile static unsigned char reg_data[REGCOUNT];

void reg_init (){
	REGDDRPORT |= 0b01110000;
	REGPORT &= 0b10001111;
}

void reg_put_data(){
	
	for (char j = 0; j < REGCOUNT; j++){
		for (char i = 0; i < 8; i++){
			if (reg_data[j] & 0b10000000){ // 1
				UPBIT(REGPORT, DATAPIN);				
			}else{ //0
				DOWNBIT(REGPORT, DATAPIN);
			}	
			UPBIT(REGPORT, CLOCK);
			DOWNBIT(REGPORT, CLOCK);

			reg_data[j] = reg_data[j] << 1;
		}
	}
	UPBIT(REGPORT, LATCH);
	DOWNBIT(REGPORT, LATCH);
}
//-----------------------------------------------------------------------------

#define INREGPORT PORTC
#define INREGPPIN PINC
#define INREGDDRPORT DDRC

#define INDATAPIN 5
#define INLATCH 3
#define INCLOCK 4

#define INREGCOUNT 3
volatile static unsigned char inreg_data[INREGCOUNT];

void inreg_init (){
	UPBIT(INREGDDRPORT, INLATCH);
	UPBIT(INREGDDRPORT, INCLOCK);

	UPBIT(INREGPORT, INLATCH);
	DOWNBIT(INREGPORT, INCLOCK);
}

unsigned char inreg_get_data(unsigned char position){
	
	DOWNBIT(INREGPORT, INLATCH);
	_delay_us(300);
	UPBIT(INREGPORT, INLATCH);
	unsigned char data = 0;

	for (char j = 0; j < INREGCOUNT; j++){
		inreg_data[j] = 0;
		for (char i = 0; i < 8; i++){
			inreg_data[j] = inreg_data[j] << 1;
			inreg_data[j] += (PINC >> INDATAPIN) & 0b00000001; 
			DOWNBIT(INREGPORT, INCLOCK);
			_delay_us(300);
			UPBIT(INREGPORT, INCLOCK);
		}
	}

	return inreg_data[position];
}
//---------------------------------------------------------------------------------------
int
main( void )
{

reg_init();
inreg_init ();


    const UCHAR     ucSlaveID[] = { 0xAA, 0xBB, 0xCC };
    eMBErrorCode    eStatus;

    eStatus = eMBInit( MB_RTU, CLIENT_ID, 0, BAUDRATE, MB_PAR_NONE );

    //eStatus = eMBSetSlaveID( 0x34, TRUE, ucSlaveID, 3 );
    sei(  );

    /* Enable the Modbus Protocol Stack. */
    eStatus = eMBEnable(  );

    for( ;; )
    {
		g_reg_holding_device[1] = inreg_get_data(0);
		g_reg_holding_device[2] = inreg_data[1];
		g_reg_holding_device[3] = inreg_data[2];


        ( void )eMBPoll(  );

        /* Here we simply count the number of poll cycles. */
        g_reg_holding_device[0]++;

		if (g_reg_holding_data[1] != 0){
			reg_data[0] = g_reg_holding_data[1];
			reg_put_data();
			g_reg_holding_data[1] = 0;
		}

		

    }
}

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
//			if (g_reg_holding_device[2]++ == 255)
//				g_reg_holding_device[3]++;

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
