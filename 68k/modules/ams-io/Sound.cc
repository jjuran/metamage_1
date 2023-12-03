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
#include "time.hh"


#pragma exceptions off


#define STR_LEN( s )  "" s, (sizeof s - 1)


const short noQueueMask   = 1 << noQueueBit;
const short asyncTrapMask = 1 << asyncTrpBit;

static inline
size_t byte_distance( const void* begin, const void* end )
{
	return (const char*) end - (const char*) begin;
}

static inline
ssize_t send_command( UInt16 domain, const void* buffer, UInt32 buffer_length )
{
	return send_sound_command( domain, buffer, buffer_length );
}

static
UInt32 sizeof_SWSynthRec( const SWSynthRec* rec, UInt16 length )
{
	short n = (length - sizeof rec->mode) / sizeof (Tone);
	
	const Tone* tone = rec->triplets;
	
	while ( n-- > 0  &&  tone++->count > 0 )
	{
		// Find the trailing count == 0 or run out of data
	}
	
	return byte_distance( rec, tone );
}

static Wave stocked_wave_1;
static Wave stocked_wave_2;
static Wave stocked_wave_3;
static Wave stocked_wave_4;

static Wave zeroWave;

static inline
WavePtr checked( WavePtr wave )
{
	return wave ? wave : zeroWave;
}

static inline
int Wave_cmp( const Wave a, const Wave b )
{
	if ( a == NULL )
	{
		/*
			If either pointer is NULL, return 0 to avoid dereferencing
			either in the caller.  However, b is a stocked wave, never NULL.
		*/
		
		return 0;
	}
	
	return fast_memcmp( a, b, sizeof (Wave) );
}

static inline
void Wave_cpy( Wave dst, const Wave src )
{
	return fast_memcpy( dst, src, sizeof (Wave) );
}

static
ssize_t start_sound( const void* buffer, UInt32 length )
{
	using namespace sndpipe;
	
	FTSynthRec_flat_buffer flat;
	
	const short mode = *(short*) buffer;
	
	if ( mode == swMode )
	{
		/*
			Many applications write bytes beyond the terminating Tone.
			We have no idea what sort of information might be in there,
			so be safe (and minimize network traffic) by truncating it.
			
			Otherwise, sndtrack reports diagnostics for malformed input.
		*/
		
		length = sizeof_SWSynthRec( (const SWSynthRec*) buffer, length );
	}
	else if ( mode == ftMode )
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
		
		Wave_cpy( stocked_wave_1, flat.sound1Wave );
		Wave_cpy( stocked_wave_2, flat.sound2Wave );
		Wave_cpy( stocked_wave_3, flat.sound3Wave );
		Wave_cpy( stocked_wave_4, flat.sound4Wave );
		
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
		
		++a.tv_sec;
	}
	
	a.tv_usec = usec;
}

static inline
int FTSoundRec_cmp( const FTSoundRec& a, const FTSoundRec& b )
{
	return fast_memcmp( &a, &b, sizeof (FTSoundRec) );
}

static inline
void FTSoundRec_cpy( FTSoundRec& dst, const FTSoundRec& src )
{
	return fast_memcpy( &dst, &src, sizeof (FTSoundRec) );
}

static FTSoundRec* current_FTSound;
static FTSoundRec  copy_of_FTSoundRec;

static bool FTSound_was_modified;

static VBLTask SoundVBL;

static
void update_wave( short channel, Wave current, Wave stocked )
{
	if ( Wave_cmp( current, stocked ) != 0 )
	{
		using namespace sndpipe;
		
		Wave_cpy( stocked, current );
		
		FTSynthRec_wave_update update;
		
		update.mode    = ftMode_wave_update;
		update.recID   = (RecID) current_FTSound;
		update.channel = channel;
		
		Wave_cpy( update.waveform, stocked );
		
		send_command( admin_domain, &update, sizeof update );
	}
}

static
pascal void SoundVBL_Proc()
{
	FTSoundRec& current = *current_FTSound;
	
	update_wave( 1 - 1, current.sound1Wave, stocked_wave_1 );
	update_wave( 2 - 1, current.sound2Wave, stocked_wave_2 );
	update_wave( 3 - 1, current.sound3Wave, stocked_wave_3 );
	update_wave( 4 - 1, current.sound4Wave, stocked_wave_4 );
	
	if ( FTSoundRec_cmp( *current_FTSound, copy_of_FTSoundRec ) != 0 )
	{
		using namespace sndpipe;
		
		FTSoundRec_cpy( copy_of_FTSoundRec, *current_FTSound );
		
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
	
	timeval duration;
	
	timeval_from_nanoseconds( &duration, &duration_nanoseconds );
	
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
	
	DCtlEntry* dce = *GetDCtlEntry( kSoundDriverRefNum );
	
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
	
	if ( pb )
	{
		pb->ioActCount = pb->ioReqCount;
	}
	
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
	
	const short immed = pb->ioTrap & noQueueMask;
	
	if ( ! immed )
	{
		IODone( dce, err );
	}
	
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
	
	const short immed = pb->ioTrap & noQueueMask;
	
	if ( ! immed )
	{
		IODone( dce, err );
	}
	
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
	
	FSWrite( kSoundDriverRefNum, &size, (Ptr) buffer );
}
