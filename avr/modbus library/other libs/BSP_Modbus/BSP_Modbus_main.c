/*
 * PMC_m644P_main.c
 *
 *  Created on: Jan 26, 2011
 *      Author: jgnoss
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include "p_defines.h"
#include "Modbus_uart.h"
#include "JG_Modbus.h"


volatile uint32_t sysSec = 0;
tMBChangedBit changedBit = {99, 0};


volatile int16_t actProfileData[MAX_WordAddress];
volatile uint8_t systemBits[MaxActionBytes];

volatile uint8_t UART_RxBuf[UART_RX_BUFFER_SIZE];
volatile uint8_t UART_RxHead;



uint8_t mesVref = TRUE;

tGLOBAL_FLAGS status = {
//		RS_UP, //rampState;
		0, //act_Step;
		2800, //act_Vel;
		0, //endUp;
		0, //begDown;
		0, //cmdLineMode;
//		PS_Idle, //ProfState;
		0, //act_RProfile;
		0, //act_SProfile;
		0, //act_SProfileIDX
		FALSE, //TC_running:1;
		0, //act_Mode:1;
		0, //act_Dir:1;
		FALSE, //rs232_cmd:1;
		FALSE, //cmdIsNumber:1;
		0 //dummy:3;
};


volatile uart_status_t uart0_status = {
		RS_Wait, // status
		0 // bitTics
};

volatile uart_status_t uart_status = {
		RS_Wait, // status
		0, // bitTics
};


uint8_t getBitValForSysBit(uint8_t b);
void switchUSART (uint8_t how);
void performBitActions();


inline uint8_t setBitValForSysBit(uint8_t b, uint8_t v){
	if (v) {
		systemBits[getByteForSysBit(b)] |= (1 << getBitForSysBit(b));
	} else {
		systemBits[getByteForSysBit(b)] &= ~(1 << getBitForSysBit(b));
	}
	return ((systemBits[getByteForSysBit(b)] & (1 << getBitForSysBit(b))) ? 1 : 0);
}


ISR (TIMER2_COMP_vect) {

	if (uart0_status.status != RS_Wait) {
		uart0_status.bitTics++;
		// this value is reset each time
		// we receive a new byte on UART
		if (uart0_status.bitTics >= FRAME_TIMEOUT_TICS) {
			// here we are if bitTics is more than 3.5 chars
			// that means frame complete or timeout for frame
			uart0_status.status = RS_FrameComplete;
		}
		if ((uart0_status.bitTics <= FRAME_TIMEOUT_TICS)
				&& (uart0_status.bitTics >= BYTE_TIMEOUT_TICS)) {
			// here we are if bitTics exceeds 1.5 chars
			// that means frame is useless
			// anyway we give it to higher instance for check
			// and exception handling
			uart0_status.status = RS_FrameComplete;
		}
	}

}

void initInterrupts(void) {


	// TC2 used for Modbus Timeout and sysSec Counter

	TCCR2 |= ((1<<CS21));
	OCR2 = 32;
	TIMSK |= (1<<OCIE2);

}

void initPorts(void) {

	RS485_CPort |= ((1 << RS485_Send));
	RS485_Port &= ~((1 << RS485_Send));

}

void initSysVars(void) {
	uint8_t i8;

	for (i8 = 0; i8 < MaxActionBytes; i8++) {
		systemBits[i8] = 0;
	}
	for (i8 = 0; i8 < MAX_WordAddress; i8++) {
		actProfileData[i8] = 0;
	}
}

int main(void) {

	uint8_t oldStatus = 0;

	initSysVars();
	initPorts();

	// we use RS485 here, to go with RS232 just have a look at Modbus_uart.c/.h
	// and use the rs232 init function
	rs485_init(UART_BAUD_SELECT(9600,F_CPU));
	initInterrupts();
	sei();

//	actProfileData[IDX_DW_Debug02] = 2;
	actProfileData[IDX_DW_Debug01] = 101;
	actProfileData[IDX_DW_Debug02] = 102;
	actProfileData[IDX_DW_Debug03] = 103;

	actProfileData[IDX_DW_EncSteps] = 200;
	actProfileData[IDX_DW_EncHyst] = 4;
	setBitValForSysBit(IDX_AB_EncSync, ON);


	for (;;) {


		if (oldStatus != uart0_status.status) {
			oldStatus = uart0_status.status;

			if (uart0_status.status == RS_FrameComplete) {
				if (UART_RxHead) {
					modbus_processSlaveFrame((uint8_t*) UART_RxBuf, UART_RxHead);
				}
				uart_flush();
				uart0_status.status = RS_Wait;
			}
		}

		if (changedBit.cBit != 99) {
			performBitActions();
		}
	}

}

void switchUSART (uint8_t how){

	// Enable/Disable USART receiver and receive complete interrupt
	// to ensure we don't get disturbed by another incoming stuff

	if (how) {
		UART0_CONTROL |= ((1<<RXCIE0) | (1 << RXEN0));
	} else {
		UART0_CONTROL &= ~((1<<RXCIE0) | (1 << RXEN0));
	}

}

void performBitActions(){

	// switching the UART off and later on again is just
	// because here I had a really com-hungry HMI connected

	switchUSART(OFF);

	switch (changedBit.cBit) {
	case IDX_PB_Dir:
		if (changedBit.cVal) {
			// action on Bit is 1
			// do some here
		} else {
			// action on Bit is 0
			// do some here
		}
		break;
	}
		changedBit.cBit = 99;
		changedBit.cVal = 0;
		switchUSART(ON);

}








