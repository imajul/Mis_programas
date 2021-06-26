/*=============================================================================
 * Author: Ignacio Majul <imajul89@gmail.com>
 * Date: 2019/12/10
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/


#include "eeprom_24C32.h"
#include "sapi.h"

/*=====[Definition macros of private constants]==============================*/

DEBUG_PRINT_ENABLE

/*=====[Definitions of extern global variables]==============================*/

/*=====[Definitions of public global variables]==============================*/


Eeprom24C32_t eeprom24C32;


/*=====[Definitions of private global variables]=============================*/

/*=====[Main function, program entry point after power on or reset]==========*/

int main( void )
{
	// ----- Setup -----------------------------------
	boardInit();
	debugPrintConfigUart( UART_USB, 115200 );

	uint32_t i=0;
	uint16_t eeprom_address = EEPROM24C32_FIRST_MEMORY_ADDRESS;
	uint8_t readedByte[10], buffer[10] = {'a','b','c','d','e','f','g','h','i','j'};

	i2cInit( I2C0, 100000 );
	debugPrintlnString( "I2C initialization complete." );

	eeprom24C32Init( &eeprom24C32, I2C0, 1, 1, 1, EEPROM24C32_PAGE_SIZE, EEPROM24C32_MEMORY_SIZE );  // inicializo la EEPROM
	debugPrintlnString( "EEPROM initialization complete." );

//	eeprom24C32WriteDate(&eeprom24C32, &eeprom_address, time); // escribo la fecha en la EEPROM

	for( i=0 ; i<10 ; i++ )
	{
		eeprom24C32WriteByte( &eeprom24C32, eeprom_address, buffer[i]);
		if(eeprom_address == EEPROM24C32_LAST_MEMORY_ADDRESS)
		{
			eeprom_address = EEPROM24C32_FIRST_MEMORY_ADDRESS;
		}
		else
		{
			eeprom_address++;
		}
	}

	eeprom_address = EEPROM24C32_FIRST_MEMORY_ADDRESS;
	eeprom24C32ReadSequential( &eeprom24C32, eeprom_address, readedByte, 10);  // leo la hora escrita e imprimo
	i=0;

	while( true )
	{
		if(i<10)
		{
			debugPrintString( "lectura: " );
			debugPrintChar( *(readedByte+i) );
			debugPrintString("\r\n ");
			delay(1000);
			i++;
		}

//		if(eeprom_address == EEPROM24C32_LAST_MEMORY_ADDRESS)
//		{
//			eeprom_address = EEPROM24C32_FIRST_MEMORY_ADDRESS;
//		}
//		else
//		{
//			eeprom_address++;
//		}

	}

	// YOU NEVER REACH HERE, because this program runs directly or on a
	// microcontroller and is not called by any Operating System, as in the
	// case of a PC program.
	return 0;
}
