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

#ifndef MIDI_H
#define MIDI_H

extern int send_command( unsigned int channel, unsigned char device, unsigned char command, FILE *midiport );
extern unsigned char *get_data( unsigned int length, FILE *midiport );
extern int send_data( unsigned char *data, unsigned int length, FILE *midiport );

#endif
