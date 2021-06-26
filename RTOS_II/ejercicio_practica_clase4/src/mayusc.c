/*
 * mayusc.c
 *
 *  Created on: 7 oct. 2019
 *      Author: Gonzalo Gontad
 */
#include "mayusc.h"
#include <string.h>
void Check_packet( char* string )
{
    int i=2;//indice arranca en uno porque el primer byte es el tamaño del string

    int largo = string[0];

    if (string[1] != '(')
    {
    	strcpy( string, " ERROR_B" );
		string[0]=7; //Indico cuantos bytes tiene el string
		return;
    }

    if (string[largo] != ')')
        {
        	strcpy( string, " ERROR_E" );
    		string[0]=7; //Indico cuantos bytes tiene el string
    		return;
        }

    while ( largo > i+1 )
    {

        if ( ( string[i]>='0' )&&( string[i]<='9' ) ) //si el caracter esta entre a y z los paso a mayuscula
        {
            //string[i]=string[i]+( 'A'-'a' );
        }
        else
        {
            strcpy( string, " ERROR_N" );
            string[0]=7; //Indico cuantos bytes tiene el string
            return;
        }
        i++;
    }
}



