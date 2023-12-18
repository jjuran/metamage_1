/*
	rate.hh
	-------
*/

#ifndef RATE_HH
#define RATE_HH


typedef unsigned FTRate;

typedef int MIDI_note;

#ifdef __MC68K__
#pragma parameter rate_from_midi_note( __D0 )
#endif

FTRate rate_from_midi_note( MIDI_note midi_note );

#endif
