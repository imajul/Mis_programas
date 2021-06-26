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
#include "DS18B20.h"


/* Select a TimeEvents choise	*/
//#define __USE_TIME_EVENTS (false)	/* "false" without TimeEvents */
#define __USE_TIME_EVENTS (true)	/* or "true" with TimerEvents */

DEBUG_PRINT_ENABLE;

#define TICKRATE_1MS	(1000)				/* 1000 ticks per second */
#define TICKRATE_MS		(TICKRATE_1MS)	/* 1000 ticks per second */

/*! This is a timed state machine that requires timer services */
//#if (__USE_TIME_EVENTS == true)
//	#define NOF_TIMERS (sizeof(PrefixTimeEvents)/sizeof(sc_boolean))
//#else
//	#define NOF_TIMERS 0
//#endif
//TimerTicks ticks[NOF_TIMERS];
///*! This is a state machine */
//static Prefix statechart;
//volatile bool SysTick_Time_Flag = false;
//volatile bool Volver_Control_Flag = false;

/*!
 * This is a timed state machine that requires timer services
 */

//#if (__USE_TIME_EVENTS == true)
/*! This function has to set up timers for the time events that are required by the state machine. */
/*!
	This function will be called for each time event that is relevant for a state when a state will be entered.
	\param evid An unique identifier of the event.
	\time_ms The time in milli seconds
	\periodic Indicates the the time event must be raised periodically until the timer is unset
*/
//extern void prefixIface_selec_Pantalla(const Prefix* handle, const sc_integer OpPantalla);


/* PROTOTIPOS DE FUNCIONES
/*Pantalla Control */

/*Pantalla Correccion */



int main(void){

		#if (__USE_TIME_EVENTS == true)
		uint32_t i;
		#endif

		int temp_1, temp_2;
		char str[5], buffer9[9], scratchbuffer[9];

		/* Inicializar la placa */
	   boardConfig();
	   /*Configuro el puerto del sensor de temperatura como entrada.*/
	   gpioConfig( GPIO7, GPIO_INPUT );
	   gpioConfig( GPIO3, GPIO_OUTPUT );
	   /*Configuro el puerto del sensor de temperatura como entrada.*/
	   gpioConfig( GPIO8, GPIO_INPUT );

	   	/* ------------- INICIALIZACIONES ------------- */

	   	/* Inicializar UART_USB como salida de consola */
//	   uartConfig( UART_USB, 115200 );
	   	/* UART for debug messages. */
	   	debugPrintConfigUart( UART_USB, 115200 );
	//   	debugPrintString( "DEBUG c/sAPI\r\n" );



	   /*Configuro owire para sensor de temp*/


	   //SystemCoreClockUpdate();
	   //SysTick_Config(SystemCoreClock / 1000);
	   //SysTick_Config(SystemCoreClock / 1000000);



	   /* Init Ticks counter => TICKRATE_MS */
		tickConfig( TICKRATE_MS );

		/* Add Tick Hook */
//		tickCallbackSet( myTickHook, (void*)NULL );
//		/* Statechart Initialization */
//		#if (__USE_TIME_EVENTS == true)
//		InitTimerTicks(ticks, NOF_TIMERS);
//		#endif


		//owInit(2,8);
		owInit(GPIO7);
		owSetBitResolution(GPIO7,12);

		//owWriteScratch(3,7);


	   /* ------------- REPETIR POR SIEMPRE ------------- */
	   while(1) {

			//temp_1 = owReadTemperature(2,8);
		   //owSetBitResolution(GPIO7,9);
			temp_2 = owReadTemperature(GPIO7);

			owReadScratch(buffer9,3,7);
			//temp_2 = temp_2 >> 4;
			//temp_1= temp_1 >> 4;
	//		dist = ultrasonicSensorGetDistance(ULTRASONIC_SENSOR_0, CM);
	//		sprintf(d_txt,"%f",dist);
//		sprintf(str, "%d.%04d\r\n", temp_2 >> 4, (temp_2 & 0xF) * 625);
			sprintf(str, "%d.%04d   %d\r\n", temp_2>>4, (temp_2 & 0xF) * 625, buffer9[4]>>5);
			debugPrintString(str);
	//		Vol_1 = (h_t-dist)*r2*3.1416;
	//		sprintf(vol_1_txt,"%f",Vol_1);
			delay(1000);

		}

	   /* NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa no es llamado
	      por ningun S.O. */

   return 0 ;
}


