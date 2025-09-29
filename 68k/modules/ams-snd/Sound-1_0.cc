/*
	Sound-1_0.cc
	------------
*/

#include "Sound-1_0.hh"

// Mac OS
#ifndef __DEVICES__
#include <Devices.h>
#endif
#ifndef __SOUND__
#include <Sound.h>
#endif

// SoundDriver
#include "SoundDriver/SoundDriver.h"

// mac-glue-utils
#include "mac_glue/OSUtils.hh"

// log-of-war
#include "logofwar/report.hh"

// ams-common
#include "callouts.hh"
#include "interrupts.hh"

// ams-snd
#include "admin.hh"
#include "buffers.hh"


#pragma exceptions off


short MemErr : 0x0220;

int max_channels;

static int n_channels;

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
	
	Fixed playback_rate = playback_rate_from_sample_rate( snd.sampleRate );
	
	if ( payload_len > 30000 )
	{
		if ( playback_rate != 0x00010000 )
		{
			ERROR = "sampled sound size of ", payload_len, " bytes is too long";
			
			return unimplemented;
		}
		
		payload_len = 370 * 81;  // 29970
	}
	
	Size buffer_size = 6 + payload_len;
	
	OSErr err;
	
	do
	{
		audio_buffer* buffer = alloc_buffer();
		
		if ( buffer == NULL )
		{
			ERROR = "bufferCmd: audio buffers exhausted";
			
			return notEnoughBufferSpace;
		}
		
		buffer->ch = chan;
		
		buffer->ff.count = playback_rate;
		
		Byte* output = (Byte*) buffer->ff.waveBytes;
		
		fast_memcpy( output, samples, payload_len );
		
		samples           += payload_len;
		samples_remaining -= payload_len;
		
		ParamBlockRec& pb = buffer->pb;
		IOParam& io = pb.ioParam;
		
		io.ioPosOffset = samples_remaining;
		
	//	io.ioRefNum = kSoundDriverRefNum;
	//	io.ioBuffer = buffer;
		io.ioReqCount = buffer_size;
	//	io.ioCompletion = NULL;
		io.ioVRefNum = 0;
		io.ioPosMode = 0;
		
		err = PBWriteAsync( &pb );
		
		if ( samples_remaining < payload_len )
		{
			payload_len = samples_remaining;
			buffer_size = 6 + payload_len;
		}
	}
	while ( samples_remaining > 0  &&  err == noErr );
	
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
	short saved_SR = disable_interrupts();
	
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
			
			reenable_interrupts( saved_SR );
			
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
	
	reenable_interrupts( saved_SR );
	
	return queued_next;
}

