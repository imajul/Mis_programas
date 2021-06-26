/*=====[app]===================================================================
 * Copyright 2018 Eric Nicolas Pernia <ericpernia@gmail.com>
 * All rights reserved.
 * License: BSD-3-Clause <https://opensource.org/licenses/BSD-3-Clause>)
 *
 * Version: 1.0.0
 * Creation Date: 2018/10/14
 */

/*=====[Inclusions of function dependencies]=================================*/

#include "sapi.h"
#include "sapi_timer_modified.h"
#include "TM1637Display.h"

/*=====[Definition macros of private constants]==============================*/

#define TIMER0_PRESCALER   10
#define CAP_NUMB           2

/*=====[Definitions of extern global variables]==============================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

static volatile bool_t calculated = FALSE;
static volatile uint32_t tCapturadoAnterior = 0;
static volatile uint32_t tCapturadoActual = 0;
static volatile float result = 0.0;

static volatile uint32_t count = 0;
static volatile bool_t canalA = LOW;
static volatile bool_t canalB = LOW;

/*=====[Prototypes (declarations) of private functions]======================*/

static bool_t timer0CaptureCallback( void* ptr );
void My_IRQ_Init (void);

/*=====[Main function, program entry point after power on or reset]==========*/

int main (void)
{
	// ----- Configurations --------------------------

	boardInit(); // Configurar plataforma

	My_IRQ_Init();

	// ENET_TXD1 --> P1.20, FUNC4 --> TIMER0_CAP2
	printf( "Connect DAC output to TXD1 (ETHERNET).\r\n\r\n" );

	gpioConfig( GPIO0, GPIO_INPUT_PULLUP );

	// Configurar Timer 0 en modo INPUT_CAPTURE (solo en flanco de subida)
	timerInputCaptureConfig( TIMER0,           // Timer
			TIMER_CAPTURE2,   // Capture Number
			TIMER0_PRESCALER, // Timer Prescaler
			TRUE,             // Interrupt on Rising edge
			FALSE );          // Interrupt on Falling edge

	timerInputCaptureSetCaptureEvent( TIMER0,                  // Timer
			TIMER_CAPTURE2,          // Capture Number
			timer0CaptureCallback ); // Callback

	// Enable TIMER0
	timerSetPower( TIMER0, ON );

	printf( "TIMER0 Configure as input capture on pin ENET_TXD1 --> T0_CAP2.\r\n\r\n" );

	printf( "F_CPU: 204 MHz.\r\n" );
	printf( "Prescale_Timer: %d.\r\n", timerGetPrescale(TIMER0) );
	printf( "F_Timer: %.5f MHz.\r\n\r\n", (float)timerGetClock(TIMER0)/1000000.0 );

	delay_t delay1s;
	delayInit( &delay1s, 100 ); // Periodic delay 1000 ms = 1 s
    delay(1);
	// ----- Repeat for ever -------------------------
	while(TRUE) {
		if( delayRead(&delay1s) ){
		//	printf( "Tmeasure: %.5f us\r\n", result );
		//	printf( "Fmeasure: %.5f Hz\r\n", 1000000.0/result );
			printf( "Vueltas: %d \r\n", count );
		//	printf( "\r\n" );
		}
	}
	// YOU NEVER REACH HERE, because this program runs directly or on a
	// microcontroller and is not called by any Operating System, as in the
	// case of a PC program.
	return 0;
}

/*=====[Definitions of private functions]====================================*/

// When a RISING EDDGE event ocurs on TIMER0, execute the function
static bool_t timer0CaptureCallback( void* ptr )
{
	if( calculated == FALSE )
	{
		tCapturadoAnterior = timerReadCapture( TIMER0, TIMER_CAPTURE2 );
		calculated = TRUE;
	}
	else
	{
		tCapturadoActual = timerReadCapture( TIMER0, TIMER_CAPTURE2 );
		// Prescale = 10, CPU_CLK = 204MHz, con *1000 da en ms
		result = (float)( tCapturadoActual - tCapturadoAnterior )
            										   / ((float)timerGetClock( TIMER0)/1000000.0);
		tCapturadoAnterior = tCapturadoActual;
	}
	gpioToggle( LED1 ); // This LED blinks at half rate of measure signal becaus
	// only search for rising edges

}

void My_IRQ_Init (void)
{
	//Inicializamos las interrupciones (LPCopen)
	Chip_PININT_Init(LPC_GPIO_PIN_INT);

	//Inicializamos de cada evento de interrupción (LPCopen)

	Chip_SCU_GPIOIntPinSel(0, 3, 0); 							//Mapeo del pin donde ocurrirá el evento y el canal al que lo va a enviar. (Canal 0 a 7, Puerto GPIO, Pin GPIO)
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH0);	//Se configura el canal para que se active por flanco
	Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH0);		//Se configura para que el flanco sea el de bajada

	//		Chip_SCU_GPIOIntPinSel(1, 0, 4);
	//		Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH1);
	//		Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH1);		//En este caso el flanco es de subida
	//
	//		Chip_SCU_GPIOIntPinSel(2, 0, 8);
	//		Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH2);
	//		Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH2);
	//
	//		Chip_SCU_GPIOIntPinSel(3, 0, 8);
	//		Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH3);
	//		Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH3);
	//
	//		Chip_SCU_GPIOIntPinSel(4, 0, 9);
	//		Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH4);
	//		Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH4);
	//
	//		Chip_SCU_GPIOIntPinSel(5, 0, 9);
	//		Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH5);
	//		Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH5);
	//
	//		Chip_SCU_GPIOIntPinSel(6, 1, 9);
	//		Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH6);
	//		Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH6);
	//
	//		Chip_SCU_GPIOIntPinSel(7, 1, 9);
	//		Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH7);
	//		Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH7);


	//Una vez que se han configurado los eventos para cada canal de interrupcion
	//Se activan las interrupciones para que comiencen a llamar al handler

	NVIC_SetPriority(PIN_INT0_IRQn, 7);
	NVIC_EnableIRQ(PIN_INT0_IRQn);
	//		NVIC_SetPriority(PIN_INT1_IRQn, 7);
	//		NVIC_EnableIRQ(PIN_INT1_IRQn);
	//		NVIC_SetPriority(PIN_INT2_IRQn, 7);
	//		NVIC_EnableIRQ(PIN_INT2_IRQn);
	//		NVIC_SetPriority(PIN_INT3_IRQn, 7);
	//		NVIC_EnableIRQ(PIN_INT3_IRQn);
	//		NVIC_SetPriority(PIN_INT4_IRQn, 7);
	//		NVIC_EnableIRQ(PIN_INT4_IRQn);
	//		NVIC_SetPriority(PIN_INT5_IRQn, 7);
	//		NVIC_EnableIRQ(PIN_INT5_IRQn);
	//		NVIC_SetPriority(PIN_INT6_IRQn, 7);
	//		NVIC_EnableIRQ(PIN_INT6_IRQn);
	//		NVIC_SetPriority(PIN_INT7_IRQn, 7);
	//		NVIC_EnableIRQ(PIN_INT7_IRQn);

}

void GPIO0_IRQHandler(void)
{
	if (Chip_PININT_GetRiseStates(LPC_GPIO_PIN_INT) & PININTCH0) // Verificamos que la interrupción es la esperada
	{
		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH0); // Borramos el flag de interrupción

		canalA = HIGH;
		canalB = gpioRead(GPIO1);
		if (canalB == LOW){
			count++;
		}
		else{
			count--;
		}
	}
}
