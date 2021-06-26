/*=============================================================================
 * Author: Ignacio Majul <imajul89@gmail.com>
 * Date: 2019/10/09
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/

#include "sapi.h"
#include "TP_PdM.h"
#include "reloj.h"
#include "terminal.h"

/*=====[Definition macros of private constants]==============================*/

/*=====[Definitions of extern global variables]==============================*/

/*=====[Definitions of public global variables]==============================*/

rtc_t now;  // variable de tipo estructura donde almacenar la hora

uint16_t tension1 = 0;     // variables a leer para almacenar las mediciones del ADC
uint16_t tension2 = 0;

/*=====[Definitions of private global variables]=============================*/

/*=====[Main function, program entry point after power on or reset]==========*/

int main( void )
{
	// ----- Setup -----------------------------------

	boardInit(); // inicializo la placa EDU-CIAA
	uartConfig( UART_USB, 115200 ); // inicializo la UART
	adcInit( ADC_ENABLE );  // inicializo el ADC

	if( !setearHora(&now) ) // inicializo la hora en la variable "now"
	{
		uartWriteString( UART_USB, "Error al iniciar el RTC" );
		uartWriteString( UART_USB, ";\r\n" );
		return 0;
	}

	uartWriteString( UART_USB, "Inicio exitoso del RTC" );
	uartWriteString( UART_USB, ";\r\n" );

	// ----- Repetir por siempre -------------------------

	while( true )
	{
		leerVariablesFSM(&now);  // FSM para lectura de variables analogicas
	}

	return 0;
}


