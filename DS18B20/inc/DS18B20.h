/*=============================================================================
 * Program: DS18B20
 * Date: 2021/04/18
 *===========================================================================*/

/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef APPLICATION_INC_ONEWIRE_H_
#define APPLICATION_INC_ONEWIRE_H_
/** @brief Brief for this header file.
 **
 ** Full description for this header file.
 **
 **/

/** \addtogroup groupName Group Name
 ** @{ */


/*==================[inclusions]=============================================*/

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

void owInit(gpioMap_t gpioPin);
int owReadTemperature(gpioMap_t gpioPin);
int owSetBitResolution(gpioMap_t gpioPin, uint8_t res);
int owSetHighTempAlarm(gpioMap_t gpioPin, int8_t alarm_high);
int owSetLowTempAlarm(gpioMap_t gpioPin, int8_t alarm_high);

int owReadScratch(void * buffer9, int port, int pin);

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/** @} doxygen end group definition */
/*==================[end of file]============================================*/


#endif /* APPLICATION_INC_ONEWIRE_H_ */
