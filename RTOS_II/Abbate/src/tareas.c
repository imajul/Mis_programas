#include "FreeRTOS.h"
#include "queue.h"
#include "sapi.h"   
#include "fsm_debounce.h"
#include "tareas.h"
#include "string.h"
#include "qmpool.h"

extern xQueueHandle cola;

extern QMPool objeto;
extern char array[];

// Parpadea un led cada 1 segundo y envía "LED ON"
void tarea_a( void* taskParmPtr )
{
    // Tarea periodica cada 1000 ms
    portTickType xPeriodicity =  1000 / portTICK_RATE_MS;
    portTickType xLastWakeTime = xTaskGetTickCount();
    const char mensaje[] = "LED ON"; // Mensaje a enviar, de 7 caracteres

    // ---------- REPETIR POR SIEMPRE --------------------------
    while(TRUE) {
        gpioToggle( LED1 );

        // --
        portENTER_CRITICAL ( ); 			// Entrada a seccion critica

        void* p;

        p = QMPool_get(&objeto,0,0);		// "malloc"

        portEXIT_CRITICAL ( );				// Salida de seccion critica
        // --

        if (p != NULL){
			// Reemplazar aca
			//char * p = pvPortMalloc(7); // Aloco memoria para el mensaje a enviar

			memcpy(p,mensaje, 7);       // Inicializo el mensaje nuevo

			xQueueSend(cola, (void *) &p, portMAX_DELAY); // Envío en la cola el puntero al nuevo mensaje
        }

        vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
    }
}


// Debounce de cada tecla instanciada (en este caso 2) y envío de mensaje (lo hace dentro de máquina de estados)
void tarea_b(void* taskParmPtr )
{
    teclas_t *teclas = (teclas_t *)taskParmPtr;
    fsmButtonInit(teclas->tecla_1);
    fsmButtonInit(teclas->tecla_2);

    while (TRUE)
    {
        fsmButtonUpdate(teclas->tecla_1);
        fsmButtonUpdate(teclas->tecla_2);
        vTaskDelay( 1 / portTICK_RATE_MS);
    }
}

// Recibe los mensajes, los imprime, y libera la memoria alocada
void tarea_c( void* taskParmPtr )
{
    char * mensaje;

    while(TRUE) {
        xQueueReceive(cola, &mensaje, portMAX_DELAY);
        
        printf("Mensaje: %s\r\n", mensaje);

        // Reemplazar aca
        //vPortFree(mensaje);

        //void QMPool_put(QMPool * const me, void *b,uint8_t enISR )

        portENTER_CRITICAL ( ); 			// Entrada a seccion critica
        QMPool_put(&objeto , (void*)mensaje , 0);
        portEXIT_CRITICAL ( );				// Salida de seccion critica

        //mensaje = NULL; 		// Verificar si hacia falta o no esto.
   }
}

