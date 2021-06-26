/*=============================================================================
 * Copyright (c) 2019, Gonzalo Gontad <gonzalogontad@gmail.com>
 * All rights reserved.
 * License: MIT (see LICENSE.txt)
 * Date: 2019/09/29
 * Version: v1.0
 *===========================================================================*/

/*=====[Inclusion of own header]=============================================*/

#include "driver.h"
#include "uartIRQ.h"

/*=====[Inclusions of private function dependencies]=========================*/

/*=====[Definition macros of private constants]==============================*/

/*=====[Private function-like macros]========================================*/

/*=====[Definitions of private data types]===================================*/

/*=====[Definitions of external public global variables]=====================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Prototypes (declarations) of private functions]======================*/

/*=====[Implementations of public functions]=================================*/

// Inicializacion de parte de transmision del driver
// Requiere que se le pase una variable del tipo driverConfig_t
// con los siguientes datos:
// 	selectedUart.uartValue = (por ej:UART_USB);
//	selectedUart.baudRate = (por ej: 115200);
bool_t driverInit( driver_t* selectedUart )
{
    // Inicializamos la UART que se ha seleccionado y con el baud rate especificado.
    uartConfig( selectedUart->uartValue, selectedUart->baudRate );

    // Creamos el timer que cumplirá la función de time out y poder separar los paquetes.

    selectedUart->onTxTimeOut = xTimerCreate( "TX Time Out", PROTOCOL_TIMEOUT,pdFALSE, ( void* ) selectedUart, onTxTimeOutCallback );
    selectedUart->onRxTimeOut = xTimerCreate( "RX Time Out", PROTOCOL_TIMEOUT,pdFALSE, ( void* ) selectedUart, onRxTimeOutCallback );

    // Creamos la cola para señalizar la recepción de un dato válido hacia la aplicación.

    selectedUart->onRxQueue = xQueueCreate( POOL_TOTAL_BLOCKS, sizeof( char* ) );

    //	Creo una cola donde van a ir los bloque que tengo que mandar por UART
    selectedUart->onTxQueue = xQueueCreate( POOL_TOTAL_BLOCKS, sizeof( char* ) ); //La cola va a tener tantos elementos como bloques de memoria pueda tener el pool

    //	Reservo memoria para el memory pool
    selectedUart->txPoolSto = ( char* ) pvPortMalloc( POOL_SIZE * sizeof( char ) );

    //	Creo el pool de memoria que va a usarse para la transmision
    QMPool_init( &selectedUart->txPool, ( void* ) selectedUart->txPoolSto,POOL_SIZE * sizeof( char ),BLOCK_SIZE ); //Tamaño del segmento de memoria reservado
    // Tamaño de cada Bloque
    //	Validamos si todos los recursos que requerían de memoria fueron creados exitosamente.
    if ( ( selectedUart->onRxTimeOut == NULL ) ||
            ( selectedUart->onTxTimeOut == NULL ) ||
            ( selectedUart->onRxQueue == NULL ) ||
            ( selectedUart->onTxQueue == NULL ) ||
            ( selectedUart->txPoolSto == NULL ) )
    {
        // No hay memoria suficiente. Por este motivo, retornamos false.
        return( false );
    }


    selectedUart->txCounter = 0; //inicializo el contador de bytes enviados
    selectedUart->rxLen = 0;
    selectedUart->txLen = 0;
    selectedUart->rxChecksum =0; //Inicializo el acumulador del crc

    // Asigno un bloque de memoria para la primera recepcion de la UART
    selectedUart->rxBlock = ( char* ) QMPool_get( &selectedUart->txPool, 0 ); //pido un bloque del pool

    // Configuramos el callback del evento de recepción y habilitamos su interrupción.
    if( ( rxInterruptEnable( selectedUart ) ) )
    {}
    else
        //se eligio una UART no valida
    {
        return ( false );
    }

    // Habilitamos todas las interrupciones de la UART seleccionada.
    uartInterrupt( selectedUart->uartValue, true );

    // Si todos los elementos fueron creados e inicializados correctamente, indicamos que el driver está listo para ser utilizado.
    return( true );
}

//	La funcion getBuffer recibe como parametro:
// 		driverConfig_t* selectedUart : Puntero a la estructura de la UART donde
//		estan todas las variables y puntros asociado a la UART.
//	Devuelve un *char que apunta al bloque de memoria del pool donde se puede escribir
//	(si es NULL significa que no hay memoria suficiente en el pool)
//	La longitud del bloque esta dada por la constante TX_BUFFER_SIZE

char* getBuffer( driver_t* selectedUart )
{
    char *newBlock;
    newBlock = ( char* ) QMPool_get( &selectedUart->txPool, 0 ); //pido un bloque del pool
    return newBlock;
}

//	La funcion putBuffer recibe como parametro:
// 		driverConfig_t* selectedUart : Puntero a la estructura de la UART donde
//		estan todas las variables y puntros asociado a la UART.
//		char *blockToFree : Puntero al bloque que se quiere liberar
//
void putBuffer( driver_t* selectedUart, char *blockToFree )
{
    QMPool_put( &selectedUart->txPool, blockToFree ); //Libero un bloque del pool

}


//	packetTX(driverConfig_t* selectedUart, char *txBlock) transmite el bloque
//	txBlock por la UART indicada por selectedUart
int packetTX( driver_t* selectedUart, char *txBlock )
{

    xQueueSend( selectedUart->onTxQueue, &txBlock, portMAX_DELAY ); //Envio a la cola de transmision el blocke a transmitir
    taskENTER_CRITICAL();  //no permito que se modifique txcounter
    if ( selectedUart->txCounter == 0 ) //si se esta enviando algo no llamo a la interrupcion para no interrumpir el delay
    {
        txInterruptEnable( selectedUart );
    }
    taskEXIT_CRITICAL();
    uartSetPendingInterrupt( selectedUart->uartValue );
}


// waitPacket() se queda esperando un paquete de la UART seleccionada y devuelve un puntero al bloque de memoria
char* waitPacket( driver_t* selectedUart )
{
    char*rxBlock;
    xQueueReceive( selectedUart->onRxQueue,( void * )&rxBlock,portMAX_DELAY ); //espero a que venga un bloque por la cola
    return rxBlock;
}

/*=====[Implementations of private functions]================================*/


