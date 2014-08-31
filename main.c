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
#include <unistd.h>
#include <string.h>

#include "types.h"
#include "midi.h"
#include "ident.h"
#include "sysex.h"
#include "text.h"

int do_ident( unsigned int channel, FILE *midiport )
{
	struct ident *ident;

	if( channel >= 16 )
	{
		fputs( "do_ident: channel out of range\n", stderr );
		return 0;
	}

	if( midiport == NULL )
	{
		fputs( "do_ident: no midi device set\n", stderr );
		return 0;
	}

	ident = get_ident( channel, midiport );

	if( ident != NULL )
	{
		printf( "Manufacturer: 0x%02hhx\n", ident->man );
		printf( "Family ID: 0x%04hx\n", ident->family );
		printf( "Member ID: 0x%04hx\n", ident->member );
		printf( "Version: %hi.%hi\n", ident->version_major, ident->version_minor );

		if( ident->man != 0x42 )
		{
			puts( "Device is not a MicroKORG (Wrong manufacturer ID)" );
		}
		else if( ident->family != 0x0058 )
		{
			puts( "Device is not a MicroKORG (Wrong family ID)" );
		}
		else if( ident->member != 0x0011 )
		{
			puts( "Device is not a MicroKORG (Wrong member ID)" );
		}
		else
		{
			puts( "Device is a MicroKORG" );
		}
		free( ident );
		return 1;
	}

	return 0;
}

int do_dump( unsigned int channel, FILE *midiport, char *filename )
{
	unsigned char *data;
	struct ident *ident;

	if( channel >= 16 )
	{
		fputs( "do_dump: channel out of range\n", stderr );
		return 0;
	}

	if( midiport == NULL )
	{
		fputs( "do_dump: no midi device set\n", stderr );
		return 0;
	}

	ident = get_ident( channel, midiport );

	if( ident != NULL )
	{
		if( ident->man != 0x42 )
		{
			fputs( "do_dump: Device is not a MicroKORG (Wrong manufacturer ID)\n", stderr );
			return 0;
		}
		if( ident->family != 0x0058 )
		{
			fputs( "do_dump: Device is not a MicroKORG (Wrong family ID)\n", stderr );
			return 0;
		}
		if( ident->member != 0x0011 )
		{
			fputs( "do_dump: Device is not a MicroKORG (Wrong member ID)\n", stderr );
			return 0;
		}
		free( ident );
	}
	else
	{
		return 0;
	}

	if( ! send_command( channel, 0x58, 0x10, midiport ) )
		return 0;
	
	data = get_data( 297, midiport );

	if( data != NULL )
	{
		if( ! save_sysex( data, filename ) )
		{
			free( data );
			return 0;
		}
		else
		{
			free( data );
			return 1;
		}
	}
	else
	{
		return 0;
	}
}

int do_upload( unsigned int channel, FILE *midiport, char *filename )
{
	unsigned char *data;
	struct ident *ident;

	if( channel >= 16 )
	{
		fputs( "do_dump: channel out of range\n", stderr );
		return 0;
	}

	if( midiport == NULL )
	{
		fputs( "do_dump: no midi device set\n", stderr );
		return 0;
	}

	ident = get_ident( channel, midiport );

	if( ident != NULL )
	{
		if( ident->man != 0x42 )
		{
			fputs( "do_dump: Device is not a MicroKORG (Wrong manufacturer ID)\n", stderr );
			return 0;
		}
		if( ident->family != 0x0058 )
		{
			fputs( "do_dump: Device is not a MicroKORG (Wrong family ID)\n", stderr );
			return 0;
		}
		if( ident->member != 0x0011 )
		{
			fputs( "do_dump: Device is not a MicroKORG (Wrong member ID)\n", stderr );
			return 0;
		}
		free( ident );
	}
	else
	{
		return 0;
	}

	data = load_sysex( filename );
	
	if( data != NULL )
	{
		data[2] = 0x30+channel;

		send_data( data, 297, midiport );
		free( data );

		data = get_data( 6, midiport );
		
		if( data != NULL )
		{
			if( 
				( data[0] != 0xF0 ) ||
				( data[1] != 0x42 ) ||
				( data[2] != 0x30+channel ) ||
				( data[3] != 0x58 ) ||
				( data[4] != 0x23 ) 
			)
			{
				fputs( "do_upload: upload failed\n", stderr );
				return 0;
			}
		}

		return 1;
	}
	else
	{
		return 0;
	}
}

int do_info( char *inputfilename )
{
	unsigned char *data;
	unsigned char *rawdata;
	unsigned char buffer;
	unsigned int i, j;

	data = load_sysex( inputfilename );

	if( data != NULL )
	{
		rawdata = malloc( sizeof( unsigned char ) * 254 );
		if( rawdata == NULL )
		{
			perror( "do_info: malloc:" );
			return 0;
		}

		j = 0;
		for( i=0; i<291; i++ )
		{
			if( i % 8 == 0 )
			{
				buffer = data[5+i];
				continue;
			}
			rawdata[j++] = data[5+i]|((buffer&0x01)<<7);
			buffer = buffer >> 1;
		}

		if( ! generate_text( rawdata, stdout ) )
		{
			free( rawdata );
			free( data );
			return 0;
		}
		else
		{
			free( rawdata );
			free( data );
			return 1;
		}
	}
	else
	{
		return 0;
	}
}

