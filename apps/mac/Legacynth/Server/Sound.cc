/*
	Sound.cc
	--------
	
	Legacynth Server for classic Mac OS
	
	Copyright 2022-2025, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	Legacynth implements the Sound Driver API in traditional Mac OS
	environments that don't.
	
	This application is a sound server and API adapter, converting
	I/O requests to the Sound Driver into Sound Manager commands.
	
	This module implements the sound server, handling intercepted
	I/O requests and generating sounds using the Sound Manager.
	
*/

#include "Sound.hh"

// Mac OS
#ifndef __DEVICES__
#include <Devices.h>
#endif
#ifndef __FIXMATH__
#include <FixMath.h>
#endif
#ifndef __MIXEDMODE__
#include <MixedMode.h>
#endif
#ifndef __SOUND__
#include <Sound.h>
#endif

// SoundDriver
#include "SoundDriver/SoundDriver.h"

// mac-config
#include "mac_config/upp-macros.hh"

// mac-sys-utils
#include "mac_sys/delay.hh"

// legacynth-common
#include "legacynth/common.hh"

// LegacynthServer
#include "lock_high_bit.hh"
#include "squarewave.hh"
#include "status.hh"


#define LOWMEM( addr, type )  (*(type*) (addr))

#define SoundDCE   LOWMEM( 0x027A, DCtlEntry* )
#define CurrentA5  LOWMEM( 0x0904, void*      )


enum
{
	noQueueMask   = 1 << noQueueBit,
	asyncTrapMask = 1 << asyncTrpBit,
};

static ProcessSerialNumber psn;

static DCtlEntry pseudo_dce;

static exported_state global_state;

/*
	Our buffer is large enough for a two-second-long
	square-wave tone sequence.  For most cases, that
	will be enough, and this is indeed the Simplest
	Thing That Could Possibly Work.  However, we'll
	need to retool it later to use double-buffering
	and generate additional audio at interrupt time.
*/

enum
{
	kOctetsPerSample =   1,
	kSamplesPerFrame = 370,
	kFramesPerSecond =  60,  // (approximate)
	
	kBufferSize = kOctetsPerSample * kSamplesPerFrame * kFramesPerSecond * 2,
};

struct PrivateStorage
{
	SndChannelPtr channel;
	SoundHeader   header;
	char          buffer[ kBufferSize ];
};

static PrivateStorage* private_storage;

static
void notify_sound_changed( bool active )
{
	sound_active = active;
	sound_changed = true;
	
	/*
		We don't need to check for Process Manager presence,
		because we only get here in System 7 or later.
	*/
	
	WakeUpProcess( &psn );
}

#ifdef __MC68K__
#pragma parameter Sound_prime( __A0, __A1 )
#endif

static
OSErr Sound_prime( IOParam* pb, DCtlPtr dce );

#ifdef __MC68K__
#pragma parameter IOComplete( __A0, __D0 )
#endif

static
OSErr IOComplete( IOParam* pb, short err )
{
	pb->ioResult = err;
	
	if ( pb->ioCompletion )
	{
		InvokeIOCompletionUPP( (ParmBlkPtr) pb, pb->ioCompletion );
	}
	
	return err;
}

#ifdef __MC68K__
#pragma parameter IOLock( __A0 )
#endif

static inline
Boolean IOLock( IOParam* pb )
{
	return lock_high_bit( &pb->ioTrap );
}

#ifdef __MC68K__
#pragma parameter IONext( __A1 )
#endif

static
OSErr IONext( DCtlEntry* dce )
{
	if ( IOParam* pb = (IOParam*) dce->dCtlQHdr.qHead )
	{
		if ( IOLock( pb ) )
		{
			return Sound_prime( pb, dce );
		}
	}
	
	return noErr;
}

#ifdef __MC68K__
#pragma parameter MyIODone( __A1, __D0 )
#endif

static
OSErr MyIODone( DCtlEntry* dce, OSErr err )
{
	notify_sound_changed( false );
	
	QElemPtr head = dce->dCtlQHdr.qHead;
	
	IOParam* pb = (IOParam*) head;
	
	if ( OSErr queue_err = Dequeue( head, &dce->dCtlQHdr ) )
	{
		/*
			Another IODone() call beat us to the punch.  Bail out now,
			so we don't call the completion handler a second time.
		*/
		
		return queue_err;
	}
	
	IOComplete( pb, err );
	
	return IONext( dce );
}

