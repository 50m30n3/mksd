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

#include "ident.h"

struct ident *get_ident( unsigned int channel, FILE *midiport )
{
	unsigned char *data;
	struct ident *ident;

	if( channel >= 16 )
	{
		fputs( "get_ident: channel out of range\n", stderr );
		return NULL;
	}

	fputc( 0xF0, midiport );
	fputc( 0x7E, midiport );
	fputc( channel, midiport );
	fputc( 0x06, midiport );
	fputc( 0x01, midiport );
	fputc( 0xF7, midiport );

	data = get_data( 15, midiport );

	if( data != NULL )
	{
		if( 
			( data[0] != 0xF0 ) ||
			( data[1] != 0x7E ) ||
			( data[2] != channel ) ||
			( data[3] != 0x06 ) ||
			( data[4] != 0x02 ) 
		)
		{
			fputs( "get_ident: incorrect reply\n", stderr );
			return NULL;
		}

		ident = malloc( sizeof( struct ident ) );
		if( ident == NULL )
		{
			perror( "get_ident: malloc:" );
			return NULL;
		}

		ident->man = data[5];
		ident->family = data[6] + ( data[7] << 7 );
		ident->member = data[8] + ( data[9] << 7 );
		ident->version_minor = data[10] + ( data[11] << 7 );
		ident->version_major = data[12] + ( data[13] << 7 );

		return ident;
	}
	else
	{
		return NULL;
	}
}

