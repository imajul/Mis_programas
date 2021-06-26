/*=============================================================================
 * Copyright (c) 2019, Agustin Curcio Berardi <acurber91@gmail.com>
 * All rights reserved.
 * License: mit (see LICENSE.txt)
 * Date: 2019/09/22
 * Version: v1.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 * CRC8-CCITT, with normal polynomial; 0x07.
 *
 *===========================================================================*/


/*=====[Inclusion of own header]=============================================*/

#include "crc8.h"

/*=====[Inclusions of private function dependencies]=========================*/

/*=====[Definition macros of private constants]==============================*/

/*=====[Private function-like macros]========================================*/

/*=====[Definitions of private data types]===================================*/

/*=====[Definitions of external public global variables]=====================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Prototypes (declarations) of private functions]======================*/

/*=====[Implementations of public functions]=================================*/


uint8_t str_to_checksum( char *mensaje , uint8_t tamanio )
{

	uint8_t i,checksum,sub_check,check_par,check_impar;

    check_par = 0;
    check_impar = 0;

    for(i = 2 ; i < tamanio-1 ; i++)
    {
    	if (i%2 == 0)
		{
    		check_par = check_par + (mensaje[i]-'0');
		}
    	else
    	{
    		check_impar = check_impar + (mensaje[i]-'0');
    	}
    }
    sub_check = 3*check_par + check_impar;


    checksum = (10 - (sub_check % 10)) % 10;

    return checksum;
}


/*=====[Implementations of interrupt functions]==============================*/

/*=====[Implementations of private functions]================================*/


