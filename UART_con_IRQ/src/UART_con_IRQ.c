#include "sapi.h"
#define SAPI_USE_INTERRUPTS
DEBUG_PRINT_ENABLE;

void My_IRQ_Init (void);
//
//void onRx( void *noUsado )
//{
////   char c = uartRxRead( UART_USB );
////   printf( "Recibimos <<%c>> por UART\r\n", c );
//   gpioToggle(LED1);
//}

int main(void)
{
   /* Inicializar la placa */
   boardConfig();
   gpioConfig( GPIO0, GPIO_INPUT_PULLUP);
   My_IRQ_Init();

   /* Inicializar la UART_USB junto con las interrupciones de Tx y Rx */
//   uartConfig(UART_USB, 115200);
//   // Seteo un callback al evento de recepcion y habilito su interrupcion
//   uartCallbackSet(UART_USB, UART_RECEIVE, onRx, NULL);
//   // Habilito todas las interrupciones de UART_USB
//   uartInterrupt(UART_USB, true);

   //printf( "Recibimos algo por UART\r\n");

   while(TRUE) {
      // Una tarea muy bloqueante para demostrar que la interrupcion funcina
      gpioToggle(LEDB);
      delay(500);
   }
   return 0;
}

void GPIO0_IRQHandler(void)
{
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH0); // Borramos el flag de interrupción
	   gpioToggle(LED1);

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

