/*
	Sound-1_0.cc
	------------
*/

#include "Sound-1_0.hh"

// Mac OS
#ifndef __SOUND__
#include <Sound.h>
#endif

// SoundDriver
#include "SoundDriver/SoundDriver.h"

// log-of-war
#include "logofwar/report.hh"

// ams-common
#include "callouts.hh"


#pragma exceptions off


enum
{
	/*
		Apparently noHardwareErr isn't used for anything -- at least nothing
		deserving of mention in Apple's Sound Manager 3.3 documentation -- so
		we'll use it here for things that are yet to be implemented.  (There's
		also unimpErr, but that's used to indicate that a particular software
		operation is unsupported by a hardware device, e.g. volume control.)
	*/
	
	unimplemented = noHardwareErr,
};

short MemErr : 0x0220;

typedef SndListResource SndList;

static
OSErr do_bufferCmd( SndChannel* chan, SndList** h, const SndCommand& command )
{
	Ptr addr = (Ptr) command.param2;
	
	if ( command.cmd & dataOffsetFlag )
	{
		addr += (long) *h;
	}
	
	const SoundHeader* snd = (SoundHeader*) addr;
	
	if ( snd->encode != stdSH )
	{
		ERROR = "unimplemented snd header encoding ", snd->encode;
		return unimplemented;
	}
	
	Ptr samples = snd->samplePtr ? snd->samplePtr : (Ptr) snd->sampleArea;
	
	if ( snd->length > 30000  &&  snd->sampleRate != rate22khz )
	{
		ERROR = "sampled sound size of ", snd->length, " bytes is is too long";
		return unimplemented;
	}
	
	Size payload_len = snd->length;
	
	if ( snd->length > 30000 )
	{
		payload_len = 370 * 80;  // 29600
	}
	
	Size buffer_size = 6 + payload_len;
	
	Ptr buffer = NewPtr( buffer_size );
	
	if ( buffer == NULL )
	{
		ERROR = "couldn't allocate ", buffer_size, " bytes";
		return MemErr;
	}
	
	Fixed playback_rate;
	
	switch ( snd->sampleRate )
	{
		case rate22khz:
			playback_rate = 0x00010000;
			break;
		
		case rate11khz:
			playback_rate = 0x00008000;
			break;
		
		default:
			playback_rate = (0ull + snd->sampleRate << 16 | 0x8000) / rate22khz;
			break;
	}
	
	OSErr err;
	
	FFSynthRec* freeform = (FFSynthRec*) buffer;
	
	freeform->mode  = ffMode;
	freeform->count = playback_rate;
	
	Size samples_remaining = snd->length;
	
	do
	{
		fast_memcpy( freeform->waveBytes, samples, payload_len );
		
		err = FSWrite( -4, &buffer_size, buffer );
		
		samples           += payload_len;
		samples_remaining -= payload_len;
		
		if ( samples_remaining < payload_len )
		{
			payload_len = samples_remaining;
			buffer_size = 6 + payload_len;
		}
	}
	while ( samples_remaining > 0  &&  err == noErr );
	
	DisposePtr( buffer );
	
	return err;
}

static
OSErr do_snd_command( SndChannel* chan, SndList** h, const SndCommand& command )
{
	const int8_t cmd = command.cmd;
	
	switch ( cmd )
	{
		case nullCmd:
			return noErr;
		
		case bufferCmd:
			return do_bufferCmd( chan, h, command );
		
		default:
			ERROR = "unimplemented Sound Manager command ", cmd;
			return unimplemented;
	}
}

pascal
short SndPlay_patch( SndChannel* c, SndListResource** h, Boolean async )
{
	const short format = h[0]->format;
	const short n_mods = h[0]->numModifiers;
	const short n_cmds = h[0]->numCommands;
	
	if ( format != 1 )
	{
		ERROR = "unimplemented 'snd ' resource format ", format;
		return unimplemented;
	}
	
	if ( n_mods != 1 )
	{
		ERROR = "'snd ' resource must have exactly one data type, not ", n_mods;
		return unimplemented;
	}
	
	const ModRef& mod = h[0]->modifierPart[ 0 ];
	
	if ( mod.modNumber != sampledSynth )
	{
		ERROR = "only sampledSynth is supported, not ", mod.modNumber;
		return unimplemented;
	}
	
	if ( n_cmds == 0 )
	{
		ERROR = "'snd ' resource expected to have at least one command";
		return unimplemented;
	}
	
	const SndCommand* command = h[0]->commandPart;
	
	for ( int i = 0;  i < n_cmds;  ++i )
	{
		if ( OSErr err = do_snd_command( c, h, *command++ ) )
		{
			return err;
		}
	}
	
	return noErr;
}

pascal
short SndNewChannel_patch( SndChannel** c, short s, long i, SndCallBackUPP u )
{
	ERROR = "SndNewChannel is unimplemented";
	
	return notEnoughHardwareErr;
}

pascal
short SndDisposeChannel_patch( SndChannel** c )
{
	ERROR = "SndDisposeChannel is unimplemented";
	
	return noErr;
}
