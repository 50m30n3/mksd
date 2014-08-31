/*
*    This file is part of mksd.
*
*    mksd is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    mksd is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with mksd.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>

#include "types.h"

#include "midi.h"

int send_command( unsigned int channel, unsigned char device, unsigned char command, FILE *midiport )
{
	if( channel >= 16 )
	{
		fputs( "send_command: channel out of range\n", stderr );
		return 0;
	}

	fputc( 0xF0, midiport );
	fputc( 0x42, midiport );
	fputc( 0x30+channel, midiport );
	fputc( device, midiport );
	fputc( command, midiport );
	fputc( 0xF7, midiport );

	return 1;
}

unsigned char *get_data( unsigned int length, FILE *midiport )
{
	unsigned char *data;
	unsigned char buffer;
	int wait;
	unsigned int i, timeout;

	wait = 1;
	i = 0;

	timeout = 50;

	data = malloc( sizeof( unsigned char ) * length );
	if( data == NULL )
	{
		perror( "get_data: malloc:" );
		return NULL;
	}

	do
	{
		buffer = fgetc( midiport );
		timeout--;

		if( buffer == 0xF0 )
		{
			wait = 0;
			data[0] = 0xF0;
			i = 1;
		}

		if( buffer == 0xF7 )
		{
			data[i++] = 0xF7;
		}

		if( wait ) continue;

		if( buffer & 0x80 )  continue;

		timeout = 10;

		data[i++] = buffer;
	}
	while( ( buffer != 0xF7 ) && ( i < length ) && ( timeout > 0 ) );

	if( timeout <= 0 )
		fputs( "get_data: timeout\n", stderr );

	if( ( i != length ) || ( buffer != 0xF7 ) )
	{
		fputs( "get_data: transmission error\n", stderr );
		free( data );
		return NULL;
	}
	else
	{
		return data;
	}
}

int send_data( unsigned char *data, unsigned int length, FILE *midiport )
{
	return ( fwrite( data, sizeof( unsigned char ), length, midiport ) == length );
}

