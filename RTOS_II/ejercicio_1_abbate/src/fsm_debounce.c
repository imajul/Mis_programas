#include "FreeRTOS.h"
#include "queue.h"
#include "sapi.h"
#include "fsm_debounce.h"
#include <stdint.h>
#include <string.h>
#include "qmpool.h"

extern xQueueHandle cola;
extern QMPool objeto;

void mensaje_tecla(gpioMap_t tecla, portTickType pressedElapsedTime)
{
   // Armo el mensaje a mandar
   char msg[] = "TECx Tyyyy\r\n";
   sprintf(msg,"TECx T%04d\r\n",pressedElapsedTime);
   msg[3] = tecla == TEC1 ? (const char)'1' : (const char)'2';

   // Aloco memoria para el mensaje
   //char *p = pvPortMalloc(10);
   
   // --
   portENTER_CRITICAL ( ); 			// Entrada a seccion critica

   void* p;

   p = QMPool_get(&objeto,0,0);		// "malloc"

   portEXIT_CRITICAL ( );				// Salida de seccion critica
   // --

   if (p != NULL){
	   	   // Inicializo la memoria recién alocada, con el mesaje
	   memcpy(p,msg, 10);
	   // Envío a la cola el puntero al mensaje
	   xQueueSend(cola, (void *) &p, portMAX_DELAY);
   }


}


void buttonPressed(  debouncedButton_t *button  )
{
   //Acá cuento inicio tiempo
   button->pressedTime = xTaskGetTickCount();
   // printf("PRESSED %d\r\n",button->name);
}

void buttonReleased( debouncedButton_t *button   )
{
   //Aca cuento fin tiempo
   button->pressedElapsedTime = xTaskGetTickCount() - button->pressedTime;
   mensaje_tecla(button->name,button->pressedElapsedTime);
}

// void fsmButtonError( void )
// {
//    fsmButtonState = BUTTON_UP;
// }

void fsmButtonInit( debouncedButton_t *button  )
{
   button->state = BUTTON_UP;  // Set initial state
   button->pressedTime = 0;
   button->pressedElapsedTime = 500;
   //button->name = TEC1;
   // printf("INIT\r\n");
}

// FSM Update Sate Function
void fsmButtonUpdate( debouncedButton_t *button )
{
   static uint8_t contFalling = 0;
   static uint8_t contRising = 0;
   
   switch( button->state ){

      case STATE_BUTTON_UP: 
         /* CHECK TRANSITION CONDITIONS */
         if( !gpioRead(button->name) ){
            button->state = STATE_BUTTON_FALLING;
         }
      break;

      case STATE_BUTTON_DOWN:
         /* CHECK TRANSITION CONDITIONS */
         if( gpioRead(button->name) ){
            button->state = STATE_BUTTON_RISING;
         }
      break;

      case STATE_BUTTON_FALLING:          
         /* CHECK TRANSITION CONDITIONS */
         if( contFalling >= 40 ){
            if( !gpioRead(button->name) ){
               button->state = STATE_BUTTON_DOWN;
               buttonPressed(button);
            } else{
               button->state = STATE_BUTTON_UP;
            }
            contFalling = 0;
         }
         contFalling++;
      break;

      case STATE_BUTTON_RISING:        
         /* CHECK TRANSITION CONDITIONS */
         
         if( contRising >= 40 ){
            if( gpioRead(button->name) ){
               button->state = STATE_BUTTON_UP;
               buttonReleased(button);
            } else{
               button->state = STATE_BUTTON_DOWN;
            }
            contRising = 0;
         }
         contRising++;
      break;

      default:
         //fsmButtonError();
      break;
   }
}
