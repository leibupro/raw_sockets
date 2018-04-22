/* 
 * ############################################################
 *
 *    File:          failwhale.c
 *
 *
 *    Purpose:       Print the failwhale to 
 *                   an arbitrary fstream.
 *
 *
 *    Remarks:       -
 *
 *
 *    Author:        P. Leibundgut <leiu@zhaw.ch>
 *
 *
 *    Date:          11/2015
 *
 * ############################################################
 */

#include <stdio.h>
#include <failwhale.h>


void failwhale( FILE* out )
{
  ( void )fprintf( out,  
                   "\n\n"
                   "          ... oh, look, it's the ...\n\n\n"


                   "           ▄██████████████▄▐█▄▄▄▄█▌\n"
                   "           ██████▌▄▌▄▐▐▌███▌▀▀██▀▀\n"
                   "           ████▄█▌▄▌▄▐▐▌▀███▄▄█▌\n"
                   "           ▄▄▄▄▄██████████████▀\n\n\n"


                   "          ...::: FAIL WHALE! :::...\n"
                   "\n\n"
         );
}

