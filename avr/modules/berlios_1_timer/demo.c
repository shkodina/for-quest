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


#define CLIENT_ID 4
#define BAUDRATE 38400


/* ----------------------- Static variables ---------------------------------*/
volatile static unsigned char	g_reg_holding_device [REG_HOLDING_NREGS_IN_BYTES];
volatile static unsigned char	g_reg_holding_data [REG_HOLDING_DATA_NREGS_IN_BYTES];

//==================================================================================
inline
void sSeg_clear(){
	PORTB |= 0b00000011;
	PORTC |= 0b00111111;
	PORTD |= 0b01111000;
}

inline
void sSeg_set_value(char value){

	switch (value){
		case 0:
			PORTC &= 0b11101000; 
			PORTD &= 0b11100111; 
			break;
		case 1:
			PORTC &= 0b11101011; 
			break;
		case 2:
			PORTC &= 0b11110010; 
			PORTD &= 0b11100111; 
			break;
		case 3:
			PORTC &= 0b11100010; 
			PORTD &= 0b11110111; 
			break;
		case 4:
			PORTC &= 0b11100001; 
			break;
		case 5:
			PORTC &= 0b11100100; 
			PORTD &= 0b11110111; 
			break;
		case 6:
			PORTC &= 0b11100100; 
			PORTD &= 0b11100111; 
			break;
		case 7:
			PORTC &= 0b11101010; 
			PORTD &= 0b11111111; 
			break;
		case 8:
			PORTC &= 0b11100000; 
			PORTD &= 0b11100111; 
			break;
		case 9:
			PORTC &= 0b11100000; 
			PORTD &= 0b11110111; 
			break;
		default:
			break;

	}
}

inline
void sSeg_print (char number, char value){
	sSeg_clear();
	switch (number){
		case 0:
			PORTB &= 0b11111101;
			break;
		case 1:
			PORTB &= 0b11111110;
			// put point
			PORTC &= 0b11011111;
			break;
		case 2:
			PORTD &= 0b10111111;
			break;
		case 3:
			PORTD &= 0b11011111;
			break;
		default:
			break;
	}
	sSeg_set_value(value);
}

/*
PD5
PD6
PB0
PB1
select digit


PC0 - PC5 PD3 PD4 select segment

*/

void sSeg_init(){
	DDRB |= 0b00000011;
	DDRC |= 0b00111111;
	DDRD |= 0b01111000;

	sSeg_clear();

}

//==================================================================================
char segpos = 0;
char count = 0;

/* ----------------------- Start implementation -----------------------------*/
int
main( void )
{
	sSeg_init();

    const UCHAR     ucSlaveID[] = { 0xAA, 0xBB, 0xCC };
    eMBErrorCode    eStatus;

 	 

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

		
//		sSeg_print(g_reg_holding_data[1], g_reg_holding_data[0]);

		if (count++ % 3 == 0){
			segpos = segpos++ & 0x03;
			sSeg_print(segpos, g_reg_holding_data[segpos]);
		}


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
