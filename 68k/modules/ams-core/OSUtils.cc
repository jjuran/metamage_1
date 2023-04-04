/*
	OSUtils.cc
	----------
*/

#include "OSUtils.hh"

// Mac OS
#ifndef __MACERRORS__
#include <MacErrors.h>
#endif
#ifndef __OSUTILS__
#include <OSUtils.h>
#endif
#ifndef __TIMER__
#include <Timer.h>
#endif

// POSIX
#include <sys/time.h>
#include <unistd.h>

// iota
#include "iota/char_types.hh"

// log-of-war
#include "logofwar/report.hh"

// ams-common
#include "callouts.hh"
#include "time.hh"

// ams-core
#include "reactor-core.hh"


#pragma exceptions off


#define STR_LEN( s )  "" s, (sizeof s - 1)


UInt8  CPUFlag    : 0x012F;
UInt16 SysVersion : 0x015A;
UInt32 Ticks      : 0x016A;
UInt32 Time       : 0x020C;
SInt16 BootDrive  : 0x0210;


static inline
UInt64 Microseconds()
{
	UInt64 result;
	
	Microseconds( (UnsignedWide*) &result );
	
	return result;
}


#pragma mark -
#pragma mark String Comparison
#pragma mark -

static
bool case_insensitive_equal( const Byte* a, const Byte* b, UInt16 len )
{
	while ( len-- > 0 )
	{
		if ( iota::to_lower( *a++ ) != iota::to_lower( *b++ ) )
		{
			return false;
		}
	}
	
	return true;
}

static
int case_insensitive_order( const Byte* a, const Byte* b, UInt16 len )
{
	while ( len-- > 0 )
	{
		Byte c = iota::to_lower( *a++ );
		Byte d = iota::to_lower( *b++ );
		
		if ( c != d )
		{
			return (c > d) - (c < d);
		}
	}
	
	return 0;
}

unsigned long CmpString_patch( const unsigned char* a : __A0,
                               const unsigned char* b : __A1,
                               unsigned long        m : __D0,
                               unsigned short       A : __D1 )
{
	enum
	{
		MARKS = 0x0200,  // diacritics-insensitive
		CASE  = 0x0400,  // case-sensitive
	};
	
	if ( m == 0 )
	{
		return 0;  // Empty strings are equal
	}
	
	UInt16 len = m;
	
	if ( (m >> 16) != len )
	{
		return 1;  // Strings of unequal length are unequal
	}
	
	if ( A & MARKS )
	{
		NOTICE = "CmpString: ignoring MARKS bit";
	}
	
	if ( A & CASE )
	{
		return fast_memcmp( a, b, len ) != 0;
	}
	
	return ! case_insensitive_equal( a, b, len );
}

long RelString_patch( const unsigned char* a : __A0,
                      const unsigned char* b : __A1,
                      unsigned long        m : __D0,
                      unsigned short       A : __D1 )
{
	enum
	{
		MARKS = 0x0200,  // diacritics-insensitive
		CASE  = 0x0400,  // case-sensitive
	};
	
	if ( m == 0 )
	{
		return 0;  // Empty strings are equal
	}
	
	const UInt16 a_len = m >> 16;
	const UInt16 b_len = m;
	
	const UInt16 min_len = b_len < a_len ? b_len : a_len;
	
	const int result_by_length = (b_len < a_len) - (a_len < b_len);
	
	if ( A & MARKS )
	{
		WARNING = "RelString: ignoring MARKS bit";
	}
	
	if ( A & CASE )
	{
		if ( int cmp = fast_memcmp( a, b, min_len ) )
		{
			return cmp;
		}
		
		return result_by_length;
	}
	
	if ( int cmp = case_insensitive_order( a, b, min_len ) )
	{
		return cmp;
	}
	
	return result_by_length;
}

unsigned char* UprString_patch( unsigned char* s : __A0,
                                short len        : __D0,
                                short trap_word  : __D1 )
{
	enum
	{
		MARKS = 0x0200,  // diacritics-stripping
	};
	
	bool contains_high_chars = false;
	
	for ( int i = 0;  i < len;  ++i )
	{
		const signed char c = s[ i ];
		
		if ( c < 0 )
		{
			contains_high_chars = true;
		}
		
		s[ i ] = iota::to_upper( c );
	}
	
	// TODO:  Implement mark stripping and marked letter upcasing.
	
	if ( contains_high_chars )
	{
		WARNING = "UprString: leaving marks as is";
	}
	
	return s;
}

