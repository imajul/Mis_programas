/*=============================================================================
 * Copyright (c) 2019, Gonzalo Gontad <gonzalogontad@gmail.com>
 * All rights reserved.
 * License: mit (see LICENSE.txt)
 * Date: 2019/09/29
 * Version: v1.0
 *===========================================================================*/

/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef RTOS2_TP1_INC_TXDRIVER_H_
#define RTOS2_TP1_INC_TXDRIVER_H_

/*=====[Inclusions of public function dependencies]==========================*/

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "string.h"
#include "sapi.h"
#include "timers.h"
#include "qmpool.h"
#include "crc8.h"

/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Definition macros of public constants]===============================*/

#define PROTOCOL_TIMEOUT pdMS_TO_TICKS(3)
#define PACKET_SIZE 127 //dejo un byte para tamaño del paquete y que quede redondo en 128. Maximo 254
#define BLOCK_SIZE (PACKET_SIZE+1)
#define POOL_TOTAL_BLOCKS 4//catidad de bloques en el pool de memoria
#define POOL_SIZE POOL_TOTAL_BLOCKS*BLOCK_SIZE //Tamaño total del pool


/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/

typedef struct
{
    uartMap_t uartValue; //Numero de UART a utilizar
    uint32_t baudRate; //velocidad de transmision
    char *txPoolSto; //puntero al segmento de memoria que albergara el pool para transmision
    QMPool txPool; //memory pool (contienen la informacion que necesita la biblioteca qmpool.h
    TimerHandle_t onTxTimeOut; //Timer para el timeout de la transmision
    TimerHandle_t onRxTimeOut; //Timer para el timeout de la recepcion
    char *txBlock; //Bloque de memoria de la transmision en curso
    char *rxBlock; //Bloque de memoria de la recepcion
    uint8_t txCounter; //contador d bytes transmitidos
    QueueHandle_t onTxQueue; //cola de transmision (por esta cola llegan los bloques de memoria a transmitir)
    QueueHandle_t onRxQueue; //cola de recepcion (por esta cola llegan los bloques de memoria recibidos)
    uint8_t txLen; //longitud del paquete en transmision
    uint8_t rxLen; //longitud del paquete recibido
    int rxChecksum; //acumulo el checksum

} driver_t;

/*=====[Prototypes (declarations) of public functions]=======================*/

bool_t	driverInit	( driver_t* );
char*	getBuffer 	( driver_t* selectedUart );
void 	putBuffer	( driver_t* selectedUart, char *blockToFree );
int 	packetTX 	( driver_t* selectedUart,char *txBlock );
char* 	waitPacket	( driver_t* );

/*=====[Prototypes (declarations) of public interrupt functions]=============*/

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* MYPROJECTS_RTOS2_TP1_INC_DRIVER_H_ */