int do_rename( char *inputfilename, char *outputfilename, char *newname )
{
	unsigned char *data;
	int i, j;

	data = load_sysex( inputfilename );
	
	if( data != NULL )
	{
		if( strlen( newname ) > 12 )
		{
			fputs( "do_rename: new name must be shorter than 12 characters\n", stderr );
			return 0;
		}

		i = 0;
		for( j = 0; j<14; j++ )
		{
			if( j % 8 == 0 )
				continue;

			if( i < strlen( newname ) )
			{
				if( newname[i] & 0x80 )
				{
					fputs( "do_rename: illegal char in new name\n", stderr );
					return 0;
				}
				else
					data[5+j] = newname[i++];
			}
			else
				data[5+j] = ' ';
		}
		
		if( ! save_sysex( data, outputfilename ) )
		{
			free( data );
			return 0;
		}
		else
		{
			free( data );
			return 1;
		}
	}
	else
	{
		return 0;
	}
}

void print_help( void )
{
	puts( "mksd v.1.0 (c) 50m30n3 2011" );
	puts( "USAGE: mksd -m device -c channel -i inputfile -o outputfile action" );
	puts( "Options:" );
	puts( "\t-h\tDisplay help" );
	puts( "\t-v\tDisplay version" );
	puts( "\t-m dev\tSet midi device\t\t\t\t/dev/midi" );
	puts( "\t-c chan\tSet midi channel\t\t\t1" );
	puts( "\t-i file\tSet input filename (\"-\" for stdin)\t-" );
	puts( "\t-o file\tSet output filename (\"-\" for stdout)\t-" );
	puts( "Actions:" );
	puts( "\t-I\tIdentify device" );
	puts( "\t-d\tDownload current program" );
	puts( "\t-u\tUpload program to device" );
	puts( "\t-t\tShow text info about patch" );
	puts( "\t-r name\tRename a patch (max 12 chars)" );
}

void print_version( void )
{
	puts( "mksd v.1.0" );
}

enum action
{
	help,
	version,
	ident,
	upload,
	download,
	info,
	name
};

int main( int argc, char *argv[] )
{
	int opt, result;
	unsigned int channel;
	char *inputfilename;
	char *outputfilename;
	char *midifilename;
	char *newname;
	FILE *midiport;
	enum action action;

	midifilename = NULL;
	inputfilename = NULL;
	outputfilename = NULL;
	newname = NULL;
	channel = 1;
	action = help;
	result = 0;

	while( ( opt = getopt( argc, argv, "hvm:c:i:o:r:tIdu" ) ) != -1 )
	{
		switch( opt )
		{
			case 'h':
				action = help;
			break;

			case 'v':
				action = version;
			break;

			case 'm':
				midifilename = strdup( optarg );
			break;

			case 'c':
				sscanf( optarg, "%u", &channel );
			break;

			case 'i':
				inputfilename = strdup( optarg );
			break;

			case 'o':
				outputfilename = strdup( optarg );
			break;

			case 'r':
				action = name;
				newname = strdup( optarg );
			break;

			case 'I':
				action = ident;
			break;

			case 'd':
				action = download;
			break;

			case 'u':
				action = upload;
			break;

			case 't':
				action = info;
			break;

			default:
			case '?':
				fputs( "main: cannot parse commandline\n", stderr );                                                                           
				return 1;
			break;
		}
	}

	if( midifilename == NULL )
		midifilename = strdup( "/dev/midi" );

	if( inputfilename == NULL )
		inputfilename = strdup( "-" );

	if( outputfilename == NULL )
		outputfilename = strdup( "-" );

	if( ( channel < 1 ) || ( channel > 16 ) )
	{
		fputs( "main: channel out of range\n", stderr );                                                                           
		return 1;
	}

	channel--;

	if( action == help )
	{
		print_help();
	}

	if( action == version )
	{
		print_version();
	}

	if( action == info )
	{
		result = do_info( inputfilename );
	}

	if( action == name )
	{
		result = do_rename( inputfilename, outputfilename, newname );
		free( newname );
	}

	if( ( action == ident ) || ( action == upload ) || ( action == download ) )
	{
		midiport = fopen( midifilename, "r+" );
		if( midiport == NULL )
		{
			perror( "main: fopen" );
			return 1;
		}

		if( action == ident )
		{
			result = do_ident( channel, midiport );
		}

		if( action == download )
		{
			result = do_dump( channel, midiport, outputfilename );
		}

		if( action == upload )
		{
			result = do_upload( channel, midiport, inputfilename );
		}

		fclose( midiport );
	}

	free( inputfilename );
	free( outputfilename );

	return 0;
}