#pragma mark -
#pragma mark Date and Time Operations
#pragma mark -

short ReadDateTime_patch( long* secs : __A0 )
{
	*secs = Time;
	
	return noErr;
}

static const char month_days[] =
{
	31, 28, 31,
	30, 31, 30,
	31, 31, 30,
	31, 30, 31,
};

unsigned long Date2Secs_patch( const DateTimeRec* date : __A0 )
{
	const unsigned long days_per_quad = 365 * 4 + 1;
	
	int years = date->year - 1904;
	int quads = years / 4u;
	
	years %= 4u;
	
	int days  = date->day   - 1 + days_per_quad * quads;
	int month = date->month - 1;
	
	if ( years == 0  &&  month > 1 )
	{
		// A leap year, in a month after February
		
		++days;
	}
	
	while ( month-- > 0 )
	{
		days += month_days[ month ];
	}
	
	unsigned long hours = 24 * days  + date->hour;
	unsigned long mins  = 60 * hours + date->minute;
	unsigned long secs  = 60 * mins  + date->second;
	
	return secs;
}

DateTimeRec* Secs2Date_patch( unsigned long  secs : __D0,
                              DateTimeRec*   date : __A0 )
{
	const unsigned long days_per_quad = 365 * 4 + 1;
	
	unsigned long mins, hour, days;
	
	date->second = secs % 60;
	mins         = secs / 60;
	date->minute = mins % 60;
	hour         = mins / 60;
	date->hour   = hour % 24;
	days         = hour / 24;
	
	date->dayOfWeek = (days + 5) % 7 + 1;  // 1904-01-01 is a Friday
	
	const unsigned short quads = days / days_per_quad;
	days                       = days % days_per_quad;
	
	const unsigned short feb29 = 31 + 28;
	
	const short leap_diff = days - feb29;
	
	days -= leap_diff > 0;
	
	unsigned short years = quads * 4;
	
	years += days / 365;
	days   = days % 365;
	
	unsigned short month = 1;
	
	for ( int i = 0;  i < 12;  ++i )
	{
		if ( days < month_days[ i ] )
		{
			break;
		}
		
		days -= month_days[ i ];
		
		++month;
	}
	
	days += 1 + (leap_diff == 0);
	
	date->year  = 1904 + years;
	date->month = month;
	date->day   = days;
	
	return date;
}

#pragma mark -
#pragma mark Parameter RAM Operations
#pragma mark -

short InitUtil_patch()
{
	return prInitErr;
}

OSErr WriteParam_patch()
{
	return noErr;
}

#pragma mark -
#pragma mark Queue Manipulation
#pragma mark -

asm
void Enqueue_patch( QElem* qEntry : __A0, QHdr* queue : __A1 )
{
	CLR.L    (A0)     // qEntry->qLink = NULL;
	
	JSR      0xFFFFFFFA  // enter_supervisor_mode()
	ORI      #0x0700,SR  // mask all interrupts (except NMI)
	ANDI     #0xDFFF,SR  // exit supervisor mode
	
	// old SR (with old interrupt mask) is in D0
	
	MOVE.L   A0,D2    // Move qEntry to D2 (freeing up A0 for next pointer)
	ADDQ.L   #2,A1    // Point to qHead field of queue
	
	MOVEA.L  A1,A0    // next = &queue->qHead;
	TST.L    (A1)+    // if (queue->qHead)  // advances A1 to &queue->qTail
	BEQ.S    set_link
	
	MOVE.L   (A1),A0  // next = &queue->qTail->qLink;
	
set_link:
	
	MOVE.L   D2,(A0)  // *next        = qEntry;
	MOVE.L   D2,(A1)  // queue->qTail = qEntry;
	
	MOVE.L   D0,D1       // move old SR to D1
	JSR      0xFFFFFFFA  // enter_supervisor_mode()
	MOVE     D1,SR       // restore old SR (with old interrupt mask)
	
	RTS
}

