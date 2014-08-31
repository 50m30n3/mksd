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
#include <string.h>

#include "types.h"
#include "midi.h"
#include "ident.h"

#include "sysex.h"

int save_sysex( unsigned char *data, char filename[] )
{
	FILE *syxfile;

	if( strcmp( filename, "-" ) == 0 )
		syxfile = stdout;
	else
		syxfile = fopen( filename, "w" );

	if( syxfile == NULL )
	{
		perror( "save_sysex: fopen:" );
		return 0;
	}

	if( fwrite( data, sizeof( unsigned char ), 297, syxfile ) != 297 )
	{
		perror( "save_sysex: fwrite:" );
		return 0;
	}

	fclose( syxfile );

	return 1;
}

unsigned char *load_sysex( char filename[] )
{
	FILE *syxfile;
	unsigned char *data;
	
	if( strcmp( filename, "-" ) == 0 )
		syxfile = stdin;
	else
		syxfile = fopen( filename, "r" );

	if( syxfile == NULL )
	{
		perror( "load_sysex: fopen:" );
		return NULL;
	}
	
	data = malloc( sizeof( unsigned char ) * 297 );
	if( data == NULL )
	{
		perror( "load_sysex: malloc:" );
		return NULL;
	}

	if( fread( data, sizeof( unsigned char), 297, syxfile ) != 297 )
	{
		fputs( "load_sysex: fread: short read\n", stderr );
		free( data );
		return NULL;
	}

	if( 
		( data[0] != 0xF0 ) ||
		( data[1] != 0x42 ) ||
		( (data[2]&0xF0) != 0x30 ) ||
		( data[3] != 0x58 ) ||
		( data[4] != 0x40 ) 
	)
	{
		fputs( "load_sysex: not a microkorg program dump\n", stderr );
		free( data );
		return 0;
	}

	return data;
}

