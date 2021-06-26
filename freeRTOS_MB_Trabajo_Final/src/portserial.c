/*
 /*
 * FreeModbus Libary: LPC4337 Port
 * Copyright (C) 2018 Lucas Dordolo <Dordolo92@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */


/* ----------------------- System includes ----------------------------------*/
#include <stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"


#define SET 				1
#define CLEAR 				0
#define USART_INVALID_PORT	0xFF
#define INT_STATUS 			0x01
#define PENDING_INTERRUPT 	0


//Select ONE UART for ModBus
#define USE_USART0_RS485	( 1 )
#define USE_USART0			( 0 )
#define USE_USART2_USB		( 0 )
#define USE_USART2_ENET		( 0 )
#define USE_USART3_232		( 0 )

typedef struct{
	uint32_t txpinPort;
	uint32_t txpinNum;
	uint32_t txpinFunc;

	uint32_t rxpinPort;
	uint32_t rxpinNum;
	uint32_t rxpinFunc;

	uint32_t dirpinPort;
	uint32_t dirpinNum;
	uint32_t dirpinFunc;
}uart_t;


#if USE_USART0_RS485
	#define	LPC_USART LPC_USART0
	#define	USART_IRQn	USART0_IRQn
	uart_t uart = 	{9,5,7, 	//tx pin
				 	 9,6,7,		//rx pin
					 6,2,2};	//dir pin
#endif

#if USE_USART0
	#define	LPC_USART LPC_USART0
	#define	USART_IRQn	USART0_IRQn
	uart_t uart = 	{6,4,2, 	//tx pin
				 	 6,5,2,		//rx pin
					 0,0,0};	//dir pin
#endif

#if USE_USART2_USB
	#define	LPC_USART LPC_USART2
	#define	USART_IRQn	USART2_IRQn
	uart_t uart = 	{7,1,6, 	//tx pin
				 	 7,2,6,		//rx pin
					 0,0,0};	//dir pin
#endif

#if USE_USART2_ENET
	#define	LPC_USART LPC_USART2
	#define	USART_IRQn	USART2_IRQn
	uart_t uart = 	{1,15,1, 	//tx pin
				 	 1,16,1,	//rx pin
					 0,0,0};	//dir pin
#endif

#if USE_USART3_232
	#define	LPC_USART LPC_USART3
	#define	USART_IRQn	USART3_IRQn
	uart_t uart = 	{2,3,2, 	//tx pin
				 	 2,4,2,		//rx pin
					 0,0,0};	//dir pin
#endif
/* ----------------------- static functions ---------------------------------*/

static UCHAR    ucUsedPort = USART_INVALID_PORT;

/* ----------------------- Start implementation -----------------------------*/
#if USE_USART0_RS485
void UART0_IRQHandler(void)
#endif
#if USE_USART0
void UART0_IRQHandler(void)
#endif
#if USE_USART2_USB
void UART2_IRQHandler(void)
#endif
#if USE_USART2_ENET
void UART2_IRQHandler(void)
#endif
#if USE_USART3_232
void UART3_IRQHandler(void)
#endif
{
	uint8_t iirvalue;
    BOOL            bTaskWoken = FALSE;

    vMBPortSetWithinException( TRUE );
	// Read interrupt identification register
	while(((iirvalue = Chip_UART_ReadIntIDReg(LPC_USART)) & INT_STATUS) == PENDING_INTERRUPT){
		iirvalue = iirvalue & UART_IIR_INTID_MASK;

		switch(iirvalue){ // Interrupt source
			case UART_IIR_INTID_RDA: // Receive data available
			case UART_IIR_INTID_CTI: // Character Time-out
				bTaskWoken = pxMBFrameCBByteReceived(  );
				break;
			case UART_IIR_INTID_THRE: // Transmitter holding register empty
				bTaskWoken = pxMBFrameCBTransmitterEmpty(  );
				break;
			case UART_IIR_INTID_RLS:  // Receive line status
				Chip_UART_ReadLineStatus(LPC_USART);
				break;
		}
	}

	vMBPortSetWithinException( FALSE );

	portEND_SWITCHING_ISR( bTaskWoken ? pdTRUE : pdFALSE );
	    //rx_status = Chip_UART_ReadLineStatus(LPC_USART) & (UART_LSR_OE | UART_LSR_PE | UART_LSR_BI | UART_LSR_FE | UART_LSR_RXFE);

}

