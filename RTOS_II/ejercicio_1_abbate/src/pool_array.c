#include <string.h>
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "sapi.h"

#include "qmpool.h"
#include "pool_array.h"

#define MAX_POOL_SIZE    512     //el tamanio de cada pool (todos iguales)
#define MAX_REQUEST_SIZE 256     //lo maximo que se puede pedir
#define MIN_BLOCK_SIZE   16      //el pool mas chico de 16, el resto incrementa linealmente

char memPoolSto[ MAX_REQUEST_SIZE/MIN_BLOCK_SIZE][MAX_POOL_SIZE];
QMPool memPool [ MAX_REQUEST_SIZE/MIN_BLOCK_SIZE];

void initPoolArray(void)
{
   uint8_t i;
   for(i=0;i<sizeof(memPool)/sizeof(memPool[0]);i++)
      QMPool_init(&memPool          [ i ],
                   memPoolSto       [ i ],
                   sizeof(memPoolSto[ 0 ]),
                   MIN_BLOCK_SIZE*( i+1 ));
}
//para no hacer la asignacion usando if/else, defino una asignacino lineal
//dividiendo e indexando un vector de pools de diferentes tamanios, pero se
//puede cambiar a otro modelo
QMPool* poolSelect(uint8_t size)
{
   return &memPool[size/MIN_BLOCK_SIZE];
}
void* poolGet(uint8_t size, uint8_t inIsr)
{
   return QMPool_get ( poolSelect ( size ),0,inIsr );
}
void poolPut(uint8_t size,uint8_t* data, uint8_t inIsr)
{
   QMPool_put ( poolSelect(size ),data ,inIsr);
}
