

/*==================[inlcusiones]============================================*/

// Includes de FreeRTOS
#include "../../TP1/inc/FreeRTOSConfig.h"
#include "../../TP1/inc/tipos.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sapi.h"

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/
tLedTecla tecla_led_config[CANT];


/*==================[definiciones de datos externos]=========================*/
DEBUG_PRINT_ENABLE;

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/
TickType_t get_diff();
void clear_diff();

// Prototipo de funcion de la tarea
void tarea_led( void* taskParmPtr );
void tarea_tecla( void* taskParmPtr );

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main( void )
{
    // ---------- CONFIGURACIONES ------------------------------
    // Inicializar y configurar la plataforma
    boardConfig();

    // UART for debug messages
    debugPrintConfigUart( UART_USB, 115200 );
    debugPrintlnString( "Blinky con freeRTOS y sAPI." );

    // Led para dar se�al de vida
    gpioWrite( LED3 , ON );

    tecla_led_config[0].led 	= LED1;
    tecla_led_config[0].tecla 	= TEC1;

    tecla_led_config[1].led 	= LED2;
    tecla_led_config[1].tecla 	= TEC2;

    // Crear tarea en freeRTOS

    xTaskCreate(
        tarea_led,                     // Funcion de la tarea a ejecutar
        ( const char * )"tarea_led1",   // Nombre de la tarea como String amigable para el usuario
        configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
        &tecla_led_config[0],                          // Parametros de tarea
        tskIDLE_PRIORITY+1,         // Prioridad de la tarea
        0                           // Puntero a la tarea creada en el sistema
    );

    xTaskCreate(
		tarea_led,                     	// Funcion de la tarea a ejecutar
		( const char * )"tarea_led2",   // Nombre de la tarea como String amigable para el usuario
		configMINIMAL_STACK_SIZE*2, 	// Cantidad de stack de la tarea
		&tecla_led_config[1],        	// Parametros de tarea
		tskIDLE_PRIORITY+1,         	// Prioridad de la tarea
		0                           	// Puntero a la tarea creada en el sistema
	);


    xTaskCreate(
    	tarea_tecla,                     // Funcion de la tarea a ejecutar
        ( const char * )"tarea_tecla1",  // Nombre de la tarea como String amigable para el usuario
        configMINIMAL_STACK_SIZE*2, 	// Cantidad de stack de la tarea
		&tecla_led_config[0],           // Parametros de tarea
        tskIDLE_PRIORITY+1,         	// Prioridad de la tarea
        0                           	// Puntero a la tarea creada en el sistema
    );

    xTaskCreate(
    	tarea_tecla,                     // Funcion de la tarea a ejecutar
        ( const char * )"tarea_tecla2",  // Nombre de la tarea como String amigable para el usuario
        configMINIMAL_STACK_SIZE*2, 	 // Cantidad de stack de la tarea
		&tecla_led_config[1],            // Parametros de tarea
        tskIDLE_PRIORITY+1,         	// Prioridad de la tarea
        0                           	// Puntero a la tarea creada en el sistema
    );

    	// Iniciar scheduler
    vTaskStartScheduler();

    // ---------- REPETIR POR SIEMPRE --------------------------
    while( TRUE )
    {
        // Si cae en este while 1 significa que no pudo iniciar el scheduler
    }

    // NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
    // directamenteno sobre un microcontroladore y no es llamado por ningun
    // Sistema Operativo, como en el caso de un programa para PC.
    return 0;
}

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/

void tarea_tecla( void* taskParmPtr )
{
	tLedTecla* config = (tLedTecla*) taskParmPtr;

	fsmButtonInit( config );

	while( 1 )
	{
		fsmButtonUpdate( config );
	 	vTaskDelay( 1 / portTICK_RATE_MS );
	}
}

// Implementacion de funcion de la tarea
void tarea_led( void* taskParmPtr )
{
    // ---------- CONFIGURACIONES ------------------------------
	tLedTecla* config = (tLedTecla*) taskParmPtr;

    // ---------- REPETIR POR SIEMPRE --------------------------
    while( TRUE )
    {
    	TickType_t dif = config->tiempo_medido;

    	if( dif == 0  )
		{
    		vTaskDelay( 40 / portTICK_RATE_MS);	//40 ms es el debounce type.
		}
		else
		{
			gpioWrite( config->led , 1 );
			vTaskDelay( dif );
			gpioWrite( config->led , 0 );

			config->tiempo_medido = 0;
		}
    }
}

/*==================[fin del archivo]========================================*/