void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
	if(xRxEnable){
		Chip_UART_IntEnable(LPC_USART, UART_IER_RBRINT);
	}
	else
	{
		Chip_UART_IntDisable(LPC_USART, UART_IER_RBRINT);
	}
	if(xTxEnable){
		Chip_UART_IntEnable(LPC_USART, UART_IER_THREINT);
		pxMBFrameCBTransmitterEmpty(  );
	}
	else
	{
		Chip_UART_IntDisable(LPC_USART, UART_IER_THREINT);
	}
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    BOOL bInitialized = TRUE;

    USHORT cfg = 0;

    //Configure data length
    switch ( ucDataBits )
    {
    case 5:
		cfg |= UART_LCR_WLEN5;
        break;

    case 6:
        cfg |= UART_LCR_WLEN6;
        break;

    case 7:
        cfg |= UART_LCR_WLEN7;
        break;

    case 8:
        cfg |= UART_LCR_WLEN8;
        break;

    default:
        bInitialized = FALSE;
    }
    //Configure Stop bits and parity
    switch ( eParity )
    {
    case MB_PAR_NONE:
        cfg |= UART_LCR_PARITY_DIS|UART_LCR_SBS_2BIT;	//N2
        break;

    case MB_PAR_ODD:
        cfg |= UART_LCR_PARITY_ODD|UART_LCR_PARITY_EN|UART_LCR_SBS_1BIT;	//O1
        break;

    case MB_PAR_EVEN:
        cfg |= UART_LCR_PARITY_EVEN|UART_LCR_PARITY_EN|UART_LCR_SBS_1BIT;	//E1
        break;

    default:
        bInitialized = FALSE;
    }

    // Initialize UART
    Chip_UART_Init( LPC_USART );
    // Set Baud rate
    Chip_UART_SetBaud( LPC_USART, ulBaudRate );  // por defecto 8N1. No me convence el càlculo de baud
    // Restart FIFOS using FCR (FIFO Control Register).
    // Set Enable, Reset content, set trigger level
    Chip_UART_SetupFIFOS( LPC_USART,
                          UART_FCR_FIFO_EN |
                          UART_FCR_TX_RS   |
                          UART_FCR_RX_RS   |
                          UART_FCR_TRG_LEV0 );
    // Dummy read
    Chip_UART_ReadByte( LPC_USART );
    // Enable UART Transmission
    Chip_UART_TXEnable( LPC_USART );
    // Configure SCU UARTn_TXD pin
    Chip_SCU_PinMux( uart.txpinPort,uart.txpinNum,MD_PDN,uart.txpinFunc );
    // Configure SCU UARTn_RXD pin
    Chip_SCU_PinMux( uart.rxpinPort, uart.rxpinNum, MD_PLN | MD_EZI | MD_ZI, uart.rxpinFunc );

    // Specific configurations for RS485
    if( USE_USART0_RS485 == 1 ) {
       // Specific RS485 Flags
       Chip_UART_SetRS485Flags( LPC_USART, UART_RS485CTRL_DCTRL_EN | UART_RS485CTRL_OINV_1 );
       // UARTn_DIR extra pin for RS485
       Chip_SCU_PinMux( uart.dirpinPort, uart.dirpinNum, MD_PDN, uart.dirpinFunc );
    }


	Chip_UART_ConfigData(LPC_USART, (cfg));

	// Determines and sets best dividers to get a target baud rate
	//Chip_UART_SetBaudFDR(LPC_USART0, ulBaudRate); //este es el que va!!!!
	NVIC_SetPriority(USART_IRQn, 5);
    NVIC_EnableIRQ(USART_IRQn);

    Chip_UART_ReadIntIDReg(LPC_USART);

    return bInitialized;
}

void
vMBPortSerialClose( void )
{
    if( USART_INVALID_PORT != ucUsedPort )
    {
        Chip_UART_DeInit(LPC_USART);
        ucUsedPort = USART_INVALID_PORT;
    }
}


/* Put a byte in the UARTs transmit buffer. This function is called
 * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
 * called. */
BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
	   // Wait for space in FIFO (blocking)
	while( (Chip_UART_ReadLineStatus( LPC_USART ) & UART_LSR_THRE) == FALSE );
	   // Send byte
	Chip_UART_SendByte( LPC_USART, ucByte );
    return TRUE;
}


/* Return the byte in the UARTs receive buffer. This function is called
 * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
 */
BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    BOOL retVal = TRUE;
    if ( Chip_UART_ReadLineStatus( LPC_USART ) & UART_LSR_RDR ) {
    	*pucByte = Chip_UART_ReadByte( LPC_USART );
    } else {
       retVal = FALSE;
    }
    return retVal;
}



