/*=============================================================================
 * Author: Ignacio Majul <imajul89@gmail.com>
 * Date: 2019/10/09
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/

#include "sapi.h"
#include "reloj.h"
#include "terminal.h"

/*=====[Definition macros of private constants]==============================*/

#define	TIEMPO_CH1  15
#define TIEMPO_CH2  45

/*=====[Definitions of extern global variables]==============================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Implementation of public functions]=============================*/

int setearHora(rtc_t *time)
{
	time->year = 2019;
	time->month = 10;
	time->mday = 01;
	time->wday = 3;
	time->hour = 19;
	time->min = 15;
	time->sec= 45;

	if(rtcInit())
	{
		rtcWrite( time );	// seteo la hora en la estructura time
		return 1;			// devuelvo TRUE si es correcta la incializacion del RTC
	}
	else
	{
		return 0;			// devuelvo FALSE si hay un error de inicializacion
	}
}

void leerVariablesFSM(rtc_t *time)  // implementacion de la FSM
{
	uint16_t lectura=0;

	switch(time->sec)
	{
	case TIEMPO_CH1:  			   // verifico si estoy en el tiempo correspondiente
		lectura = adcRead( CH1 );  // leo el canal CH1
		uartWriteString( UART_USB, "Valor CH1 a los 15 segundos: " );
		imprimir(lectura);         // envio por UART el valor medido
		delay(1000);
		break;

	case TIEMPO_CH2:
		lectura = adcRead( CH2 );
		uartWriteString( UART_USB, "Valor de CH2 a los 45 segundos: " );
		imprimir(lectura);
		delay(1000);
		break;
	}
}

