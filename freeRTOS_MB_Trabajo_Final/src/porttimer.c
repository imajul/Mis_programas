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
 * File: $Id: porttimer.c,v 1.1 2007/04/24 23:15:18 wolti Exp $
 */

/* ----------------------- Platform includes --------------------------------*/
#include <stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "chip.h"


#define LPC4337_MAX_FREC 204000000
#define TIMERCOMPAREMATCH0	0

#define USE_TIMER0	( 1 )
#define USE_TIMER1	( 0 )
#define USE_TIMER2	( 0 )
#define USE_TIMER3	( 0 )

#if USE_TIMER0
	#define RGU_TIMER_RST RGU_TIMER0_RST
	#define TIMER_IRQn TIMER0_IRQn
	#define LPC_TIMER LPC_TIMER0
#endif

#if USE_TIMER1
	#define RGU_TIMER_RST RGU_TIMER1_RST
	#define TIMER_IRQn TIMER1_IRQn
	#define LPC_TIMER LPC_TIMER1
#endif

#if USE_TIMER2
	#define RGU_TIMER_RST RGU_TIMER2_RST
	#define TIMER_IRQn TIMER2_IRQn
	#define LPC_TIMER LPC_TIMER2
#endif

#if USE_TIMER3
	#define RGU_TIMER_RST RGU_TIMER3_RST
	#define TIMER_IRQn TIMER3_IRQn
	#define LPC_TIMER LPC_TIMER3
#endif
/* ----------------------- static functions ---------------------------------*/


/* ----------------------- Start implementation -----------------------------*/


BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
	uint32_t tickss;

	tickss = 50*(LPC4337_MAX_FREC/1000000); //50uS to ticks

	/* Enable timer clock and reset it */
	Chip_TIMER_Init(LPC_TIMER);
	Chip_RGU_TriggerReset(RGU_TIMER_RST);
	while (Chip_RGU_InReset(RGU_TIMER_RST)) {}
	Chip_TIMER_Reset(LPC_TIMER);

	/* Initialize compare match with the specified ticks (number of counts needed to clear the match counter) */
	Chip_TIMER_MatchEnableInt( LPC_TIMER, TIMERCOMPAREMATCH0 );
	Chip_TIMER_SetMatch( LPC_TIMER, TIMERCOMPAREMATCH0, tickss*usTim1Timerout50us );

	/* Makes Timer Match 0 period the timer period*/
	Chip_TIMER_ResetOnMatchEnable( LPC_TIMER, TIMERCOMPAREMATCH0 );
	/*Enable timer*/
	Chip_TIMER_Disable(LPC_TIMER);

	/* Enable timer interrupt */
	NVIC_SetPriority(TIMER_IRQn, 6);
	NVIC_EnableIRQ(TIMER_IRQn);
	NVIC_ClearPendingIRQ(TIMER_IRQn);

    return TRUE;
}

void
vMBPortTimersDelay( USHORT usTimeOutMS )
{
    vTaskDelay( usTimeOutMS / portTICK_RATE_MS );
}

void
vMBPortTimersEnable(  )
{
	LPC_TIMER->TCR = 0x02;	//Clear TC
	LPC_TIMER->TCR = 0x01;	//Enable
}

void
vMBPortTimersDisable(  )
{
    Chip_TIMER_Disable(LPC_TIMER);
}


#if USE_TIMER0
	void TIMER0_IRQHandler(void)
#endif
#if USE_TIMER1
	void TIMER1_IRQHandler(void)
#endif
#if USE_TIMER2
	void TIMER2_IRQHandler(void)
#endif
#if USE_TIMER3
	void TIMER3_IRQHandler(void)
#endif
{
   uint8_t compareMatchNumber = 0;
   BOOL    bTaskWoken = FALSE;

   vMBPortSetWithinException( TRUE );


	if( Chip_TIMER_MatchPending(LPC_TIMER, compareMatchNumber) ) {
	 /*Run the functions saved in the timer dynamic data structure*/
	  bTaskWoken = pxMBPortCBTimerExpired(  );
	 Chip_TIMER_ClearMatch(LPC_TIMER, compareMatchNumber);
	}


	vMBPortSetWithinException( FALSE );
	portEND_SWITCHING_ISR( bTaskWoken ? pdTRUE : pdFALSE );
}

