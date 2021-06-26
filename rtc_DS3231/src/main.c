/* Copyright 2017, Pablo Ridolfi, Juan Esteban Alarcón, Juan Manuel Cruz
 * All rights reserved.
 *
 * This file is part of Workspace.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/** @brief This is a simple statechart example using Yakindu Statechart Tool
 * Plug-in (update site: http://updates.yakindu.org/sct/mars/releases/).
 */

/** \addtogroup statechart Simple UML Statechart example.
 ** @{ */

/*==================[inclusions]=============================================*/

//#include "main.h"
#include "sapi.h"       // <= sAPI header
#include "rtc_DS3231.h"

DEBUG_PRINT_ENABLE;
#define RTC_SET			OFF
#define PRINT_DEBUG 	OFF


#define	DS3231_I2C_ADDRESS	0x68

// SETEO DE LA HORA
#define ANIO 		21
#define MES 		5
#define DIA_MES 	8
#define DIA_SEMANA 	7
#define HORA 		16
#define MINUTOS		16
#define SEGUNDOS	20

// SETEO DE LA ALARMA
#define ALARMA_HORA 	20
#define ALARMA_MINUTOS 	30
#define ALARMA_SEGUNDOS 5
#define ALARMA_DIA		0


void My_IRQ_Init (void);

rtcDS3231_t time;

int main(void)
{
	boardConfig();
	gpioConfig( GPIO0, GPIO_INPUT_PULLUP);
	My_IRQ_Init();

	debugPrintConfigUart( UART_USB, 115200 );
	debugPrintString( "DEBUG c/sAPI\r\n" );

	i2cInit( I2C0, 100000 );
	debugPrintlnString( "I2C initialization complete." );

#if RTC_SET

	RTC_Init(&time,ANIO,MES,DIA_MES,DIA_SEMANA,HORA,MINUTOS,SEGUNDOS);  // inicializo la estructura time con los registros horarios
	debugPrintlnString( "RTC initialization complete." );

	RTC_write_time(&time, I2C0, DS3231_I2C_ADDRESS);  // cargo la hora en el RTC DS3231

	RTC_set_alarm_time(&time,I2C0, DS3231_I2C_ADDRESS,0,ALARMA_HORA,ALARMA_MINUTOS,ALARMA_SEGUNDOS,SECONDS_MATCH);
	RTC_turn_alarm_on(&time, I2C0, DS3231_I2C_ADDRESS);

#endif

	RTC_reset_alarm(&time, I2C0, DS3231_I2C_ADDRESS);

	while(1)
	{

#if	PRINT_DEBUG

		RTC_read_time( &time, I2C0, DS3231_I2C_ADDRESS);  // leo los registros horarios del RTC y los guardo en la estructura time

		debugPrintHex( time.hour,8 );   // imprimo la hora por UART
		debugPrintString(":");
		debugPrintHex( time.min,8 );
		debugPrintString(":");
		debugPrintHex( time.sec,8 );
		debugPrintString("  ");
		debugPrintHex( time.mday,8 );
		debugPrintString("/");
		debugPrintHex( time.month,8 );
		debugPrintString("/");
		debugPrintHex( time.year,8 );

		RTC_read_temp( &time, I2C0, DS3231_I2C_ADDRESS);

		debugPrintString(" Temp: ");	// imprimo la temperatura por UART
		debugPrintInt(time.MSB_temp);
		debugPrintString(",");
		debugPrintInt(time.LSB_temp);

		RTC_read_control_registers( &time, I2C0, DS3231_I2C_ADDRESS);

		debugPrintString(" Control register: ");	// imprimo la temperatura por UART
		debugPrintUIntFormat(time.control, 2);

		debugPrintString(" Control/Status register: ");	// imprimo la temperatura por UART
		debugPrintUIntFormat(time.control_status, 2);

		debugPrintString("\r\n ");
		delay(1000);

#else
	    sleepUntilNextInterrupt();
#endif

	}

   return 0 ;
}

void GPIO0_IRQHandler(void)
{
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH0); // Borramos el flag de interrupción
	RTC_reset_alarm(&time, I2C0, DS3231_I2C_ADDRESS);

	RTC_read_time( &time, I2C0, DS3231_I2C_ADDRESS);  // leo los registros horarios del RTC y los guardo en la estructura time
	debugPrintString(" ALARMA !!  ");
	debugPrintHex( time.hour,8 );   // imprimo la hora por UART
	debugPrintString(":");
	debugPrintHex( time.min,8 );
	debugPrintString("  ");
	debugPrintHex( time.mday,8 );
	debugPrintString("/");
	debugPrintHex( time.month,8 );
	debugPrintString("/");
	debugPrintHex( time.year,8 );
	debugPrintString("\r\n ");

}

void My_IRQ_Init (void)
{
	//Chip_PININT_Init(LPC_GPIO_PIN_INT);
	NVIC_ClearPendingIRQ( PIN_INT0_IRQn );
	Chip_SCU_GPIOIntPinSel(0, 3, 0);  // Canal 0, Puerto 3, Pin 0 correspondiente al pin GPIO0 de la EDU-CIAA

	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH0);
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH0);
	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH0);

	NVIC_SetPriority(PIN_INT0_IRQn, 5);

	NVIC_EnableIRQ(PIN_INT0_IRQn);
}


