/* Copyright 2018, Eric Pernia.
 * All rights reserved.
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

/*==================[inclusions]=============================================*/

#include "modbus_rtu.h"

#include <stdio.h>
#include "sapi.h"
//#include "serial_port.h"


/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

// Claculate the MODBUS RTU CRC
static uint16_t modbusRTU_CRC( uint8_t *buf, uint32_t size );

// Check CRC from an incomming MODBUS RTU package
static bool_t modbusRTU_CheckCRC( uint8_t *buf, uint32_t size );

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

// Claculate the MODBUS RTU CRC
// https://ctlsys.com/support/how_to_compute_the_modbus_rtu_message_crc/
static uint16_t modbusRTU_CRC( uint8_t *buf, uint32_t size )
{
   uint16_t crc = 0xFFFF;
   uint32_t pos = 0;
   uint8_t i = 0;

   for( pos=0; pos<size; pos++ ) {
      crc ^= (uint16_t)buf[pos];    // XOR byte into least sig. byte of crc
      for( i=8; i!=0; i-- ) {       // Loop over each bit
         if( (crc & 0x0001) != 0 ) { // If the LSB is set
            crc >>= 1;              // Shift right and XOR 0xA001
            crc ^= 0xA001;
         } else                     // Else LSB is not set
            crc >>= 1;              // Just shift right
      }
   }
   // Note, this number has low and high bytes swapped, so use it accordingly
   // (or swap bytes)
   return crc;
}

// Check CRC from an incomming MODBUS RTU package
static bool_t modbusRTU_CheckCRC( uint8_t *buf, uint32_t size )
{
   uint16_t crcTxMsg = modbusRTU_CRC( buf, size-2 );

   //printf( "CRC (low):  %d\n", LOW16(crcTxMsg) );
   //printf( "CRC (high): %d\n", HIGH16(crcTxMsg) );

   // MODBUS RTU package are bytes:
   // [ ADDR, FUNC, DATA1_H, DATA1_L, ..., CRC_L, CRC_H ]
   if( (HIGH16(crcTxMsg)==buf[size-1]) && (LOW16(crcTxMsg)==buf[size-2]) ) {
      //printf( "CRC OK\n\n" );
      return 1;
   }
   //printf( "CRC FAIL!!\n\n" );
   return 0;
}

/*==================[external functions definition]==========================*/

void modbusMasterRtuInit( int32_t uart, uint32_t baudRate,
                          uint8_t dataBits, uint8_t parity, uint8_t stopBits )
{
   uartInit2( uart, baudRate, dataBits, parity, stopBits );
}

