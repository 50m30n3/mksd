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

// This file contains what was supposed to be a full text dump routine for patches.
// When I realised that I would have to write a parser for this to be of any real use I gave up on the idea.
// I included this in case anybody wants to continue my futile work.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "text.h"

char *delaytimebases[15] = { "1/32", "1/24", "1/16", "1/12", "3/32", "1/8", "1/6", "3/16", "1/4", "1/3", "3/8", "1/2", "2/3", "3/4", "1/1" };

char *lfotimebases[15] = { "1/1", "3/4", "2/3", "1/2", "3/8", "1/3", "1/4", "3/16", "1/6", "1/8", "3/32", "1/12", "1/16", "1/24", "1/32" };

unsigned int eqhighfreqs[30] =
{
	1000, 1250, 1500, 1750,
	2000, 2250, 2500, 2750,
	3000, 3250, 3500, 2750,
	4000, 4250, 4500, 4750,
	5000, 5250, 5500, 5750,
	6000, 7000, 8000, 9000,
	10000, 11000, 12000,
	14000, 16000, 18000
};

unsigned int eqlowfreqs[30] =
{
	40, 50, 60, 80,
	100, 120, 140, 160, 180,
	200, 220, 240, 260, 280,
	300, 320, 340, 260, 380,
	400, 420, 440, 460, 480,
	500, 600, 700,
	800, 900, 1000
};

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

char *dwgswaves[64] =
{
	"synsine1",
	"synsine2",
	"synsine3",
	"synsine4",
	"synsine5",
	"synsine6",
	"synsine7",
	"synbass1",
	"synbass2",
	"synbass3",
	"synbass4",
	"synbass5",
	"synbass6",
	"synbass7",
	"synwave1",
	"synwave2",
	"synwave3",
	"synwave4",
	"synwave5",
	"synwave6",
	"synwave7",
	"synwave8",
	"synwave9",
	"5thwave1",
	"5thwave2",
	"5thwave3",
	"digi1",
	"digi2",
	"digi3",
	"digi4",
	"digi5",
	"digi6",
	"digi7",
	"digi8",
	"endless",
	"piano1",
	"piano2",
	"piano3",
	"piano4",
	"organ1",
	"organ2",
	"organ3",
	"organ4",
	"organ5",
	"organ6",
	"organ7",
	"clav1",
	"clav2",
	"guitar1",
	"guitar2",
	"guitar3",
	"bass1",
	"bass2",
	"bass3",
	"bass4",
	"bass5",
	"bell1",
	"bell2",
	"bell3",
	"bell4",
	"voice1",
	"voice2",
	"voice3",
	"voice4"
};

