/*
	async.cc
	--------
*/

#include "async.hh"

// Mac OS
#ifndef __DEVICES__
#include <Devices.h>
#endif
#ifndef __FIXMATH__
#include <FixMath.h>
#endif

// log-of-war
#include "logofwar/report.hh"

// ams-snd
#include "buffers.hh"


enum
{
	/*
		Apparently noHardwareErr isn't used for anything -- at least
		nothing deserving of mention in Apple's Sound Manager 3.3
		documentation -- so we'll use it here for things that are
		yet to be implemented.  (There's also unimpErr, but that's
		used to indicate that a particular software operation is
		unsupported by a hardware device, e.g. volume control.)
	*/
	
	unimplemented = -200,  // same as noHardwareErr
};

/*
	You'd think that
	
		f( buffer->ff.waveBytes, packets, cluster_len );
	
	would work, but Metrowerks' codegen is faulty:
	
		PEA      resume
		MOVE.L   f,-(A7)
		LEA      buffer->ff.waveBytes,A1
		MOVEA.L  packets,A0
		MOVE.L   len,D0
		RTS
	resume:
		ADDQ.W   #4,A7
	
	The compiler correctly recognizes that it can't
	jump through either A0 or A1, because both are
	used for parameters.  No other address registers
	are considered volatile, so it can't use a JSR.
	
	Conceptually, a JSR can be split into a push / JMP
	combo.  Although JMP is unavailable for the same
	reason, it in turn can be replaced by push / RTS.
	
	The compiler in fact applies this exact pattern.
	First it pushes a return address onto the stack,
	then the jump target address, and after loading the
	parameter registers, it effects a jump with RTS.
	
	The problem is the last instruction.  The function
	address has been popped by the generated RTS, and
	the return address has been popped by the function's
	own RTS, returning the stack to its original state.
	The subsequent superfluous pop corrupts the stack.
	
	As a workaround, we call a utility function (using
	JSR with either a PC-relative address or a relocated
	absolute address) that uses the push / RTS pattern:
	
		call_cpy( f, buffer->ff.waveBytes, packets, cluster_len );
*/

static
asm
void call_cpy( cpy f : __D2, void* : __A1, const void* : __A0, long : __D0 )
{
	MOVE.L   D2,-(SP)
	RTS
}

static inline
asm
short divu_w( UInt32 dividend : __D0, UInt16 divisor : __D1 )
{
	DIVU.W   D1,D0
}

static inline
Fixed playback_rate_from_sample_rate( UnsignedFixed sampleRate )
{
	enum
	{
		rate22khz = 0x56EE8BA3,
		rate11khz = 0x2B7745D1,
	};
	
	switch ( sampleRate )
	{
		case rate22khz:  return 0x00010000;
		case rate11khz + 1:  // TaskMaker does this
		case rate11khz:  return 0x00008000;
		default:         return FixDiv( sampleRate, rate22khz );
	}
}

OSErr play_async( SndChannel* chan, Ptr p, long n, UFixed rate, cpy f, int x )
{
	Byte* packets = (Byte*) p;
	
	Size packets_remaining = n;
	Size cluster_len       = n;
	
	Fixed playback_rate = playback_rate_from_sample_rate( rate );
	
	if ( cluster_len > divu_w( 30000, x ) )
	{
		if ( playback_rate != 0x00010000  &&  playback_rate != 0x00008000 )
		{
			ERROR = "sampled sound size of ", cluster_len, " bytes is too long";
			
			return unimplemented;
		}
		
		cluster_len = divu_w( 370 * 81, x );  // 29970 / x (for x = 3, 9995)
	}
	
	OSErr err;
	
	do
	{
		audio_buffer* buffer = alloc_buffer();
		
		if ( buffer == NULL )
		{
			ERROR = "bufferCmd: audio buffers exhausted";
			
			return notEnoughBufferSpace;
		}
		
		buffer->ch       = chan;
		buffer->ff.count = playback_rate;
		
		call_cpy( f, buffer->ff.waveBytes, packets, cluster_len );
		
		packets           += cluster_len;
		packets_remaining -= cluster_len;
		
		ParamBlockRec& pb = buffer->pb;
		IOParam&       io = pb.ioParam;
		
		io.ioPosOffset = packets_remaining;
		io.ioReqCount  = 6 + (short) cluster_len * (short) x;
		
		err = PBWriteAsync( &pb );
		
		if ( packets_remaining < cluster_len )
		{
			cluster_len = packets_remaining;
		}
	}
	while ( packets_remaining > 0  &&  err == noErr );
	
	return err;
}