void modbusMasterRtuWriteRegister( int32_t uart, uint8_t slaveAddr,
                                   uint16_t regAddr, uint16_t regValue )
{
   // MODBUS RTU FRAME (bytes):

   // Address [dec]: Slave address
   // Function [dec]: 0x06 (Write Single Holding Register)
   // Write register addres (high) [dec].
   // Write register addres (low) [dec].
   // Write register value (high) [dec].
   // Write register value (low) [dec].
   // CRC (low) [dec].
   // CRC (high) [dec].

   uint8_t txBuffer[] = { slaveAddr,
                          0x06,
                          HIGH16(regAddr),
                          LOW16(regAddr),
                          HIGH16(regValue),
                          LOW16(regValue),
                          0,
                          0
                        };

   uint16_t crc = modbusRTU_CRC( txBuffer, sizeof(txBuffer) - 2 );

   txBuffer[6] = LOW16(crc);
   txBuffer[7] = HIGH16(crc);

   uartWriteByteArray( uart, txBuffer, sizeof(txBuffer) );

   delay(5); // > 3.5 char

   // Modbus states that a baud rate higher than 19200 must use a fixed 750 us
   // for inter character time out and 1.75 ms for a frame delay.
   // For baud rates below 19200 the timeing is more critical and has to be calculated.
   // E.g. 9600 baud in a 10 bit packet is 960 characters per second
   // In milliseconds this will be 960characters per 1000ms. So for 1 character
   // 1000ms/960characters is 1.04167ms per character and finaly modbus states an
   // intercharacter must be 1.5T or 1.5 times longer than a normal character and thus
   // 1.5T = 1.04167ms * 1.5 = 1.5625ms. A frame delay is 3.5T.

 /*
   if (baud > 19200)
   {
       T1_5 = 750;
       T3_5 = 1750;
   }
   else
   {
       T1_5 = 15000000/baud;
       T3_5 = 35000000/baud;
   }
}
*/


bool_t modbusMasterRtuReadRegisters( int32_t uart, uint8_t slaveAddr,
                                     uint16_t startRegAddr, uint16_t amountOfRegs,
                                     uint8_t* rxBuffer )
{
   // MODBUS RTU FRAME (bytes):

   // Address [dec]: Slave address
   // Function [dec]: 3 (Read HoldingRegisters)
   // Start read from Register (high) [dec].
   // Start read from Register (low) [dec].
   // Amount of registers to read (high) [dec].
   // Amount of registers to read (low) [dec].
   // CRC (low) [dec].
   // CRC (high) [dec].

   uint8_t txBuffer[] = { slaveAddr,
                          0x03,
                          HIGH16(startRegAddr),
                          LOW16(startRegAddr),
                          HIGH16(amountOfRegs),
                          LOW16(amountOfRegs),
                          0,
                          0
                        };

   uint16_t crc = modbusRTU_CRC( txBuffer, sizeof(txBuffer) - 2 );

   txBuffer[6] = LOW16(crc);
   txBuffer[7] = HIGH16(crc);

   uint8_t data = 0;
   uint8_t i = 0;
   uint8_t rxModbusBuffer[6] = { 0, 0, 0, 0, 0, 0 };

   delay_t modbusTimeout;
   delayInit( &modbusTimeout, 150 );
   //int32_t modbusTimeout = 500;

   uartWriteByteArray( uart, txBuffer, sizeof(txBuffer) );

   while( (i<((amountOfRegs*2)+6)) && !delayRead( &modbusTimeout ) ) {
      if( uartReadByte( uart, &data ) ) {
         //uartWriteByte( UART_USB, data );
         if( i>=0 && i<4 ) {
            rxModbusBuffer[i] = data;
            //printf( "%d, %d\r\n", i, data );
         }
         if( i>=4 && i<(amountOfRegs*2)+4 ) {
            rxBuffer[i-4] = data;
            //printf( "%d, %d\r\n", i-4, data );
         }
         if( i>=(amountOfRegs*2)+4 && i<(amountOfRegs*2)+6 ) {
            rxModbusBuffer[i-(amountOfRegs*2)] = data;
            //printf( "%d, %d\r\n", i-(amountOfRegs*2), data );
         }
         i++;
      }
      //delayInaccurateMs(10);
      //modbusTimeout = modbusTimeout - 10;
   }
   //uartWriteString( UART_USB, "\0\0\0\0\0\0\0\0\0\0\0\0" );

   bool_t checkReception;
   if( i == 0 ) { // Si i = 0 ==> Salio por timeout
      checkReception = false;
   } else {
      checkReception = true;
   }

   //                    ADDR  FUN    NBYTES       CRC
   // rxModbusBuffer = { 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX }

   /*
   checkReception = (slaveAddr == rxModbusBuffer[0]);                            // Address
   checkReception = checkReception && ( 0x03 == rxModbusBuffer[1] );             // Modbus Function
   checkReception = checkReception && ( (amountOfRegs*2) == rxModbusBuffer[3] ); // N Bytes
   // CRC
   */

   /*
   printf( "slaveAddr: %d\r\n", rxModbusBuffer[0] );
   printf( "Func: %d\r\n", rxModbusBuffer[1] );
   printf( "amountOfBytes: %d\r\n", (rxModbusBuffer[2]<<8) | rxModbusBuffer[3] );
   printf( "rxBuffer[0]: %d\r\n", rxBuffer[0] );
   printf( "rxBuffer[1]: %d\r\n", rxBuffer[1] );
   printf( "\r\n" );
   */

   delay(5);

   return checkReception;
}






// Open a Modbus RTU connection on selected serial port
bool_t modbusRTU_OpenOnSerialPort( ModbusRTU_t* modbusRTU, uint32_t uart)
{
   return TRUE;
}

// Read from slave with address "slaveAddr", "quantityOfRegs" registers,
// starting from "startRegAddr".
bool_t modbusRTU_RegistersRead( ModbusRTU_t* modbusRTU,
                                uint8_t slaveAddr,
                                uint16_t startRegAddr,
                                uint16_t quantityOfRegs )
{
   // MODBUS RTU FRAME (bytes):

   // Address [dec]: Slave address
   // Function [dec]: 3 (Read Registers)
   // Start read from Register (high) [dec].
   // Start read from Register (low) [dec].
   // Amount of registers to read (high) [dec].
   // Amount of registers to read (low) [dec].
   // CRC (low) [dec].
   // CRC (high) [dec].

   uint8_t txBuffer[] = { slaveAddr,
                          3,
                          HIGH16(startRegAddr),
                          LOW16(startRegAddr),
                          HIGH16(quantityOfRegs),
                          LOW16(quantityOfRegs),
                          0,
                          0
                        };

   uint16_t crc = modbusRTU_CRC( txBuffer, sizeof(txBuffer) - 2 );

   txBuffer[6] = LOW16(crc);
   txBuffer[7] = HIGH16(crc);

   //serialPortWrite( modbusRTU->port, txBuffer, sizeof(txBuffer) );

   return TRUE;
}




// Write "registerValues[]" to slave with address "slaveAddr", "quantityOfRegs"
// registers, starting from "startRegAddr"
bool_t modbusRTU_RegistersWrite( ModbusRTU_t* modbusRTU,
                                 uint8_t slaveAddr,
                                 uint16_t startRegAddr,
                                 uint16_t* registerValues,
                                 uint16_t quantityOfRegs )
{
   return TRUE;
}

// Close a Modbus RTU connection
void modbusRTU_Close( ModbusRTU_t* modbusRTU )
{
	int i = 0;
}

}

/*==================[end of file]============================================*/
