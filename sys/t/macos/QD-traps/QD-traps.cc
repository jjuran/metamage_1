/*
	QD-traps.cc
	-----------
*/

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif
#ifndef __TRAPS__
#include <Traps.h>
#endif

// POSIX
#include <unistd.h>

// Standard C
#include <stdlib.h>
#include <string.h>

// mac-sys-utils
#include "mac_sys/trap_address.hh"

// tap-out
#include "tap/test.hh"


#pragma exceptions off


static const unsigned n_tests = 0;


Boolean QDExist : 0x08F3;

GrafPtr WMgrPort : 0x09DE;

enum
{
	trace_none,
	
	#define X(name)  trace_##name,
	
	#include "QD-traps.h"
	#undef X
};

static const char* trace_names[] =
{
	"-",
	
	#define X(name)  #name,
	
	#include "QD-traps.h"
	#undef X
};


#define X(name)  static UniversalProcPtr old_##name;

#include "QD-traps.h"
#undef X

static QDGlobals qd;

static GrafPtr testing_grafPort;
static RgnHandle tmp_rgn;

static Byte trace_log[ 1000 ];

static int mark;

static bool tracing;
static bool custom_procs;

static
void trace_on()
{
	mark = 0;
	
	memset( trace_log, '\0', sizeof trace_log );
	
	tracing = true;
}

static inline
void trace_off()
{
	tracing = false;
}

static
void trace( Byte c : __D0 )
{
	if ( tracing  &&  mark < sizeof trace_log )
	{
		trace_log[ mark++ ] = c;
	}
}

#define X( name )    \
static asm void my_##name()  \
{  \
	LINK     A6,#0            ;  \
	MOVE.B   #trace_##name,D0 ;  \
	JSR      trace            ;  \
	UNLK     A6               ;  \
	MOVEA.L  old_##name,A0    ;  \
	JMP      (A0)             ;  \
}

#include "QD-traps.h"
#undef X

enum
{
	port_width  = 64,
	port_height = 64,
	
	port_px = port_width * port_height,
	port_bytes = port_px / 8,
	
	port_rowBytes = (port_width + 15) / 16 * 2,
};

static QDProcs qdProcs;

static
void init()
{
	if ( QDExist != 0 )
	{
		write( STDOUT_FILENO, STR_LEN( "### Initializing QuickDraw\n" ) );
		
		InitResources();
		
		InitGraf( &qd.thePort );
		InitFonts();
	}
	
	tmp_rgn = NewRgn();
	
	testing_grafPort = (GrafPtr) malloc( sizeof (GrafPort) );
	
	OpenPort( testing_grafPort );
	
	PortSize( port_width, port_height );
	
	char* mem = (char*) malloc( port_bytes );
	
	BitMap bitmap = { mem, port_rowBytes, { 0, 0, port_height, port_width } };
	
	SetPortBits( &bitmap );
	
	if ( custom_procs )
	{
		SetStdProcs( &qdProcs );
		
		testing_grafPort->grafProcs = &qdProcs;
	}
	
	using mac::sys::get_trap_address;
	using mac::sys::set_trap_address;
	
	#define X( name )  old_##name = get_trap_address( _##name );
	
	#include "QD-traps.h"
	#undef X
	
	#define X( name )  set_trap_address( (ProcPtr) my_##name, _##name );
	
	#include "QD-traps.h"
	#undef X
}

static
void term()
{
	using mac::sys::set_trap_address;
	
	#define X( name )  set_trap_address( (ProcPtr) old_##name, _##name );
	
	#include "QD-traps.h"
	#undef X
	
	SetPort( NULL );
	
	if ( testing_grafPort != NULL )
	{
		free( testing_grafPort->portBits.baseAddr );
		
		ClosePort( testing_grafPort );
		
		free( testing_grafPort );
	}
	
	DisposeRgn( tmp_rgn );
}

static
void report()
{
	for ( int i = 0;  i < mark;  ++i )
	{
		Byte c = trace_log[ i ];
		
		const char* name = trace_names[ c ];
		
		write( STDERR_FILENO, name, strlen( name ) );
		write( STDERR_FILENO, STR_LEN( "\n" ) );
	}
}

static Pattern pattern;

static
void rects()
{
	EraseRect ( &testing_grafPort->portRect );
	PaintRect ( &testing_grafPort->portRect );
	FrameRect ( &testing_grafPort->portRect );
	InvertRect( &testing_grafPort->portRect );
	FillRect  ( &testing_grafPort->portRect, &pattern );
	
	++mark;
}

static
void rrects()
{
	EraseRoundRect ( &testing_grafPort->portRect, 16, 16 );
	PaintRoundRect ( &testing_grafPort->portRect, 16, 16 );
	FrameRoundRect ( &testing_grafPort->portRect, 16, 16 );
	InvertRoundRect( &testing_grafPort->portRect, 16, 16 );
	FillRoundRect  ( &testing_grafPort->portRect, 16, 16, &pattern );
	
	++mark;
}

static
void ovals()
{
	EraseOval ( &testing_grafPort->portRect );
	PaintOval ( &testing_grafPort->portRect );
	FrameOval ( &testing_grafPort->portRect );
	InvertOval( &testing_grafPort->portRect );
	FillOval  ( &testing_grafPort->portRect, &pattern );
	
	++mark;
}

static
void arcs()
{
	EraseArc ( &testing_grafPort->portRect, 90, 180 );
	PaintArc ( &testing_grafPort->portRect, 90, 180 );
	FrameArc ( &testing_grafPort->portRect, 90, 180 );
	InvertArc( &testing_grafPort->portRect, 90, 180 );
	FillArc  ( &testing_grafPort->portRect, 90, 180, &pattern );
	
	++mark;
}

static
void poly()
{
	PolyHandle polygon = OpenPoly();
	
	MoveTo( 0, 0 );
	LineTo( 8, 8 );
	Line( -1, -4 );
	Line( -2, -1 );
	
	ClosePoly();
	
	ErasePoly ( polygon );
	PaintPoly ( polygon );
	FramePoly ( polygon );
	InvertPoly( polygon );
	FillPoly  ( polygon, &pattern );
	
	KillPoly( polygon );
	
	++mark;
}

static
void regions()
{
	EraseRgn ( testing_grafPort->visRgn );
	PaintRgn ( testing_grafPort->visRgn );
	FrameRgn ( testing_grafPort->visRgn );
	InvertRgn( testing_grafPort->visRgn );
	FillRgn  ( testing_grafPort->visRgn, &pattern );
	
	++mark;
}

static
void lines()
{
	MoveTo( 0, 0 );
	LineTo( 8, 8 );
	Move( 3, 4 );
	Line( 2, 1 );
	
	++mark;
}

static
void bits()
{
	CopyBits( &testing_grafPort->portBits,
	          &testing_grafPort->portBits,
	          &testing_grafPort->portRect,
	          &testing_grafPort->portRect,
	          srcCopy,
	          NULL );
	
	ScrollRect( &testing_grafPort->portRect, 1, 1, tmp_rgn );
	
	++mark;
}

int main( int argc, char** argv )
{
	if ( argc > 1  &&  strcmp( argv[ 1 ], "--custom" ) == 0 )
	{
		custom_procs = true;
	}
	
	tap::start( "QD-traps", n_tests );
	
	init();
	
	trace_on();
	
	rects();
	rrects();
	ovals();
	arcs();
	poly();
	regions();
	lines();
	bits();
	
	trace_off();
	
	term();
	
	report();
	
	return 0;
}
