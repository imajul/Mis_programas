#include "fsm_debounce.h"
#include "sapi.h"

// La cola de mensajes va a contener punteros a cada mensaje alocado dinámicamente
#define msg_t void*

void tarea_a(void* taskParmPtr);

void tarea_b(void* taskParmPtr);

void tarea_c(void* taskParmPtr);

