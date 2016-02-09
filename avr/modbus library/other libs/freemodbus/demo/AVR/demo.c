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
 * File: $Id: excoils.c,v 1.7 2006/09/17 16:46:33 wolti Exp $
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbutils.h"

/* ----------------------- Defines ------------------------------------------*/
#define REG_DISC_START     1000
#define REG_DISC_SIZE      16
#define REG_COILS_START     1000
#define REG_COILS_SIZE      16
/* ----------------------- Static variables ---------------------------------*/
static unsigned char ucRegDiscBuf[REG_DISC_SIZE / 8];
static unsigned char ucRegCoilsBuf[REG_COILS_SIZE / 8];
/* ----------------------- Start implementation -----------------------------*/
int
main( void )
{
    DDRB = 0xff;
  	DDRC=0xff;
	  PORTD=0x00;
    DDRD=0x00;
    PORTA=0x00;
    DDRA=0x00;
   
   
  
   /* Select either ASCII or RTU Mode. */
    ( void )eMBInit( MB_RTU, 0x0A, 0, 9600, MB_PAR_NONE );
    
   //xMBUtilSetBits( ucRegDiscBuf, 2, 2, 3 );    /* Set bit 2:3 to 11b. */
    //xMBUtilSetBits( ucRegDiscBuf, 8, 1, 1 );    /* Set bit 8 to 1b. */
    
   /* Enable the Modbus Protocol Stack. */
    ( void )eMBEnable(  );
    for( ;; )
    {
        /* Call the main polling loop of the Modbus protocol stack. */
      
    
      
   
    /* Check if we have registers mapped at this block. */
   
      ( void )eMBPoll(  );
             
        if (xMBUtilGetBits( ucRegCoilsBuf, 0, 1 )==1)
        {
       
         PORTB |= _BV(PB0);
        
      }else 
       
         {
         PORTB &= ~_BV(PB0);
         }
         
       if (xMBUtilGetBits( ucRegCoilsBuf, 1, 1 )==1)
         {
          PORTB |= _BV(PB1);
         
       }else 
       
         {
          PORTB &= ~_BV(PB1);
         }
      if (xMBUtilGetBits( ucRegCoilsBuf, 2, 1 )==1)
      {
       PORTB |= _BV(PB2);
      }else 
       
       {
       PORTB &= ~_BV(PB2);
       }
      if (xMBUtilGetBits( ucRegCoilsBuf, 3, 1 )==1)
      {
       PORTB |= _BV(PB3);
      }else 
       
       {
       PORTB &= ~_BV(PB3);
       }
             
      if (xMBUtilGetBits( ucRegCoilsBuf, 4, 1 )==1)
      {
       PORTB |= _BV(PB4);
      }else 
       
       {
       PORTB &= ~_BV(PB4);
       }
      
      if (xMBUtilGetBits( ucRegCoilsBuf, 5, 1 )==1)
      {
       PORTB |= _BV(PB5);
      }else 
       
       {
       PORTB &= ~_BV(PB5);
       }
      
      if (xMBUtilGetBits( ucRegCoilsBuf, 6, 1 )==1)
      {
       PORTB |= _BV(PB6);
      }else 
       
       {
       PORTB &= ~_BV(PB6);
       }
      
      if (xMBUtilGetBits( ucRegCoilsBuf, 7, 1 )==1)
      {
       PORTB |= _BV(PB7);
      }else 
       
       {
       PORTB &= ~_BV(PB7);
       }
      
       if (xMBUtilGetBits( ucRegCoilsBuf, 8, 1 )==1)
        {
       
         PORTC |= _BV(PC0);
        
      }else 
       
         {
         PORTC &= ~_BV(PC0);
         }
         
       if (xMBUtilGetBits( ucRegCoilsBuf, 9, 1 )==1)
         {
          PORTC |= _BV(PC1);
         
       }else 
       
         {
          PORTC &= ~_BV(PC1);
         }
      if (xMBUtilGetBits( ucRegCoilsBuf, 10, 1 )==1)
      {
       PORTC |= _BV(PC2);
      }else 
       
       {
       PORTC &= ~_BV(PC2);
       }
      if (xMBUtilGetBits( ucRegCoilsBuf, 11, 1 )==1)
      {
       PORTC |= _BV(PC3);
      }else 
       
       {
       PORTC &= ~_BV(PC3);
       }
             
      if (xMBUtilGetBits( ucRegCoilsBuf, 12, 1 )==1)
      {
       PORTC |= _BV(PC4);
      }else 
       
       {
       PORTC &= ~_BV(PC4);
       }
      
      if (xMBUtilGetBits( ucRegCoilsBuf, 13, 1 )==1)
      {
       PORTC |= _BV(PC5);
      }else 
       
       {
       PORTC &= ~_BV(PC5);
       }
      
      if (xMBUtilGetBits( ucRegCoilsBuf, 14, 1 )==1)
      {
       PORTC |= _BV(PC6);
      }else 
       
       {
       PORTC &= ~_BV(PC6);
       }
      
      if (xMBUtilGetBits( ucRegCoilsBuf, 15, 1 )==1)
      {
       PORTC |= _BV(PC7);
      }else 
       
       {
       PORTC &= ~_BV(PC7);
       }
      
     
            }
}

eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iNCoils = ( int )usNCoils;
    unsigned short  usBitOffset;

    /* Check if we have registers mapped at this block. */
    if( ( usAddress >= REG_COILS_START ) &&
        ( usAddress + usNCoils <= REG_COILS_START + REG_COILS_SIZE ) )
    {
        usBitOffset = ( unsigned short )( usAddress - REG_COILS_START );
        switch ( eMode )
        {
                /* Read current values and pass to protocol stack. */
            case MB_REG_READ:
                while( iNCoils > 0 )
                {
                    *pucRegBuffer++ =
                        xMBUtilGetBits( ucRegCoilsBuf, usBitOffset,
                                        ( unsigned char )( iNCoils >
                                                           8 ? 8 :
                                                           iNCoils ) );
                    iNCoils -= 8;
                    usBitOffset += 8;
                }
                break;

                /* Update current register values. */
            case MB_REG_WRITE:
                while( iNCoils > 0 )
                {
                    xMBUtilSetBits( ucRegCoilsBuf, usBitOffset, 
                                    ( unsigned char )( iNCoils > 8 ? 8 : iNCoils ),
                                    *pucRegBuffer++ );
                    iNCoils -= 8;
                    usBitOffset += 8;
                }
                break;
        }

    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}


eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    return MB_ENOREG;
}
   

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
    return MB_ENOREG;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )

    
{
    
   eMBErrorCode    eStatus = MB_ENOERR;
    short           iNDiscrete = ( short )usNDiscrete;
    unsigned short  usBitOffset;
   
   
      if (bit_is_clear(PIND,0))
      {
      xMBUtilSetBits( ucRegDiscBuf, 0, 1, 1 );
      }
      else 
      {
      xMBUtilSetBits( ucRegDiscBuf, 0, 1, 0 );
   }
   
      if (bit_is_clear(PIND,1))
      {
      xMBUtilSetBits( ucRegDiscBuf, 1, 1, 1 );
      }
      else 
      {
      xMBUtilSetBits( ucRegDiscBuf, 1, 1, 0 );
   }
   
      if (bit_is_clear(PIND,2))
      {
      xMBUtilSetBits( ucRegDiscBuf, 2, 1, 1 );
      }
      else 
      {
      xMBUtilSetBits( ucRegDiscBuf, 2, 1, 0 );
   }
   
      if (bit_is_clear(PIND,3))
      {
      xMBUtilSetBits( ucRegDiscBuf, 3, 1, 1 );
      }
      else 
      {
      xMBUtilSetBits( ucRegDiscBuf, 3, 1, 0 );
   }
   
      if (bit_is_clear(PIND,4))
      {
      xMBUtilSetBits( ucRegDiscBuf, 4, 1, 1 );
      }
      else 
      {
      xMBUtilSetBits( ucRegDiscBuf, 4, 1, 0 );
   }
   
      if (bit_is_clear(PIND,5))
      {
      xMBUtilSetBits( ucRegDiscBuf, 5, 1, 1 );
      }
      else 
      {
      xMBUtilSetBits( ucRegDiscBuf, 5, 1, 0 );
   }
   
      if (bit_is_clear(PIND,6))
      {
      xMBUtilSetBits( ucRegDiscBuf, 6, 1, 1 );
      }
      else 
      {
      xMBUtilSetBits( ucRegDiscBuf, 6, 1, 0 );
   }
   
      if (bit_is_clear(PIND,7))
      {
      xMBUtilSetBits( ucRegDiscBuf, 7, 1, 1 );
      }
      else 
      {
      xMBUtilSetBits( ucRegDiscBuf, 7, 1, 0 );
   }
   if (bit_is_clear(PINA,0))
      {
      xMBUtilSetBits( ucRegDiscBuf, 8, 1, 1 );
      }
      else 
      {
      xMBUtilSetBits( ucRegDiscBuf, 8, 1, 0 );
   }
   
      if (bit_is_clear(PINA,1))
      {
      xMBUtilSetBits( ucRegDiscBuf, 9, 1, 1 );
      }
      else 
      {
      xMBUtilSetBits( ucRegDiscBuf, 9, 1, 0 );
   }
   
      if (bit_is_clear(PINA,2))
      {
      xMBUtilSetBits( ucRegDiscBuf, 10, 1, 1 );
      }
      else 
      {
      xMBUtilSetBits( ucRegDiscBuf, 10, 1, 0 );
   }
   
      if (bit_is_clear(PINA,3))
      {
      xMBUtilSetBits( ucRegDiscBuf, 11, 1, 1 );
      }
      else 
      {
      xMBUtilSetBits( ucRegDiscBuf, 11, 1, 0 );
   }
   
      if (bit_is_clear(PINA,4))
      {
      xMBUtilSetBits( ucRegDiscBuf, 12, 1, 1 );
      }
      else 
      {
      xMBUtilSetBits( ucRegDiscBuf, 12, 1, 0 );
   }
   
      if (bit_is_clear(PINA,5))
      {
      xMBUtilSetBits( ucRegDiscBuf, 13, 1, 1 );
      }
      else 
      {
      xMBUtilSetBits( ucRegDiscBuf, 13, 1, 0 );
   }
   
      if (bit_is_clear(PINA,6))
      {
      xMBUtilSetBits( ucRegDiscBuf, 14, 1, 1 );
      }
      else 
      {
      xMBUtilSetBits( ucRegDiscBuf, 14, 1, 0 );
   }
   
      if (bit_is_clear(PINA,7))
      {
      xMBUtilSetBits( ucRegDiscBuf, 15, 1, 1 );
      }
      else 
      {
      xMBUtilSetBits( ucRegDiscBuf, 15, 1, 0 );
   }
      
   if( ( usAddress >= REG_DISC_START ) &&
        ( usAddress + usNDiscrete <= REG_DISC_START + REG_DISC_SIZE ) )
    {
        usBitOffset = ( unsigned short )( usAddress - REG_DISC_START );
        while( iNDiscrete > 0 )
        {
            *pucRegBuffer++ =
                xMBUtilGetBits( ucRegDiscBuf, usBitOffset,
                                ( unsigned char )( iNDiscrete >
                                                   8 ? 8 : iNDiscrete ) );
            iNDiscrete -= 8;
            usBitOffset += 8;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}
