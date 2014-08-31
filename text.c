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

#include "text.h"

char *osc1waves[8] = 
{
	"saw",
	"pulse",
	"triangle",
	"sin",
	"vox",
	"dwgs",
	"noise",
	"audio"
};

char *osc2waves[3] =
{
	"saw",
	"square",
	"triangle"
};

char *filtertypes[4] =
{
	"24lpf",
	"12lpf",
	"12bpf",
	"12hpf"
};

int generate_timbre_text( unsigned char *data, unsigned int offset, char prefix[], FILE *outfile )
{
	fprintf( outfile, "%svoice: ", prefix );
	switch( (data[offset+1]&0xC0)>>6 )
	{
		case 0:
			fputs( "mono\n", outfile );
		break;
		case 1:
			fputs( "poly\n", outfile );
		break;
		case 2:
			fputs( "unision\n", outfile );
		break;
		default:
			fputs( "generate_timre_text: Error in voice (unknown mode)\n", stderr );
			return 0;
		break;
	}

	fprintf( outfile, "%sosc 1: ", prefix );
	if( data[offset+7] > 7 )
	{
		fputs( "generate_timbre_text: Error in osc1\n", stderr );
		return 0;
	}
	fprintf( outfile, "%s\n", osc1waves[data[offset+7]] );

	fprintf( outfile, "%sosc 2: ", prefix );
	if( (data[offset+12]&0x03) > 3 )
	{
		fputs( "generate_timbre_text: Error in osc2\n", stderr );
		return 0;
	}
	fprintf( outfile, "%s\n", osc2waves[data[offset+12]&0x03] );

	fprintf( outfile, "%sfilter: ", prefix );
	if( data[offset+19] > 3 )
	{
		fputs( "generate_timbre_text: Error in filter\n", stderr );
		return 0;
	}
	fprintf( outfile, "%s\n", filtertypes[data[offset+19]] );

	fprintf( outfile, "%sdistortion: ", prefix );
	if( data[offset+27]&0x01 )
		fputs( "on\n", outfile );
	else
		fputs( "off\n", outfile );

	return 1;
}

int generate_vocoder_text( unsigned char *data, unsigned int offset, char prefix[], FILE *outfile )
{
	fprintf( outfile, "%svoice: ", prefix );
	switch( (data[offset+1]&0xC0)>>6 )
	{
		case 0:
			fputs( "mono\n", outfile );
		break;
		case 1:
			fputs( "poly\n", outfile );
		break;
		case 2:
			fputs( "unision\n", outfile );
		break;
		default:
			fputs( "generate_timre_text: Error in voice (unknown mode)\n", stderr );
			return 0;
		break;
	}

	fprintf( outfile, "%sosc: ", prefix );
	if( data[offset+7] > 7 )
	{
		fputs( "generate_timbre_text: Error in osc1\n", stderr );
		return 0;
	}
	fprintf( outfile, "%s\n", osc1waves[data[offset+7]] );

	fprintf( outfile, "%sdistortion: ", prefix );
	if( data[offset+29]&0x01 )
		fputs( "on\n", outfile );
	else
		fputs( "off\n", outfile );

	return 1;
}

int generate_text( unsigned char *data, FILE *outfile )
{
	unsigned int i;

	fputs( "name: \"", outfile );
	for( i=0; i<12; i++ )
		putc( data[i], outfile );
	fputs( "\"\n", outfile );

	fprintf( outfile, "bpm: %hu\n", data[31]|(data[30]<<8) );

	fputs( "type: ", outfile );
	switch( (data[16]&0x30)>>4 )
	{
		case 0:
			fputs( "synth\n", outfile );
		break;
		case 2:
			fputs( "layer\n", outfile );
		break;
		case 3:
			fputs( "vocoder\n", outfile );
		break;
		default:
			fputs( "generate_text: Error in voice.mode (unknown mode)\n", stderr );
			return 0;
		break;
	}

	fputs( "arpeggiator: ", outfile );
	if( data[32]&0x80 )
		fputs( "on\n", outfile );
	else
		fputs( "off\n", outfile );

	fputs( "latch: ", outfile );
		if( data[32]&0x40 )
			fputs( "on\n", outfile );
		else
			fputs( "off\n", outfile );

	if( ((data[16]&0x30)>>4) == 3 )
	{
		if( ! generate_vocoder_text( data, 38, "", outfile ) )
			return 0;
	}
	else
	{
		if( ((data[16]&0x30)>>4) == 2 )
		{
			if( ! generate_timbre_text( data, 38, "timbre 1 -> ", outfile ) )
				return 0;

			if( ! generate_timbre_text( data, 146, "timbre 2 -> ", outfile ) )
				return 0;
		}
		else
		{
			if( ! generate_timbre_text( data, 38, "", outfile ) )
				return 0;
		}
	}

	return 1;
}