char *osc2mods[4] =
{
	"off",
	"ring",
	"sync",
	"ring/sync"
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

char *patchdest[8] =
{
	"pitch",
	"osc2.pitch",
	"osc1.control",
	"noise.level",
	"cutoff",
	"amp",
	"pan",
	"lfo2.speed"
};

char *patchsrc[8] =
{
	"filter.eg",
	"amp.eg",
	"lfo1",
	"lfo2",
	"velocity",
	"kbd.track",
	"pitch.bend",
	"mod"
};

int generate_timbre_text( unsigned char *data, unsigned int offset, char prefix[], FILE *outfile )
{
	fprintf( outfile, "[%s.channel]\n%hhi\n\n", prefix, (signed char)data[offset+0] );
	
	fprintf( outfile, "[%s.assign]\n", prefix );
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
			fputs( "generate_timre_text: Error in assign (unknown mode)\n", stderr );
			return 0;
		break;
	}

	putc( '\n', outfile );

	fprintf( outfile, "[%s.unision.detune]\n%hhu\n\n", prefix, data[offset+2] );
	
	fprintf( outfile, "[%s.pitch.tune]\n%hhi\n", prefix, data[offset+3]-64 );
	fprintf( outfile, "[%s.pitch.bend.range]\n%hhi\n", prefix, data[offset+4]-64 );
	fprintf( outfile, "[%s.pitch.transpose]\n%hhi\n", prefix, data[offset+5]-64 );
	fprintf( outfile, "[%s.pitch.vibrato]\n%hhi\n", prefix, data[offset+6]-64 );
	fprintf( outfile, "[%s.unision.portamento.time]\n%hhu\n", prefix, data[offset+15] );

	putc( '\n', outfile );
	
	fprintf( outfile, "[%s.osc1.wave]\n", prefix );
	if( data[offset+7] > 7 )
	{
		fputs( "generate_timbre_text: Error in osc1.wave\n", stderr );
		return 0;
	}
	fprintf( outfile, "%s\n", osc1waves[data[offset+7]] );

	fprintf( outfile, "[%s.osc1.ctrl1]\n%hhu\n", prefix, data[offset+8] );
	fprintf( outfile, "[%s.osc1.ctrl2]\n%hhu\n", prefix, data[offset+9] );

	fprintf( outfile, "[%s.osc1.dwgs]\n", prefix );
	if( data[offset+10] > 63 )
	{
		fputs( "generate_timbre_text: Error in osc1.dwgs\n", stderr );
		return 0;
	}
	fprintf( outfile, "%s\n", dwgswaves[data[offset+10]] );

	putc( '\n', outfile );

	fprintf( outfile, "[%s.osc2.wave]\n", prefix );
	if( (data[offset+12]&0x03) > 3 )
	{
		fputs( "generate_timbre_text: Error in osc2.wave\n", stderr );
		return 0;
	}
	fprintf( outfile, "%s\n", osc2waves[data[offset+12]] );

	fprintf( outfile, "[%s.osc2.mod]\n%s\n", prefix , osc2mods[(data[offset+12]&0x30)>>4] );
	fprintf( outfile, "[%s.osc2.semitone]\n%hhi\n", prefix, data[offset+13]-64 );
	fprintf( outfile, "[%s.osc2.tune]\n%hhi\n", prefix, data[offset+14]-64 );

	putc( '\n', outfile );

	fprintf( outfile, "[%s.mixer.osc1]\n%hhu\n", prefix, data[offset+16] );
	fprintf( outfile, "[%s.mixer.osc2]\n%hhu\n", prefix, data[offset+17] );
	fprintf( outfile, "[%s.mixer.noise]\n%hhu\n", prefix, data[offset+18] );
	
	putc( '\n', outfile );

	fprintf( outfile, "[%s.filter.type]\n", prefix );
	if( data[offset+19] > 3 )
	{
		fputs( "generate_timbre_text: Error in filter.type\n", stderr );
		return 0;
	}
	fprintf( outfile, "%s\n", filtertypes[data[offset+19]] );

	fprintf( outfile, "[%s.filter.cutoff]\n%hhu\n", prefix, data[offset+20] );
	fprintf( outfile, "[%s.filter.resonance]\n%hhu\n", prefix, data[offset+21] );
	fprintf( outfile, "[%s.filter.eg]\n%hhi\n", prefix, data[offset+22]-64 );
	fprintf( outfile, "[%s.filter.velocity]\n%hhi\n", prefix, data[offset+23]-64 );
	fprintf( outfile, "[%s.filter.track]\n%hhi\n", prefix, data[offset+24]-64 );

	putc( '\n', outfile );
	
	fprintf( outfile, "[%s.amp.level]\n%hhu\n", prefix, data[offset+25] );
	fprintf( outfile, "[%s.amp.pan]\n%hhi\n", prefix, data[offset+26]-64 );

	fprintf( outfile, "[%s.amp.distortion]\n", prefix );
	if( data[offset+27]&0x01 )
		fputs( "on\n", outfile );
	else
		fputs( "off\n", outfile );

	fprintf( outfile, "[%s.amp.velocity]\n%hhi\n", prefix, data[offset+28]-64 );
	fprintf( outfile, "[%s.amp.track]\n%hhi\n", prefix, data[offset+29]-64 );
	
	putc( '\n', outfile );

	fprintf( outfile, "[%s.eg.trigger]\n", prefix );
	if( data[offset+1]&0x08 )
		fputs( "multi\n", outfile );
	else
		fputs( "single\n", outfile );

	putc( '\n', outfile );

	fprintf( outfile, "[%s.eg.filter.attack]\n%hhu\n", prefix, data[offset+30] );
	fprintf( outfile, "[%s.eg.filter.decay]\n%hhu\n", prefix, data[offset+31] );
	fprintf( outfile, "[%s.eg.filter.sustain]\n%hhu\n", prefix, data[offset+32] );
	fprintf( outfile, "[%s.eg.filter.release]\n%hhu\n", prefix, data[offset+33] );

	fprintf( outfile, "[%s.eg.filter.reset]\n", prefix );
	if( data[offset+1]&0x10 )
		fputs( "on\n", outfile );
	else
		fputs( "off\n", outfile );

	putc( '\n', outfile );

	fprintf( outfile, "[%s.eg.amp.attack]\n%hhu\n", prefix, data[offset+34] );
	fprintf( outfile, "[%s.eg.amp.decay]\n%hhu\n", prefix, data[offset+35] );
	fprintf( outfile, "[%s.eg.amp.sustain]\n%hhu\n", prefix, data[offset+36] );
	fprintf( outfile, "[%s.eg.amp.release]\n%hhu\n", prefix, data[offset+37] );

	fprintf( outfile, "[%s.eg.amp.reset]\n", prefix );
	if( data[offset+1]&0x20 )
		fputs( "on\n", outfile );
	else
		fputs( "off\n", outfile );
	
	putc( '\n', outfile );

	fprintf( outfile, "[%s.lfo1.waveform]\n", prefix );
	switch( data[offset+38]&0x03 )
	{
		case 0:
			fputs( "saw\n", outfile );
		break;
		case 1:
			fputs( "square\n", outfile );
		break;
		case 2:
			fputs( "triangle\n", outfile );
		break;
		case 3:
			fputs( "s-h\n", outfile );
		break;
		default:
			fputs( "generate_timre_text: Error in lfo1.waveform (unknown waveform)\n", stderr );
			return 0;
		break;
	}

	fprintf( outfile, "[%s.lfo1.keysync]\n", prefix );
	switch( (data[offset+38]&0x30)>>4 )
	{
		case 0:
			fputs( "off\n", outfile );
		break;
		case 1:
			fputs( "timbre\n", outfile );
		break;
		case 2:
			fputs( "voice\n", outfile );
		break;
		default:
			fputs( "generate_timre_text: Error in lfo1.keysync (unknown sync mode)\n", stderr );
			return 0;
		break;
	}

	fprintf( outfile, "[%s.lfo1.speed]\n%hhu\n", prefix, data[offset+39] );

	fprintf( outfile, "[%s.lfo1.sync]\n", prefix );
	if( data[offset+40]&0x80 )
		fputs( "on\n", outfile );
	else
		fputs( "off\n", outfile );

	fprintf( outfile, "[%s.lfo1.timebase]\n", prefix );
	if( (data[offset+40]&0x0F) > 15 )
	{
		fputs( "generate_text: Error in lfo1.timebase\n", stderr );
		return 0;
	}
	fprintf( outfile, "%s\n", lfotimebases[data[offset+40]&0x0F] );

	putc( '\n', outfile );

	fprintf( outfile, "[%s.lfo2.waveform]\n", prefix );
	switch( data[offset+41]&0x03 )
	{
		case 0:
			fputs( "saw\n", outfile );
		break;
		case 1:
			fputs( "square\n", outfile );
		break;
		case 2:
			fputs( "sin\n", outfile );
		break;
		case 3:
			fputs( "s-h\n", outfile );
		break;
		default:
			fputs( "generate_timre_text: Error in lfo2.waveform (unknown waveform)\n", stderr );
			return 0;
		break;
	}

	fprintf( outfile, "[%s.lfo2.keysync]\n", prefix );
	switch( (data[offset+41]&0x30)>>4 )
	{
		case 0:
			fputs( "off\n", outfile );
		break;
		case 1:
			fputs( "timbre\n", outfile );
		break;
		case 2:
			fputs( "voice\n", outfile );
		break;
		default:
			fputs( "generate_timre_text: Error in lfo2.keysync (unknown sync mode)\n", stderr );
			return 0;
		break;
	}

	fprintf( outfile, "[%s.lfo2.speed]\n%hhu\n", prefix, data[offset+42] );

	fprintf( outfile, "[%s.lfo2.sync]\n", prefix );
	if( data[offset+43]&0x80 )
		fputs( "on\n", outfile );
	else
		fputs( "off\n", outfile );

	fprintf( outfile, "[%s.lfo2.timebase]\n", prefix );
	if( (data[offset+43]&0x0F) > 15 )
	{
		fputs( "generate_text: Error in lfo2.timebase\n", stderr );
		return 0;
	}
	fprintf( outfile, "%s\n", lfotimebases[data[offset+43]&0x0F] );

	putc( '\n', outfile );

	fprintf( outfile, "[%s.patch1.src]\n%s\n", prefix, patchsrc[data[offset+44]&0x0F] );
	fprintf( outfile, "[%s.patch1.dest]\n%s\n", prefix, patchdest[(data[offset+44]&0xF0)>>4] );
	fprintf( outfile, "[%s.patch1.intensity]\n%hhi\n", prefix, data[offset+45]-64 );

	putc( '\n', outfile );

	fprintf( outfile, "[%s.patch2.src]\n%s\n", prefix, patchsrc[data[offset+46]&0x0F] );
	fprintf( outfile, "[%s.patch2.dest]\n%s\n", prefix, patchdest[(data[offset+46]&0xF0)>>4] );
	fprintf( outfile, "[%s.patch2.intensity]\n%hhi\n", prefix, data[offset+47]-64 );

	putc( '\n', outfile );

	fprintf( outfile, "[%s.patch3.src]\n%s\n", prefix, patchsrc[data[offset+48]&0x0F] );
	fprintf( outfile, "[%s.patch3.dest]\n%s\n", prefix, patchdest[(data[offset+48]&0xF0)>>4] );
	fprintf( outfile, "[%s.patch3.intensity]\n%hhi\n", prefix, data[offset+49]-64 );

	putc( '\n', outfile );

	fprintf( outfile, "[%s.patch4.src]\n%s\n", prefix, patchsrc[data[offset+50]&0x0F] );
	fprintf( outfile, "[%s.patch4.dest]\n%s\n", prefix, patchdest[(data[offset+50]&0xF0)>>4] );
	fprintf( outfile, "[%s.patch4.intensity]\n%hhi\n", prefix, data[offset+51]-64 );

	return 1;
}

