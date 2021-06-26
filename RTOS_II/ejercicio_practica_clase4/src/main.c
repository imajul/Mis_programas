#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "sapi.h"
#include "userTasks.h"
#include "driver.h"

int main( void )
{
    static driver_t selectedUart; //Variable de la instanciacion del driver que va a usar la tarea principal

    /* Inicializar la placa */

    boardConfig();

    //Inicializacion del driver de UART

    selectedUart.uartValue = UART_USB;
    selectedUart.baudRate = 115200;
    if ( driverInit( &selectedUart )==false )
    {
        gpioWrite( LED3,true ); //Error al inicializar el driver
        printf( "Error al inicializar el driver" );
        while( 1 );
    }

    xTaskCreate(
        tickTask,                     // Funcion de la tarea a ejecutar
        ( const char * )"tickTask",   // Nombre de la tarea como String amigable para el usuario
        configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
        0,                          // Parametros de tarea
        tskIDLE_PRIORITY+1,         // Prioridad de la tarea
        0                           // Puntero a la tarea creada en el sistema
    );

    xTaskCreate(
        checkTask,                     // Funcion de la tarea a ejecutar
        ( const char * )"tarea de chequeo de paquetes",   // Nombre de la tarea como String amigable para el usuario
        configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
        ( void* )&selectedUart,                        // Parametros de tarea
        tskIDLE_PRIORITY+2,         // Prioridad de la tarea
        0                           // Puntero a la tarea creada en el sistema
    );

    vTaskStartScheduler();

    return 0;
}
