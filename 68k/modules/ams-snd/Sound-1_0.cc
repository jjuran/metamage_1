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

static inline
Fixed playback_rate_from_sample_rate( long sampleRate )
{
	switch ( sampleRate )
	{
		case rate22khz:  return 0x00010000;
		case rate11khz:  return 0x00008000;
		default:         return (0ull + sampleRate << 16 | 0x8000) / rate22khz;
	}
}

static
OSErr do_bufferCmd( SndChannel* chan, const SndCommand& command )
{
	Ptr addr = (Ptr) command.param2;
	
	const SoundHeader& snd = *(SoundHeader*) addr;
	
	if ( snd.encode != stdSH )
	{
		ERROR = "unimplemented snd header encoding ", snd.encode;
		return unimplemented;
	}
	
	Ptr samples = snd.samplePtr;
	
	if ( ! samples )
	{
		samples = (Ptr) snd.sampleArea;
	}
	
	Size payload_len = snd.length;
	
	Size samples_remaining = payload_len;
	
	const long sampleRate = snd.sampleRate;
	
	if ( payload_len > 30000 )
	{
		if ( sampleRate != rate22khz )
		{
			ERROR = "sampled sound size of ", payload_len, " bytes is too long";
			
			return unimplemented;
		}
		
		payload_len = 370 * 81;  // 29970
	}
	
	Size buffer_size = 6 + payload_len;
	
	Ptr buffer = NewPtr( buffer_size );
	
	if ( buffer == NULL )
	{
		ERROR = "couldn't allocate ", buffer_size, " bytes";
		return MemErr;
	}
	
	OSErr err;
	
	FFSynthRec* freeform = (FFSynthRec*) buffer;
	
	freeform->mode  = ffMode;
	freeform->count = playback_rate_from_sample_rate( sampleRate );
	
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
	
	chan->cmdInProgress.cmd = nullCmd;
	
	return err;
}

static
OSErr do_snd_command( SndChannel* chan, const SndCommand& command )
{
	const int8_t cmd = command.cmd;
	
	switch ( cmd )
	{
		case bufferCmd:
			return do_bufferCmd( chan, command );
		
		default:
			ERROR = "unimplemented Sound Manager command ", cmd;
			return unimplemented;
	}
}

static
short enqueue_command( SndChannel& chan, const SndCommand& command )
{
	short tail = chan.qTail;
	
	if ( ++tail == chan.qLength )
	{
		tail = 0;
	}
	
	bool queued_next = false;
	
	if ( chan.qHead == tail )
	{
		if ( chan.qTail >= 0 )
		{
			/*
				In an empty queue, qTail is set to -1.
				Both -1 and 127 increment to 0 above,
				but only the former succeeds when qHead
				is also 0 (inserting into a previously
				empty queue).  Any non-negative value of
				qTail whose successor ((qTail + 1) % 128)
				matches qHead indicates a full queue.
			*/
			
			return queueFull;
		}
		
		/*
			If the queue was empty (putting this command at the
			head of the queue) and no command is in progress,
			tell SndDoCommand() to start playing the sound.
		*/
		
		queued_next = chan.cmdInProgress.cmd == nullCmd;
	}
	
	chan.qTail = tail;
	
	chan.queue[ tail ] = command;
	
	return queued_next;
}

static
void start_next_command( SndChannel& chan )
{
	if ( chan.qTail < 0 )
	{
		chan.cmdInProgress.cmd = nullCmd;
		
		return;  // queue is empty
	}
	
	chan.cmdInProgress = chan.queue[ chan.qHead ];
	
	/*
		Remove the command from the queue immediately.
	*/
	
	if ( chan.qHead == chan.qTail )
	{
		chan.qTail = -1;
		chan.qHead = 0;
	}
	else if ( ++chan.qHead == chan.qLength )
	{
		chan.qHead = 0;
	}
	
	do_snd_command( &chan, chan.cmdInProgress );
}

static SndChannel* default_channel;

static
SndChannel* get_default_channel()
{
	if ( default_channel == NULL )
	{
		default_channel = (SndChannel*) NewPtrClear( sizeof (SndChannel) );
		
		if ( default_channel )
		{
			default_channel->qLength = stdQLength;  // 128
			default_channel->qTail   = -1;
		}
		else
		{
			ERROR = "couldn't allocate internal sound channel";
		}
	}
	
	return default_channel;
}

pascal
OSErr SndPlay_patch( SndChannel* chan, SndListResource** h, Boolean async )
{
	if ( chan == NULL )
	{
		chan = get_default_channel();
		
		if ( chan == NULL )
		{
			return MemErr;
		}
	}
	
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
		const SndCommand* next = command;
		
		if ( next->cmd != nullCmd )
		{
			SndCommand copy;
			
			if ( next->cmd == bufferCmd + dataOffsetFlag )
			{
				copy.cmd    = bufferCmd;
				copy.param1 = next->param1;
				copy.param2 = next->param2 + (long) *h;
				
				next = &copy;
			}
			
			if ( OSErr err = SndDoCommand( chan, next, false ) )
			{
				return err;
			}
		}
		
		++command;
	}
	
	return noErr;
}

pascal
OSErr SndNewChannel_patch( SndChannel** c, short s, long i, SndCallBackUPP u )
{
	ERROR = "SndNewChannel is unimplemented";
	
	return notEnoughHardwareErr;
}

pascal
OSErr SndDisposeChannel_patch( SndChannel* chan, Boolean quietNow )
{
	ERROR = "SndDisposeChannel is unimplemented";
	
	return noErr;
}

pascal
OSErr SndDoCommand_patch( SndChannel* chan, SndCommand* cmd, Boolean noWait )
{
	short queued_next = enqueue_command( *chan, *cmd );
	
	if ( queued_next > 0 )
	{
		start_next_command( *chan );
		
		queued_next = 0;  // return noErr
	}
	
	return queued_next;  // either noErr or queueFull
}