int generate_text( unsigned char *data, FILE *outfile )
{
	unsigned int i;

	fputs( "[name]\n", outfile );
	for( i=0; i<12; i++ )
		putc( data[i], outfile );
	fputs( "\n\n", outfile );

	fprintf( outfile, "[octave]\n%hhi\n", (signed char)data[37] );

	fprintf( outfile, "[bpm]\n%hu\n\n", data[31]|(data[30]<<8) );

	fputs( "[mode]\n", outfile );
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

	putc( '\n', outfile );

	fputs( "[delay.type]\n", outfile );
	switch( data[22]&0x3 )
	{
		case 0:
			fputs( "stereo\n", outfile );
		break;
		case 1:
			fputs( "cross\n", outfile );
		break;
		case 2:
			fputs( "l/r\n", outfile );
		break;
		default:
			fputs( "generate_text: Error in delay.type (unknown type)\n", stderr );
			return 0;
		break;
	}

	fputs( "[delay.sync]\n", outfile );
	if( data[19]&0x80 )
		fputs( "on\n", outfile );
	else
		fputs( "off\n", outfile );

	fputs( "[delay.timebase]\n", outfile );
	if( (data[19]&0x0F) > 15 )
	{
		fputs( "generate_text: Error in delay.timebase\n", stderr );
		return 0;
	}
	fprintf( outfile, "%s\n", delaytimebases[data[19]&0x0F] );

	fprintf( outfile, "[delay.time]\n%hhu\n", data[20] );

	fprintf( outfile, "[delay.depth]\n%hhu\n", data[21] );
	
	putc( '\n', outfile );

	fputs( "[fx.type]\n", outfile );
	switch( data[25]&0x3 )
	{
		case 0:
			fputs( "flanger\n", outfile );
		break;
		case 1:
			fputs( "ensemble\n", outfile );
		break;
		case 2:
			fputs( "phaser\n", outfile );
		break;
		default:
			fputs( "generate_text: Error in fx.type (unknown type)\n", stderr );
			return 0;
		break;
	}

	fprintf( outfile, "[fx.lfo.speed]\n%hhu\n", data[23] );
	
	fprintf( outfile, "[fx.depth]\n%hhu\n", data[24] );

	putc( '\n', outfile );

	fputs( "[eq.low.freq]\n", outfile );
	if( data[26] > 29 )
	{
		fputs( "generate_text: Error in eq.low.freq\n", stderr );
		return 0;
	}
	fprintf( outfile, "%uhz\n", eqlowfreqs[data[28]] );

	fprintf( outfile, "[eq.high.gain]\n%hhi\n", data[29]-64 );

	fputs( "[eq.high.freq]\n", outfile );
	if( data[26] > 29 )
	{
		fputs( "generate_text: Error in eq.high.freq\n", stderr );
		return 0;
	}
	fprintf( outfile, "%uhz\n", eqhighfreqs[data[26]] );

	fprintf( outfile, "[eq.high.gain]\n%hhi\n", data[27]-64 );

	putc( '\n', outfile );
	
	fputs( "[arp]\n", outfile );
	if( data[32]&0x80 )
		fputs( "on\n", outfile );
	else
		fputs( "off\n", outfile );

	fprintf( outfile, "[arp.length]\n%hhu\n", (data[14]&0x07)+1 );

	fputs( "[arp.pattern]\n", outfile );
	for( i=0; i<8; i++ )
	{
		if( ((data[15]>>i)&0x01) == 0 )
			putc( '*', outfile );
		else
			putc( '-', outfile );
	}

	putc( '\n', outfile );

	fputs( "[arp.latch]\n", outfile );
		if( data[32]&0x40 )
			fputs( "on\n", outfile );
		else
			fputs( "off\n", outfile );

	fputs( "[arp.target]\n", outfile );
	switch( (data[32]&0x30)>>4 )
	{
		case 0:
			fputs( "both\n", outfile );
		break;
		case 1:
			fputs( "1\n", outfile );
		break;
		case 2:
			fputs( "2\n", outfile );
		break;
		default:
			fputs( "generate_text: Error in arp.target (unknown target)\n", stderr );
			return 0;
		break;
	}
	
	fputs( "[arp.sync]\n", outfile );
	if( data[32]&0x01 )
		fputs( "on\n", outfile );
	else
		fputs( "off\n", outfile );

	fputs( "[arp.type]\n", outfile );
	switch( data[33]&0x7 )
	{
		case 0:
			fputs( "up\n", outfile );
		break;
		case 1:
			fputs( "down\n", outfile );
		break;
		case 2:
			fputs( "alt1\n", outfile );
		break;
		case 3:
			fputs( "alt2\n", outfile );
		break;
		case 4:
			fputs( "random\n", outfile );
		break;
		case 5:
			fputs( "trigger\n", outfile );
		break;
		default:
			fputs( "generate_text: Error in arp.type (unknown type)\n", stderr );
			return 0;
		break;
	}

	fprintf( outfile, "[arp.range]\n%hhu\n", ((data[33]&0xF0)>>4)+1 );
	
	fprintf( outfile, "[arp.gate]\n%hhu\n", data[34] );

	fputs( "[arp.resolution]\n", outfile );
	switch( data[35] )
	{
		case 0:
			fputs( "1/24\n", outfile );
		break;
		case 1:
			fputs( "1/16\n", outfile );
		break;
		case 2:
			fputs( "1/12\n", outfile );
		break;
		case 3:
			fputs( "1/8\n", outfile );
		break;
		case 4:
			fputs( "1/6\n", outfile );
		break;
		case 5:
			fputs( "1/4\n", outfile );
		break;
		default:
			fputs( "generate_text: Error in arp.resolution (unknown resolution)\n", stderr );
			return 0;
		break;
	}

	fprintf( outfile, "[arp.swing]\n%hhi\n", (signed char)data[36] );

	putc( '\n', outfile );
	
	if( ! generate_timbre_text( data, 38, "timbre1", outfile ) )
		return 0;

	putc( '\n', outfile );

	if( ! generate_timbre_text( data, 146, "timbre2", outfile ) )
		return 0;

	return 1;
}

