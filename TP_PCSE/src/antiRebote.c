/*=============================================================================
 * Author: Ignacio Majul <imajul89@gmail.com>
 * Date: 2019/09/25
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/

#include "antiRebote.h"
#include "sapi.h"

/*=====[Definition macros of private constants]==============================*/

/*=====[Definitions of extern global variables]==============================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Main function, program entry point after power on or reset]==========*/

estados_t actualizarEstado(estados_t estadoTecla)
{
	static uint32_t contFalling=0;
	static uint32_t contRising=0;

	switch(estadoTecla){

	case UP:
		if(gpioRead(TEC1)==DOWN)
		{
			estadoTecla=FALLING;
		}
		break;

	case FALLING:
		if(gpioRead(TEC1)==DOWN)
		{
			if(contFalling>=40)
			{
				estadoTecla=DOWN;
				contFalling=0;
			}
			contFalling++;
			break;
		}

	case DOWN:
		if(gpioRead(TEC1)==UP)
		{
			estadoTecla=RISING;
		}
		break;

	case RISING:
		if(gpioRead(TEC1)==UP)
		{
			if(contRising>=40)
			{
				estadoTecla=UP;
				contRising=0;
			}
			contRising++;
		}
		break;
	}

	return estadoTecla;
}