static
void start_next_command( SndChannel& chan )
{
next:
	
	short saved_SR = disable_interrupts();
	
	if ( chan.qTail < 0 )
	{
		chan.cmdInProgress.cmd = nullCmd;
		
		reenable_interrupts( saved_SR );
		
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
	
	reenable_interrupts( saved_SR );
	
	/*
		Run any consecutive admin commands immediately, in a loop.
	*/
	
	OSErr err = do_admin_command( &chan, chan.cmdInProgress );
	
	if ( err == noErr )
	{
		goto next;
	}
	
	if ( err == unimplemented )
	{
		do_snd_command( &chan, chan.cmdInProgress );
	}
}

static
OSErr audio_completion( ParamBlockRec* pb : __A0 )
{
	IOParam& io = pb->ioParam;
	
	io.ioResult = noErr;
	
	const int pb_offset = offsetof(audio_buffer, pb);
	
	audio_buffer* buffer = (audio_buffer*) ((char*) pb - pb_offset);
	
	return_buffer( buffer );
	
	SndChannel* chan = buffer->ch;
	
	if ( io.ioPosOffset == 0 )
	{
		start_next_command( *chan );
	}
	
	return noErr;
}

static
void init_buffer( audio_buffer& buffer )
{
	buffer.next = NULL;
	
	IOParam& io = buffer.pb.ioParam;
	
	io.ioRefNum = kSoundDriverRefNum;
	io.ioBuffer = (Ptr) &buffer.ff;
	io.ioCompletion = (IOCompletionProcPtr) &audio_completion;
	io.ioVRefNum = 0;
	io.ioPosMode = 0;
	
	buffer.ff.mode = ffMode;
}

static
void create_buffers( int n )
{
	while ( n-- > 0 )
	{
		audio_buffer* buffer = (audio_buffer*) NewPtr( sizeof (audio_buffer) );
		
		init_buffer( *buffer );
		
		return_buffer( buffer );
	}
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
		
		async = false;
	}
	
	if ( free_audio_buffer == NULL )
	{
		create_buffers( 2 );
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
	
	if ( ! async )
	{
		while ( chan->qTail >= 0 )
		{
			mac::glue::delay( -1 );  // calls reactor_wait() once
		}
	}
	
	return noErr;
}

pascal
OSErr SndNewChannel_patch( SndChannel** c, short s, long i, SndCallBackUPP u )
{
	if ( n_channels >= max_channels )
	{
		NOTICE = "SndNewChannel: channel count is already ", n_channels;
		
		return notEnoughHardwareErr;
	}
	
	SndChannel* chan = *c;
	
	if ( chan == NULL )
	{
		chan = get_default_channel();
		
		if ( chan == NULL )
		{
			return MemErr;
		}
		
		*c = chan;
	}
	
	chan->callBack = u;
	
	if ( free_audio_buffer == NULL )
	{
		create_buffers( 2 );
	}
	
	++n_channels;
	
	return noErr;
}

pascal
OSErr SndDisposeChannel_patch( SndChannel* chan, Boolean quietNow )
{
	if ( chan == NULL )
	{
		if ( (chan = default_channel) == NULL )
		{
			return badChannel;
		}
	}
	
	SndCommand flush;  flush.cmd = flushCmd;
	SndCommand quiet;  quiet.cmd = quietCmd;
	
	/*
		If quietNow is set, then we immediately flush (emptying the
		queue) and quiet (terminating any currently playing sound).
		
		If it's clear, we queue a quietCmd (as documented).  We also
		queue a flushCmd in case anything got queued while we waited.
	*/
	
	if ( quietNow )
	{
		do_admin_command( chan, flush );
		do_admin_command( chan, quiet );
	}
	else
	{
		SndDoCommand( chan, &quiet, false );
		SndDoCommand( chan, &flush, false );
	}
	
	if ( chan == default_channel )
	{
		DisposePtr( (Ptr) chan );
		
		default_channel = NULL;
	}
	
	--n_channels;
	
	return noErr;
}

pascal
OSErr SndDoCommand_patch( SndChannel* chan, SndCommand* cmd, Boolean noWait )
{
	bool warned = false;
	
retry:
	
	short queued_next = enqueue_command( *chan, *cmd );
	
	if ( queued_next < 0  &&  ! warned )
	{
		WARNING = "SndDoCommand: channel's queue is full";
		
		warned = true;
	}
	
	if ( queued_next < 0  &&  ! noWait )
	{
		/*
			enqueue_command() returned queueFull.  If we're
			waiting for a queue slot, delay and try again.
		*/
		
		mac::glue::delay( -1 );  // calls reactor_wait() once
		
		goto retry;
	}
	
	if ( queued_next > 0 )
	{
		start_next_command( *chan );
		
		queued_next = 0;  // return noErr
	}
	
	return queued_next;  // either noErr or queueFull
}

pascal
OSErr SndDoImmediate_patch( SndChannel* chan, SndCommand* command )
{
	if ( command->cmd == bufferCmd )
	{
		if ( chan->qTail < 0 )
		{
			NOTICE = "SndDoImmediate: processing bufferCmd via SndDoCommand";
			
			return SndDoCommand( chan, command, true );
		}
		
		WARNING = "SndDoImmediate: rejecting bufferCmd with non-empty queue";
		
		return channelBusy;
	}
	
	return do_admin_command( chan, *command );
}