static
pascal void sound_callback( SndChannelPtr channel, SndCommand* command )
{
	channel;
	
	DCtlEntry* dce = (DCtlEntry*) command->param2;
	
	MyIODone( dce, noErr );
}

DEFINE_UPP( SndCallBack, sound_callback )

/*
	MWC68K honors `#pragma parameter` on a forward declaration,
	and then ignores it for the actual function definition,
	so we have to (conditionally) use `foo : __REG` syntax.
*/

#ifdef __MC68K__
#define IN_REGISTER( x )  : x
#else
#define IN_REGISTER( x )  /**/
#endif

static
OSErr Sound_prime( IOParam*  pb  IN_REGISTER( __A0 ),
                   DCtlPtr   dce IN_REGISTER( __A1 ) )
{
	++pb->ioResult;  // Bump ioResult to 2 to mark the sound as active
	
	PrivateStorage& storage = *(PrivateStorage*) dce->dCtlStorage;
	
	SndChannelPtr channel = storage.channel;
	
	SoundHeader& sh = storage.header;
	
	OSErr err = paramErr;
	
	const short* p16 = (const short*) pb->ioBuffer;
	
	short mode = *p16++;
	
	if ( mode == ffMode )
	{
		const long* p32 = (const long*) p16;
		
		long rate = *p32++;
		
		sh.samplePtr  = (Ptr) p32;
		sh.length     = pb->ioReqCount - 6;
		sh.sampleRate = FixMul( rate, rate22khz );
		
		SndCommand playback = { bufferCmd,   0, (long) &sh };
		SndCommand callback = { callBackCmd, 0, (long) dce };
		
		(err = SndDoCommand( channel, &playback, true ))  ||
		(err = SndDoCommand( channel, &callback, true ));
	}
	else if ( mode == swMode )
	{
		const Ptr end = pb->ioBuffer + pb->ioReqCount;
		const Ptr last = end - sizeof (Tone);
		
		const Tone* tone = (const Tone*) p16;
		
		long extra = squarewave_fill( tone, last, storage.buffer, kBufferSize );
		
		sh.samplePtr  = storage.buffer;
		sh.length     = kBufferSize - extra;
		sh.sampleRate = rate22khz;
		
		SndCommand playback = { bufferCmd,   0, (long) &sh };
		SndCommand callback = { callBackCmd, 0, (long) dce };
		
		(err = SndDoCommand( channel, &playback, true ))  ||
		(err = SndDoCommand( channel, &callback, true ));
	}
	
	if ( err )
	{
		return MyIODone( dce, err );
	}
	
	sound_mode = mode;
	
	notify_sound_changed( true );
	
	return err;
}

#ifdef __MC68K__
#pragma parameter Sound_Write( __A0 )
#endif

static
OSErr Sound_Write( IOParam* pb )
{
	if ( ! private_storage )
	{
		return IOComplete( pb, notOpenErr );
	}
	
	if ( pb->ioTrap & noQueueMask )
	{
		return IOComplete( pb, paramErr );
	}
	
	pb->ioResult = ioInProgress;
	
	pb->qType   = ioQType;
	pb->ioTrap &= ~0x8000;  // clear high bit to mark as unserviced
	
	Enqueue( (QElemPtr) pb, &SoundDCE->dCtlQHdr );
	
	OSErr err = IONext( SoundDCE );
	
	if ( err )
	{
		return IOComplete( pb, err );
	}
	
	if ( (pb->ioTrap & asyncTrapMask) == 0 )
	{
		while ( pb->ioResult > 0 )
		{
			mac::sys::delay( 1 );
		}
	}
	
	return err;
}

static
OSErr stop_sound()
{
	OSErr err = noErr;
	
	if ( SndChannelPtr channel = private_storage->channel )
	{
		SndCommand flush = { flushCmd };
		SndCommand quiet = { quietCmd };
		
		(err = SndDoImmediate( channel, &flush ))  ||
		(err = SndDoImmediate( channel, &quiet ));
	}
	
	while ( QElemPtr head = SoundDCE->dCtlQHdr.qHead )
	{
		if ( Dequeue( head, &SoundDCE->dCtlQHdr ) == noErr )
		{
			IOParam* pb = (IOParam*) head;
			
			IOComplete( pb, abortErr );
		}
	}
	
	notify_sound_changed( false );
	
	return err;
}

#ifdef __MC68K__
#pragma parameter Sound_KillIO( __A0 )
#endif

static
OSErr Sound_KillIO( IOParam* pb )
{
	return pb->ioResult = stop_sound();
}

