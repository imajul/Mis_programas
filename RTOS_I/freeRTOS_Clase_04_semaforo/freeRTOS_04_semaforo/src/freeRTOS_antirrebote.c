/* Copyright 2017-2018, Eric Pernia
 * All rights reserved.
 *
 * This file is part of sAPI Library.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*==================[inlcusiones]============================================*/

// Includes de FreeRTOS
#include "../../../../RTOS_I/freeRTOS_Clase_04_semaforo/freeRTOS_04_semaforo/inc/FreeRTOSConfig.h"
#include "FreeRTOS.h"   //Motor del OS
#include "task.h"		//Api de control de tareas y temporización
#include "semphr.h"		//Api de sincronización (sem y mutex)

// sAPI header
#include "sapi.h"


/*==================[definiciones y macros]==================================*/
#define UP      1
#define FALLING 2
#define DOWN	3
#define RISING  4

/*==================[definiciones de datos internos]=========================*/
SemaphoreHandle_t Evento_pulsado, Mutex_t_pulsado;
portTickType TiempoPulsado;
/*==================[definiciones de datos externos]=========================*/

DEBUG_PRINT_ENABLE;

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

// Prototipo de funcion de la tarea
void Tecla( void* taskParmPtr );

void Led_task( void* taskParmPtr );

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main(void)
{
   uint8_t Error_state = 0;
	// ---------- CONFIGURACIONES ------------------------------
   // Inicializar y configurar la plataforma
   boardConfig();

   // UART for debug messages
   debugPrintConfigUart( UART_USB, 115200 );
   debugPrintlnString( "Blinky con freeRTOS y sAPI." );

   // Led para dar se�al de vida
   gpioWrite( LED3, ON );

   /* Attempt to create a semaphore. */
   if (NULL == (Evento_pulsado = xSemaphoreCreateBinary())){
	   Error_state =1;
   }

   if (NULL == (Mutex_t_pulsado = xSemaphoreCreateMutex())){
   	   Error_state =1;
   }

   // Crear tarea Tecla en freeRTOS
   xTaskCreate(
	   Tecla,                     // Funcion de la tarea a ejecutar
      (const char *)"Tecla",     // Nombre de la tarea como String amigable para el usuario
      configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
      0,                          // Parametros de tarea
      tskIDLE_PRIORITY+1,         // Prioridad de la tarea
      0                           // Puntero a la tarea creada en el sistema
   );

   // Crear tarea LED en freeRTOS
  xTaskCreate(
   Led_task,                     // Funcion de la tarea a ejecutar
	 (const char *)"Led",     // Nombre de la tarea como String amigable para el usuario
	 configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
	 0,                          // Parametros de tarea
	 tskIDLE_PRIORITY+1,         // Prioridad de la tarea
	 0                           // Puntero a la tarea creada en el sistema
  );

   // Iniciar scheduler
   if (0 == Error_state){
  	  vTaskStartScheduler();
   } else{
	  printf("Error al iniciar el sistema !!!!!!!!!!!!!!");
   }

   // ---------- REPETIR POR SIEMPRE --------------------------
   while( TRUE ) {
      // Si cae en este while 1 significa que no pudo iniciar el scheduler
   }

   // NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
   // directamenteno sobre un microcontroladore y no es llamado por ningun
   // Sistema Operativo, como en el caso de un programa para PC.
   return 0;
}

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/

// Implementacion de funcion de la tarea Tecla
void Tecla( void* taskParmPtr )
{
   // ---------- CONFIGURACIONES ------------------------------
   printf( "Antirrebote freeRTOS y sAPI.\r\n" );

   //Variables que contiene al estado y tiempo intermedio
   uint8_t Estado = UP;
   portTickType TiempoIntermedio, TiempoProvisional;


   // Tarea periodica cada 5 ms
   portTickType xPeriodicity =  500 / portTICK_RATE_MS;
   portTickType xLastWakeTime = xTaskGetTickCount();
   
   // ---------- REPETIR POR SIEMPRE --------------------------
   while(TRUE) {
	  switch (Estado){
	  	  case UP:
	  		  if(0 == gpioRead(TEC1)){
	  			  Estado = FALLING;
	  			  TiempoIntermedio = xTaskGetTickCount();
	  		  } else {
	  			//código para eventos por nivel (UP)
	  		  }
	     break;

	  	 case FALLING:
	  		 if(0  == gpioRead(TEC1)){
	  			 if (((xTaskGetTickCount()-TiempoIntermedio)*portTICK_RATE_MS)>=20){
	  				Estado = DOWN;
	  				//Codigo para eventos por flanco de bajada

	  				TiempoProvisional = xTaskGetTickCount();
	  				xSemaphoreTake(Mutex_t_pulsado,portMAX_DELAY);
	  					TiempoPulsado = TiempoProvisional;
					xSemaphoreGive(Mutex_t_pulsado);
	  			 }
	  		 }
	  		 else {
	  			Estado = UP;
	  		 }
	  	break;

	  	 case DOWN:
	  		if(1 == gpioRead(TEC1)){
			  Estado = RISING;
			  TiempoIntermedio = xTaskGetTickCount();
	  		} else {
	  			//código para eventos por nivel (DOWN)
	  		}
	  	 break;

	  	 case RISING:
	  		if(1  == gpioRead(TEC1)){
				 if (((xTaskGetTickCount()-TiempoIntermedio)*portTICK_RATE_MS)>=20){
					Estado = UP;
					//Codigo para eventos por flanco de subida
					TiempoProvisional = xTaskGetTickCount() - TiempoProvisional;
					xSemaphoreTake(Mutex_t_pulsado,portMAX_DELAY);
						TiempoPulsado = TiempoProvisional;
					xSemaphoreGive(Mutex_t_pulsado);
					xSemaphoreGive(Evento_pulsado);
				 }
			}
			 else {
				Estado = DOWN;
			 }
		 break;

	  	 default:
	  		 Estado = UP;
	  	 break;
	  }

	}

   // Envia la tarea al estado bloqueado durante xPeriodicity (delay periodico)
   vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
}


// Implementacion de funcion de la tarea Tecla
void Led_task( void* taskParmPtr ){
	portTickType TiempoProvisional;
	while (TRUE){
		//evento de boton pulsado
		xSemaphoreTake( Evento_pulsado, portMAX_DELAY );
		xSemaphoreTake(Mutex_t_pulsado,portMAX_DELAY);
			TiempoProvisional = TiempoPulsado;
		xSemaphoreGive(Mutex_t_pulsado);
		gpioWrite(LEDB,ON);
		//tiempo de encendido
		vTaskDelay( TiempoProvisional );
		gpioWrite(LEDB,OFF);
	}
}


/*==================[fin del archivo]========================================*/
