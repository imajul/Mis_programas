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

#ifndef _MODBUS_RTU_H_
#define _MODBUS_RTU_H_

/*==================[inclusions]=============================================*/

#include "sapi.h"

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

#define HIGH16(var)   (((var)>>8) & 0x00FF)
#define LOW16(var)    ((var) & 0x00FF)

/*==================[typedef]================================================*/

//typedef SerialPort_t ModbusRTU_t;
typedef int ModbusRTU_t;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/


void modbusMasterRtuInit( int32_t uart, uint32_t baudRate, 
                          uint8_t dataBits, uint8_t parity, uint8_t stopBits );

// Func 0x06
void modbusMasterRtuWriteRegister( int32_t uart, uint8_t slaveAddr, 
                                   uint16_t regAddr, uint16_t regValue );

// Func 0x03
bool_t modbusMasterRtuReadRegisters( int32_t uart, uint8_t slaveAddr, 
                                     uint16_t startRegAddr, uint16_t amountOfRegs,
                                     uint8_t* rxBuffer );
                                     
                                     

// Open a Modbus RTU connection on selected serial port
bool_t modbusRTU_OpenOnSerialPort( ModbusRTU_t* modbusRTU, uint32_t uart );

// Read from slave with address "slaveAddr", "quantityOfRegs" registers,
// starting from "startRegAddr".
bool_t modbusRTU_RegistersRead( ModbusRTU_t* modbusRTU,
                                uint8_t slaveAddr,
                                uint16_t startRegAddr,
                                uint16_t quantityOfRegs );

// Write "registerValues[]" to slave with address "slaveAddr", "quantityOfRegs"
// registers, starting from "startRegAddr"
bool_t modbusRTU_RegistersWrite( ModbusRTU_t* modbusRTU,
                                 uint8_t slaveAddr,
                                 uint16_t startRegAddr,
                                 uint16_t* registerValues,
                                 uint16_t quantityOfRegs );

// Close a Modbus RTU connection
void modbusRTU_Close( ModbusRTU_t* modbusRTU );

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* #ifndef _MODBUS_RTU_H_ */
