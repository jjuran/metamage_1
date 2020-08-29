/*
	Sound.cc
	--------
*/

#include "Sound.hh"

// Mac OS
#ifndef __DEVICES__
#include <Devices.h>
#endif
#ifndef __RETRACE__
#include <Retrace.h>
#endif

// POSIX
#include <sys/uio.h>

// sndpipe
#include "sndpipe/sndpipe.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// mac-snd-utils
#include "mac_snd/duration.hh"

// ams-common
#include "callouts.hh"
#include "interrupts.hh"
#include "reactor.hh"
#include "reactor-gestalt.hh"
#include "options.hh"
#include "time.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)


static
ssize_t send_command( UInt16 domain, const void* buffer, UInt32 buffer_length )
{
	/*
		Callers of this function pass a buffer that includes a `mode` field,
		but the wire protocol regards the mode as part of an 8-byte header --
		therefore the length (which covers only the extent) should exclude it.
	*/
	
	UInt32 length = buffer_length - sizeof (short);
	
	const int n_iov = 3;
	
	iovec iov[ n_iov ] =
	{
		{ (void*) &length, sizeof length },
		{ (void*) &domain, sizeof domain },
		{ (void*) buffer,  buffer_length },
	};
	
	return writev( sound_fd, iov, n_iov );
}

static Wave zeroWave;

static inline
WavePtr checked( WavePtr wave )
{
	return wave ? wave : zeroWave;
}

static
ssize_t start_sound( const void* buffer, UInt32 length )
{
	using namespace sndpipe;
	
	FTSynthRec_flat_buffer flat;
	
	if ( *(short*) buffer == ftMode )
	{
		flat.mode = ftMode_flat_buffer;
		
		const FTSoundRec* sndRec = ((const FTSynthRec*) buffer)->sndRec;
		
		flat.recID = (RecID) sndRec;
		
		const int n_from_sndRec = offsetof( FTSoundRec, sound1Wave );
		
		fast_memcpy( &flat.duration, sndRec, n_from_sndRec );
		
		const int wave_len = sizeof (Wave);
		
		fast_memcpy( flat.sound1Wave, checked( sndRec->sound1Wave ), wave_len );
		fast_memcpy( flat.sound2Wave, checked( sndRec->sound2Wave ), wave_len );
		fast_memcpy( flat.sound3Wave, checked( sndRec->sound3Wave ), wave_len );
		fast_memcpy( flat.sound4Wave, checked( sndRec->sound4Wave ), wave_len );
		
		buffer = &flat;
		length = sizeof flat;
	}
	
	return send_command( sound_domain, buffer, length );
}

static inline
ssize_t send_basic_command( uint16_t command )
{
	return send_command( sndpipe::basic_domain, &command, sizeof command );
}

static inline
ssize_t begin_audio()
{
	return send_basic_command( sndpipe::switch_on );
}

static inline
ssize_t abort_sound()
{
	return send_basic_command( sndpipe::full_stop );
}

static timer_node Sound_timer_node;
static bool       timer_scheduled;

static inline
reactor_core_parameter_block* reactor_core()
{
	typedef reactor_core_parameter_block pb_t;
	
	pb_t* reactor = (pb_t*) mac::sys::gestalt( gestaltReactorCoreAddr );
	
	return reactor;
}

static
void timeval_add( timeval& a, const timeval& b )
{
	a.tv_sec += b.tv_sec;
	
	uint32_t usec = a.tv_usec + b.tv_usec;
	
	if ( usec >= 1000000 )
	{
		usec -= 1000000;
	}
	
	a.tv_usec = usec;
}

static inline
bool operator==( const FTSoundRec& a, const FTSoundRec& b )
{
	return memcmp( &a, &b, sizeof (FTSoundRec) ) == 0;
}

static inline
bool operator!=( const FTSoundRec& a, const FTSoundRec& b )
{
	return !(a == b);
}

static FTSoundRec* current_FTSound;
static FTSoundRec  copy_of_FTSoundRec;

static bool FTSound_was_modified;

static VBLTask SoundVBL;

static
pascal void SoundVBL_Proc()
{
	if ( *current_FTSound != copy_of_FTSoundRec )
	{
		using namespace sndpipe;
		
		copy_of_FTSoundRec = *current_FTSound;
		
		FTSound_was_modified = true;
		
		FTSynthRec_flat_update flat;
		
		flat.mode = ftMode_flat_update;
		flat.recID = (RecID) current_FTSound;
		
		const int n_from_sndRec = offsetof( FTSoundRec, sound1Wave );
		
		fast_memcpy( &flat.duration, &copy_of_FTSoundRec, n_from_sndRec );
		
		send_command( admin_domain, &flat, sizeof flat );
	}
	
	--current_FTSound->duration;
	--copy_of_FTSoundRec.duration;
	
	SoundVBL.vblCount = 1;
}

