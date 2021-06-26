/*=============================================================================
 * Author: Ignacio Majul <imajul89@gmail.com>
 * Date: 2019/09/25
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/

#include "../../../Mis_programas/semaforoFSM/inc/semaforoFSM.h"

#include "sapi.h"



/*=====[Definition macros of private constants]==============================*/

/*=====[Definitions of extern global variables]==============================*/

/*=====[Definitions of public global variables]==============================*/

estado_t estadoSemaforo;

/*=====[Definitions of private global variables]=============================*/

/*=====[Main function, program entry point after power on or reset]==========*/
void iniciarSemaforo( void );
void actualizarSemaforo( void );

int main( void )
{
	// ----- Setup -----------------------------------
	boardInit();
	iniciarSemaforo();

	// ----- Repeat for ever -------------------------
	while( true )
	{
		actualizarSemaforo();
		delay(1);
	}

	// YOU NEVER REACH HERE, because this program runs directly or on a
	// microcontroller and is not called by any Operating System, as in the
	// case of a PC program.
	return 0;
}

void iniciarSemaforo( void )
{
	estadoSemaforo=ROJO;
}

void actualizarSemaforo( void )
{
	static int32_t contador;
	contador++;

	switch(estadoSemaforo){
	case ROJO:
		gpioWrite( LED1, OFF );
		gpioWrite( LED2, ON );
		gpioWrite( LED3, OFF );
		if(contador==1000)
		{
			estadoSemaforo=ROJO_AMARILLO;
			contador=0;
		}
		break;

	case ROJO_AMARILLO:
		gpioWrite( LED1, ON);
		gpioWrite( LED2, ON );
		gpioWrite( LED3, OFF );
		if(contador==500)
		{
			estadoSemaforo=VERDE;
			contador=0;
		}
		break;

	case VERDE:
		gpioWrite( LED1, OFF );
		gpioWrite( LED2, OFF );
		gpioWrite( LED3, ON );
		if(contador==1500)
		{
			estadoSemaforo=AMARILLO;
			contador=0;
		}
		break;

	case AMARILLO:
		gpioWrite( LED1, ON );
		gpioWrite( LED2, OFF );
		gpioWrite( LED3, OFF );
		if(contador==500)
		{
			estadoSemaforo=ROJO;
			contador=0;
		}
		break;

	}

}