static const UInt32 kPBIOTrapProcInfo
	= kRegisterBased
	| REGISTER_ROUTINE_PARAMETER( 1, kRegisterA0, SIZE_CODE( sizeof (Ptr) ) );

#ifndef __MC68K__
#define BUILD_UPP( info, proc ) \
static RoutineDescriptor proc##_upp[] = BUILD_ROUTINE_DESCRIPTOR( info, &proc );

BUILD_UPP( kPBIOTrapProcInfo, Sound_Write  )
BUILD_UPP( kPBIOTrapProcInfo, Sound_KillIO )
#endif

static CntrlParam Control_pb;

static
OSErr check_installability()
{
	if ( has_Sound_Driver() )
	{
		return dupFNErr;  // we don't replace a working .Sound driver
	}
	
	/*
		System 7+ is required to get here, so we have the Process Manager.
	*/
	
	GetCurrentProcess( &psn );
	
	/*
		This call will succeed if Legacynth is installed,
		and fail if the non-functioning .Sound driver stub
		on PPC machines answers it.  (Note that this call
		*has* been observed to return noErr when fielded by
		the functioning .Sound driver in System 6 on a Mac II
		(in Mini vMac), even though it obviously can't work,
		which is why we check for such environments above.)
	*/
	
	Control_pb.ioCRefNum = kSoundDriverRefNum;
	Control_pb.csCode    = kLegacynthQueryInstalled;
	
	OSErr err = PBControlSync( (ParmBlkPtr) &Control_pb );
	
	if ( err == noErr )
	{
		const long* p = (const long*) Control_pb.csParam;
		
		extension_protocol_version = *p++;
		extension_software_version = *p++;
	}
	
	return err;
}

short Sound_installed()
{
	static OSErr err = check_installability();
	
	return err;
}

OSErr Sound_startup()
{
	OSErr err = Sound_installed();  // sets Control_pb.ioCRefNum
	
	if ( err < 0 )
	{
		return err;
	}
	
	if ( (long) SoundDCE != -1 )
	{
		return fBsyErr;  // another Legacynth server is already running
	}
	
	private_storage = (PrivateStorage*) NewPtrClear( sizeof (PrivateStorage) );
	
	if ( ! private_storage )
	{
		return memFullErr;
	}
	
	err = SndNewChannel( &private_storage->channel,
	                     sampledSynth,
	                     initNoDrop | initMono,
	                     UPP_ARG( sound_callback ) );
	
	if ( err )
	{
		goto no_channel;
	}
	
	pseudo_dce.dCtlFlags = dOpenedMask
	                     | dWritEnableMask
	                     | dCtlEnableMask
	                     | dNeedLockMask;
	
	pseudo_dce.dCtlStorage = (Handle) private_storage;
	pseudo_dce.dCtlRefNum  = kSoundDriverRefNum;
	
	SoundDCE = &pseudo_dce;
	
	if ( TARGET_CPU_68K )
	{
		global_state.a5 = CurrentA5;
	}
	
	global_state.write  = (void*) UPP_ARG( Sound_Write  );
	global_state.killIO = (void*) UPP_ARG( Sound_KillIO );
	
	Control_pb.csCode = kLegacynthSetBaseAddress;
	
	*(void**) Control_pb.csParam = &global_state;
	
	err = PBControlSync( (ParmBlkPtr) &Control_pb );
	
	if ( err )
	{
		SndDisposeChannel( private_storage->channel, true );
		
	no_channel:
		
		DisposePtr( (Ptr) private_storage );
		
		private_storage = NULL;
	}
	
	return err;
}

OSErr Sound_cleanup()
{
	if ( private_storage )
	{
		/*
			check_installability() sets Control_pb.ioCRefNum for us.
			It's called (once) by Sound_installed(), and never again.
			
			If Sound_startup() has been called, then Control_pb.csCode
			is already set to kLegacynthSetBaseAddress.  If it hasn't,
			then we don't need to do anything to clean up.
		*/
		
		if ( ! *(long*) Control_pb.csParam )
		{
			// Startup hasn't happened (or at least, not since cleanup).
			
			return noErr;
		}
		
		*(void**) Control_pb.csParam = NULL;
		
		PBControlSync( (ParmBlkPtr) &Control_pb );
		
		stop_sound();
		
		SoundDCE = (DCtlPtr) -1;
		
		SndDisposeChannel( private_storage->channel, true );
		
		DisposePtr( (Ptr) private_storage );
		
		private_storage = NULL;
	}
	
	return noErr;
}

/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