static
void schedule_timer( IOParam* pb, uint64_t duration_nanoseconds )
{
	time( &Sound_timer_node.wakeup );
	
	const uint64_t duration_microseconds = duration_nanoseconds / 1000;
	
	const timeval duration =
	{
		duration_microseconds / 1000000,
		duration_microseconds % 1000000,
	};
	
	timeval_add( Sound_timer_node.wakeup, duration );
	
	timer_scheduled = true;
	
	reactor_core()->schedule( &Sound_timer_node );
	
	if ( *(short*) pb->ioBuffer == ftMode )
	{
		const FTSynthRec& synth = *(const FTSynthRec*) pb->ioBuffer;
		
		current_FTSound = synth.sndRec;
		
		fast_memcpy( &copy_of_FTSoundRec, synth.sndRec, sizeof (FTSoundRec) );
		
		FTSound_was_modified = false;
		
		SoundVBL.vblAddr  = &SoundVBL_Proc;
		SoundVBL.vblCount = 1;
		
		OSErr err = VInstall( (QElem*) &SoundVBL );
	}
}

static
void cancel_timer()
{
	short saved_SR = disable_interrupts();
	
	if ( timer_scheduled )
	{
		OSErr err = VRemove( (QElem*) &SoundVBL );
		
		reactor_core()->cancel( &Sound_timer_node );
		
		timer_scheduled = false;
		
		current_FTSound = NULL;
	}
	
	reenable_interrupts( saved_SR );
}

static
void Sound_ready( timer_node* node )
{
	OSErr err = VRemove( (QElem*) &SoundVBL );
	
	timer_scheduled = false;
	
	DCtlEntry* dce = *GetDCtlEntry( -4 );
	
	IOParam* pb = (IOParam*) dce->dCtlQHdr.qHead;
	
	if ( const FTSoundRec* sndRec = current_FTSound )
	{
		if ( FTSound_was_modified  &&  sndRec->duration != 0 )
		{
			const int ns_per_sec = 1000 * 1000 * 1000;
			
			int64_t duration = sndRec->duration * (ns_per_sec * 100ull) / 6015;
			
			schedule_timer( pb, duration );
			return;
		}
	}
	
	current_FTSound = NULL;
	
	pb->ioActCount = pb->ioReqCount;
	
	IODone( dce, noErr );
}

OSErr Sound_open( short trap_word : __D1, IOParam* pb : __A0, DCE* dce : __A1 )
{
	static ssize_t begun = begin_audio();  // send this message first
	
	if ( begun < 0 )
	{
		return openErr;
	}
	
	return noErr;
}

OSErr Sound_prime( short trap_word : __D1, IOParam* pb : __A0, DCE* dce : __A1 )
{
	OSErr err = noErr;
	
	int64_t duration = mac::snd::duration( pb->ioBuffer, pb->ioReqCount );
	
	if ( duration < 0 )
	{
		err = paramErr;
		goto done;
	}
	
	pb->ioResult = 1;
	
	short saved_SR = disable_interrupts();
	
	Sound_timer_node.ready = &Sound_ready;
	
	if ( ! timer_scheduled )
	{
		start_sound( pb->ioBuffer, pb->ioReqCount );
		
		schedule_timer( pb, duration );
	}
	
	reenable_interrupts( saved_SR );
	
	return noErr;  // queued
	
done:
	
	IODone( dce, err );
	
	return err;
}

short Sound_control( short trap : __D1, CntrlParam* pb : __A0, DCE* dce : __A1 )
{
	OSErr err = noErr;
	
	switch ( pb->csCode )
	{
		case killCode:
			abort_sound();
			
			cancel_timer();
			
			return pb->ioResult = noErr;
		
		default:
			err = controlErr;
			break;
	}
	
	IODone( dce, err );
	
	return err;
}

OSErr Sound_close( short trap_word : __D1, IOParam* pb : __A0, DCE* dce : __A1 )
{
	return noErr;
}

pascal void SysBeep_patch( short duration )
{
	enum
	{
		swMode = -1,
	};
	
	uint16_t buffer[] = { swMode, 650, 255, duration, 0, 0, 0 };
	
	SInt32 size = sizeof buffer;
	
	FSWrite( -4, &size, (Ptr) buffer );
}
