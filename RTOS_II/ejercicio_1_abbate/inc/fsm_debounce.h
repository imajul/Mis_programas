#ifndef FSM_DEBOUNCE_H
#define FSM_DEBOUNCE_H

#include "FreeRTOS.h"
#include "task.h"

typedef enum{
   STATE_BUTTON_UP,
   STATE_BUTTON_DOWN,
   STATE_BUTTON_FALLING,
   STATE_BUTTON_RISING
} fsmButtonState_t;

typedef struct
{
    fsmButtonState_t state;
    gpioMap_t   name;
    portTickType pressedTime;
    portTickType pressedElapsedTime;
} debouncedButton_t;

typedef struct 
{
    debouncedButton_t * tecla_1;
    debouncedButton_t * tecla_2;
} teclas_t;


// void fsmButtonError( void );
void fsmButtonInit( debouncedButton_t *button );
void fsmButtonUpdate( debouncedButton_t *button );
void buttonPressed(  debouncedButton_t *button  );
void buttonReleased( debouncedButton_t *button  );

#endif  //FSM_DEBOUNCE_H