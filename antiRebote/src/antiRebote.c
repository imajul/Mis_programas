/*=============================================================================
 * Author: Ignacio Majul <imajul89@gmail.com>
 * Date: 2019/09/25
 *===========================================================================*/

/*=====[Inclusions of function dependencies CAMBIO]=================================*/

#include "../../../Mis_programas/antiRebote/inc/antiRebote.h"

#include "sapi.h"

/*=====[Definition macros of private constants]==============================*/

/*=====[Definitions of extern global variables]==============================*/

/*=====[Definitions of public global variables]==============================*/

estados_t estadoPin;

/*=====[Definitions of private global variables]=============================*/

/*=====[Main function, program entry point after power on or reset]==========*/

void actualizarEstado(void);

int main( void )
{

	// ----- Setup -----------------------------------
	boardInit();

	estadoPin=gpioRead(TEC1);
	gpioWrite(LED2,estadoPin);

	// ----- Repeat for ever -------------------------
	while( true )
	{
		actualizarEstado();
		if(estadoPin==UP || estadoPin==DOWN)
		{
			gpioWrite(LED2,!estadoPin);
		}
		delay(1);
	}

	// YOU NEVER REACH HERE, because this program runs directly or on a
	// microcontroller and is not called by any Operating System, as in the
	// case of a PC program.
	return 0;
}
void actualizarEstado(void)
{
	static uint32_t contFalling=0;
	static uint32_t contRising=0;

	switch(estadoPin){

	case UP:
		if(gpioRead(TEC1)==DOWN)
		{
			estadoPin=FALLING;
		}
		break;

	case FALLING:
	if(gpioRead(TEC1)==DOWN)
	{
		if(contFalling>=40)
		{
			estadoPin=DOWN;
			contFalling=0;
		}
		contFalling++;
		break;
	}

	case DOWN:
		if(gpioRead(TEC1)==UP)
		{
			estadoPin=RISING;
		}
		break;

	case RISING:
		if(gpioRead(TEC1)==UP)
		{
			if(contRising>=40)
			{
				estadoPin=UP;
				contRising=0;
			}
			contRising++;
		}
		break;
	}


}
