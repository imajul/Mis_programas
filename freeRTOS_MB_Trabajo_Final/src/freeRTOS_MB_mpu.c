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
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"



// sAPI header
#include "sapi.h"
#include "stdint.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "user_mb_app.h"

/*==================[definiciones y macros]==================================*/
/* ----------------------- Defines ------------------------------------------*/

#define PERIODO_MUESTREO_MS	100

// MPU9250 Address
MPU9250_address_t addr = MPU9250_ADDRESS_0; // If MPU9250 AD0 pin is connected to GND



#define TASK_MODBUS_STACK_SIZE          ( 256 )
#define TASK_MODBUS_PRIORITY            ( tskIDLE_PRIORITY + 1 )

#define TASK_APPL_STACK_SIZE            ( 256 )
#define TASK_APPL_PRIORITY              ( tskIDLE_PRIORITY + 1 )


/* ----------------------- Static functions ---------------------------------*/
//static void     vTaskApplication( void *pvArg );
static void     vTaskMODBUS( void *pvArg );

/* ----------------------- Static variables ---------------------------------*/

/*==================[definiciones de datos internos]=========================*/

/*==================[definiciones de datos externos]=========================*/

extern USHORT   usSRegHoldStart;
extern USHORT   usSRegHoldBuf[];

DEBUG_PRINT_ENABLE;

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

// Prototipo de funcion de la tarea
void myTask( void* taskParmPtr );
void TaskReadMPU( void* taskParmPtr );

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main(void)
{
   // ---------- CONFIGURACIONES ------------------------------
   // Inicializar y configurar la plataforma
   boardConfig();

   mpu9250Init( addr );
	uint32_t i;
   // UART for debug messages
   debugPrintConfigUart( UART_USB, 115200 );
   debugPrintlnString( "Modbus con FreeRTOS." );

   // Led para dar se�al de vida
   gpioWrite( LED3, ON );

   xTaskCreate(
   	  TaskReadMPU,                     // Funcion de la tarea a ejecutar
      (const char *)"TaskReadMPU",     // Nombre de la tarea como String amigable para el usuario
      configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
      0,                          // Parametros de tarea
      tskIDLE_PRIORITY+3,         // Prioridad de la tarea
      0                           // Puntero a la tarea creada en el sistema
   );

   // Crear tarea en freeRTOS
   xTaskCreate(
      myTask,                     // Funcion de la tarea a ejecutar
      (const char *)"myTask",     // Nombre de la tarea como String amigable para el usuario
	  TASK_MODBUS_STACK_SIZE, // Cantidad de stack de la tarea
      0,                          // Parametros de tarea
      tskIDLE_PRIORITY+1,         // Prioridad de la tarea
      0                           // Puntero a la tarea creada en el sistema
   );
   xTaskCreate( vTaskMODBUS, "MODBUS", TASK_MODBUS_STACK_SIZE, 0, tskIDLE_PRIORITY+2, 0 );
   // Iniciar scheduler
   vTaskStartScheduler();

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

// Implementacion de funcion de la tarea
void myTask( void* taskParmPtr )
{
   // ---------- CONFIGURACIONES ------------------------------
   // ---------- REPETIR POR SIEMPRE --------------------------
   while(TRUE) {
      // Intercambia el estado del LEDB
      gpioToggle( LEDB );
      debugPrintlnString( "Blink!" );
      // Envia la tarea al estado bloqueado durante 500ms
      vTaskDelay( 500 / portTICK_RATE_MS );
   }
}

static void
vTaskMODBUS( void *pvArg )
{
    eMBErrorCode    eStatus;
	uint32_t indice = 0;
    eStatus = eMBInit( MB_RTU, 0x0A, 7, 115200, MB_PAR_NONE );
    eStatus = eMBEnable(  );
    if (eStatus == MB_ENOERR){
        debugPrintlnString( "Modbus inicializado correctamente" );
    }
    else{
        debugPrintlnString( " Error inicalizando Modbus" );
    }


    for( ;; )
    {
    	( void )eMBPoll(  );
        vTaskDelay( 5 / portTICK_RATE_MS );
    }
}

void TaskReadMPU( void* taskParmPtr ){
	TickType_t tiempo_inicial;
	uint32_t i;
	uint16_t *pC;
	float temporal;
	tiempo_inicial = xTaskGetTickCount();
	while(1){
	      mpu9250Read();


	      i = 0;
	      pC = &temporal;

	      temporal = mpu9250GetTemperature_C();

	      usSRegHoldBuf[i] =  *pC;i++;
	      usSRegHoldBuf[i] =  *(pC+1);i++;

	      temporal = mpu9250GetGyroX_rads();

	      usSRegHoldBuf[i] =  *pC;i++;
	      usSRegHoldBuf[i] =  *(pC+1);i++;

	      temporal = mpu9250GetGyroY_rads();

	      usSRegHoldBuf[i] =  *pC;i++;
	      usSRegHoldBuf[i] =  *(pC+1);i++;

	      temporal = mpu9250GetGyroZ_rads();

	      usSRegHoldBuf[i] =  *pC;i++;
	      usSRegHoldBuf[i] =  *(pC+1);i++;

	      temporal = mpu9250GetAccelX_mss();

	      usSRegHoldBuf[i] =  *pC;i++;
	      usSRegHoldBuf[i] =  *(pC+1);i++;

	      temporal = mpu9250GetAccelY_mss();

	      usSRegHoldBuf[i] =  *pC;i++;
	      usSRegHoldBuf[i] =  *(pC+1);i++;

	      temporal = mpu9250GetAccelZ_mss();

	      usSRegHoldBuf[i] =  *pC;i++;
	      usSRegHoldBuf[i] =  *(pC+1);i++;

	      temporal = mpu9250GetMagX_uT();

	      usSRegHoldBuf[i] =  *pC;i++;
	      usSRegHoldBuf[i] =  *(pC+1);i++;

	      temporal = mpu9250GetMagY_uT();

	      usSRegHoldBuf[i] =  *pC;i++;
	      usSRegHoldBuf[i] =  *(pC+1);i++;

	      temporal = mpu9250GetMagZ_uT();

	      usSRegHoldBuf[i] =  *pC;i++;
	      usSRegHoldBuf[i] =  *(pC+1);i++;


	      vTaskDelayUntil(&tiempo_inicial, PERIODO_MUESTREO_MS / portTICK_RATE_MS );
	}
}

void
vApplicationIdleHook( void )
{
}

void
vApplicationTickHook( void )
{
}

/*==================[fin del archivo]========================================*/