asm
short Dequeue_patch( QElem* qEntry : __A0, QHdr* queue : __A1 )
{
	MOVEQ.L  #qErr,D0
	MOVE.L   A0,D2       // Null-check qEntry (and move to D2 for later)
	BEQ.S    bail
	
	CLR.W    -(SP)       // noErr
	
	ADDQ.L   #2,A1       // Point to qHead field of queue
	MOVE.L   A1,D1       // Copy &queue->qHead to D1
	
	JSR      0xFFFFFFFA  // enter_supervisor_mode()
	ORI      #0x0700,SR  // mask all interrupts (except NMI)
	ANDI     #0xDFFF,SR  // exit supervisor mode
	
	// old SR (with old interrupt mask) is in D0
	
	CMPA.L   (A1)+,A0    // if ( qEntry == queue->qHead )
	BNE.S    loop
	
	CMP.L    (A1),A0     // if ( qEntry == queue->qTail )
	BNE.S    first_but_not_last
	
	CLR.L    (A1)        // queue->qTail = NULL;
	
first_but_not_last:
	
	MOVE.L   (A0),-(A1)  // queue->qHead = qEntry->qLink;
	
	BRA.S    cleanup
	
not_found:
	
	SUBQ.W   #1,(SP)     // qErr: "Entry not in specified queue"
	BRA.S    cleanup
	
loop:
	
	MOVEA.L  D1,A0       // prev = next;         // or &queue->qHead, once
	MOVE.L   (A0),D1     // next = next->qLink;  // or queue->qHead, once
	
	BEQ.S    not_found   // while ( next != NULL )
	
	CMP.L    D2,D1       // if ( next == qEntry )
	BNE.S    loop
	
	CMP.L    (A1),D2     // if ( qEntry == queue->qTail )
	BNE.S    not_last
	
	MOVE.L   A0,(A1)     // queue->qTail = prev;
	
not_last:
	
	MOVEA.L  D2,A1
	MOVE.L   (A1),(A0)   // prev->qLink = qEntry->qLink;
	
cleanup:
	
	MOVE.L   D0,D1       // move old SR to D1
	JSR      0xFFFFFFFA  // enter_supervisor_mode()
	MOVE     D1,SR       // restore old SR (with old interrupt mask)
	
	MOVE.W   (SP)+,D0
	
bail:
	RTS
}

#pragma mark -
#pragma mark Miscellaneous Utilities
#pragma mark -

static inline
bool reactor_wait( UInt64 dt )
{
	timeval timeout;
	
	timeval_from_microseconds( &timeout, &dt );
	
	return reactor_wait( &timeout );
}

/*
	Empirical testing (sampling Microseconds() before and after
	calling Delay(1)) shows that Delay(N) doesn't wait for N whole
	ticks to elapse, but rather waits until Ticks changes N times.
*/

long Delay_patch( long numTicks : __A0, short trap_word : __D1 )
{
	const UInt64 start = Ticks * tick_microseconds;
	
	SInt64 dt = (UInt32) numTicks * tick_microseconds;
	
	const UInt64 end_time = start + dt;
	
	// If numTicks is negative, return after one reactor-wait.
	
	do
	{
		reactor_wait( dt );
		
		dt = end_time - Microseconds();
	}
	while ( dt > 0  &&  numTicks >= 0 );
	
	return Ticks;
}

pascal void SysBeep_patch( short duration )
{
	char c = 0x07;
	
	write( STDOUT_FILENO, &c, sizeof c );
}

short SysEnvirons_patch( short v : __D0, SysEnvRec* env : __A0 )
{
	/*
		Default to a Mac Plus for now, which is the minimum for Glider 3.0.
		A program that refuses to run because a Mac Plus is too old wouldn't
		have run anyway, and a program so old it doesn't work on a Mac Plus
		probably wasn't calling SysEnvirons() in the first place.
	*/
	
	const short maxSysEnvVers = curSysEnvVers;  // version 2
	
	env->environsVersion = maxSysEnvVers;
	env->machineType     = envMacPlus;
	env->systemVersion   = SysVersion;
	env->processor       = CPUFlag + 1;
	env->hasFPU          = false;
	env->hasColorQD      = false;
	env->keyBoardType    = envUnknownKbd;  // Mac Plus keyboard with keypad
	env->atDrvrVersNum   = 0;
	env->sysVRefNum      = BootDrive;
	
	if ( v > maxSysEnvVers )
	{
		WARNING = "SysEnvirons version ", v, " too big";
	}
	
	return v > maxSysEnvVers ? envVersTooBig : noErr;
}
