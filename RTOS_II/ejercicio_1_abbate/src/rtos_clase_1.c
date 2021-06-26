/*==================[inlcusiones]============================================*/

// Includes de FreeRTOS
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "sapi.h"

#include "fsm_debounce.h"
#include "tareas.h"
#include "qmpool.h"

/*==================[definiciones y macros]==================================*/
void initPoolArray(void);
/*==================[definiciones de datos internos]=========================*/
// Creo teclas
debouncedButton_t tecla_1;
debouncedButton_t tecla_2;
teclas_t teclas;

// Cola de mensajes entre tareas
xQueueHandle cola;
/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.

QMPool objeto;

char array[5*30]; // 5 bloques de 30 bytes

int main(void)
{
   // ---------- CONFIGURACIONES ------------------------------
   // Inicializar y configurar la plataforma
   boardConfig();

   // UART for debug messages
   uartInit( UART_USB, 115200 );
   QMPool_init(&objeto,array,5,30); 	// QM_pool_init(&pool,array,#bloques,bytes de c/bloque)

   //

   // Inicializo parámetros de las tareas
   tecla_1.name = TEC1;
   tecla_2.name = TEC2;
   teclas.tecla_1 = &tecla_1;
   teclas.tecla_2 = &tecla_2;

   // Tarea A: Enciende periódicamente un LED y envía el mensaje "LED ON"
   xTaskCreate(
      tarea_a,
      (const char *) "tarea_a",
      configMINIMAL_STACK_SIZE*2,
      NULL,
      tskIDLE_PRIORITY+1,
      0
   );

   // Tarea B: Maneja el antirrebote de las teclas, mide el tiempo de pulsación, y envía el mensaje "TECx yyyy"
   // Recibe como parámetro una estructura que tiene los punteros a las instancias de las teclas
   xTaskCreate(
      tarea_b,                     
      (const char *)"tarea_b",     
      configMINIMAL_STACK_SIZE*2, 
      &teclas,                    
      tskIDLE_PRIORITY+1,         
      0                           
   );

   // Tarea C: Recibe mensajes de las tareas A y B
   xTaskCreate(
      tarea_c,                     
      (const char *)"tarea_c",     
      configMINIMAL_STACK_SIZE*2, 
      NULL,                        
      tskIDLE_PRIORITY+1,         
      0                           
   );

   // Creación de colas de mensajes
   cola = xQueueCreate(10, sizeof(msg_t));

   // Iniciar scheduler
   vTaskStartScheduler();

   // ---------- REPETIR POR SIEMPRE --------------------------
   while( TRUE ) {
      // Si cae en este while 1 significa que no pudo iniciar el scheduler
   }
   return 0;
}

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/
