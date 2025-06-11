/*
	ams-qd.cc
	---------
*/

// Mac OS
#ifndef __TRAPS__
#include <Traps.h>
#endif

// POSIX
#include <unistd.h>

// Standard C
#include <stdlib.h>

// ams-common
#include "callouts.hh"

// ams-core
#include "Arcs.hh"
#include "CopyBits.hh"
#include "CopyMask.hh"
#include "Cursor.hh"
#include "Fixed.hh"
#include "Fonts.hh"
#include "GrafPorts.hh"
#include "InitGraf.hh"
#include "OpenPoly.hh"
#include "Ovals.hh"
#include "QDUtils.hh"
#include "Pen.hh"
#include "Pictures.hh"
#include "Points.hh"
#include "Polygons.hh"
#include "Rects.hh"
#include "Region-ops.hh"
#include "Regions.hh"
#include "RoundRects.hh"
#include "StdBits.hh"
#include "StdProcs.hh"
#include "Text.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define PROGRAM  "ams-qd"

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )


const int tb_traps = 3 * 1024;

struct trap_patch
{
	long  slot;
	void* addr;
};

static
void install( const trap_patch* patches, int n )
{
	for ( int i = 0;  i < n;  ++i )
	{
		const trap_patch& patch = patches[ i ];
		
		*(void**) patch.slot = patch.addr;
	}
}

#define COUNT(array)  (sizeof (array) / sizeof *(array))

#define TBTRAP( Proc )  { tb_traps + 4 * (_##Proc & 0x03FF), &Proc##_patch }

#define INSTALL( patches )  install( patches, COUNT( patches ) )


enum
{
	_SetPortBits = _SetPBits,
	_SetRectRgn  = _SetRecRgn,
};


static const trap_patch QuickDraw[] =
{
	TBTRAP( CopyMask ),  // A817
	
	TBTRAP( SetFScaleDisable ),  // A834
	
	TBTRAP( MeasureText ),  // A837
	
	TBTRAP( FixDiv   ),  // A84D
	
	TBTRAP( InitCursor ),  // A850
	TBTRAP( SetCursor  ),  // A851
	TBTRAP( HideCursor ),  // A852
	TBTRAP( ShowCursor ),  // A853
	
	TBTRAP( ShieldCursor  ),  // A855
	TBTRAP( ObscureCursor ),  // A856
	
	TBTRAP( BitAnd ),  // A858
	
	TBTRAP( BitNot ),  // A85A
	
	TBTRAP( BitShift ),  // A85C
	TBTRAP( BitTst ),  // A85D
	TBTRAP( BitSet ),  // A85E
	TBTRAP( BitClr ),  // A85F
	
	TBTRAP( Random ),  // A861
	
	TBTRAP( ForeColor ),  // A862
	TBTRAP( BackColor ),  // A863
	
	TBTRAP( GetPixel ),  // A865
	TBTRAP( StuffHex ),  // A866
	TBTRAP( LongMul  ),  // A867
	TBTRAP( FixMul   ),  // A868
	TBTRAP( FixRatio ),  // A869
	TBTRAP( HiWord   ),  // A86A
	TBTRAP( LoWord   ),  // A86B
	TBTRAP( FixRound ),  // A86C
	
	TBTRAP( InitPort    ),  // A86D
	TBTRAP( InitGraf    ),  // A86E
	TBTRAP( OpenPort    ),  // A86F
	
	TBTRAP( LocalToGlobal ),  // A870
	TBTRAP( GlobalToLocal ),  // A871
	
	TBTRAP( SetPort     ),  // A873
	TBTRAP( GetPort     ),  // A874
	TBTRAP( SetPortBits ),  // A875
	TBTRAP( PortSize    ),  // A876
	TBTRAP( MovePortTo  ),  // A877
	TBTRAP( SetOrigin   ),  // A878
	TBTRAP( SetClip     ),  // A879
	TBTRAP( GetClip     ),  // A87A
	TBTRAP( ClipRect    ),  // A87B
	TBTRAP( BackPat     ),  // A87C
	TBTRAP( ClosePort   ),  // A87D
	TBTRAP( AddPt       ),  // A87E
	TBTRAP( SubPt       ),  // A87F
	TBTRAP( SetPt       ),  // A880
	TBTRAP( EqualPt     ),  // A881
	TBTRAP( StdText     ),  // A882
	TBTRAP( DrawChar    ),  // A883
	TBTRAP( DrawString  ),  // A884
	TBTRAP( DrawText    ),  // A885
	TBTRAP( TextWidth   ),  // A886
	TBTRAP( TextFont    ),  // A887
	TBTRAP( TextFace    ),  // A888
	TBTRAP( TextMode    ),  // A889
	TBTRAP( TextSize    ),  // A88A
	TBTRAP( GetFontInfo ),  // A88B
	TBTRAP( StringWidth ),  // A88C
	TBTRAP( CharWidth   ),  // A88D
	TBTRAP( SpaceExtra  ),  // A88E
	
	TBTRAP( StdLine ),  // A890
	TBTRAP( LineTo  ),  // A891
	TBTRAP( Line    ),  // A892
	TBTRAP( MoveTo  ),  // A893
	TBTRAP( Move    ),  // A894
	
	TBTRAP( HidePen     ),  // A896
	TBTRAP( ShowPen     ),  // A897
	TBTRAP( GetPenState ),  // A898
	TBTRAP( SetPenState ),  // A899
	TBTRAP( GetPen      ),  // A89A
	TBTRAP( PenSize     ),  // A89B
	TBTRAP( PenMode     ),  // A89C
	TBTRAP( PenPat      ),  // A89D
	TBTRAP( PenNormal   ),  // A89E
	
	TBTRAP( StdRect    ),  // A8A0
	TBTRAP( FrameRect  ),  // A8A1
	TBTRAP( PaintRect  ),  // A8A2
	TBTRAP( EraseRect  ),  // A8A3
	TBTRAP( InverRect  ),  // A8A4
	TBTRAP( FillRect   ),  // A8A5
	TBTRAP( EqualRect  ),  // A8A6
	TBTRAP( SetRect    ),  // A8A7
	TBTRAP( OffsetRect ),  // A8A8
	TBTRAP( InsetRect  ),  // A8A9
	TBTRAP( SectRect   ),  // A8AA
	TBTRAP( UnionRect  ),  // A8AB
	TBTRAP( Pt2Rect    ),  // A8AC
	TBTRAP( PtInRect   ),  // A8AD
	TBTRAP( EmptyRect  ),  // A8AE
	
	TBTRAP( StdRRect       ),  // A8AF
	TBTRAP( FrameRoundRect ),  // A8B0
	TBTRAP( PaintRoundRect ),  // A8B1
	TBTRAP( EraseRoundRect ),  // A8B2
	TBTRAP( InverRoundRect ),  // A8B3
	TBTRAP( FillRoundRect  ),  // A8B4
	
	TBTRAP( StdOval    ),  // A8B6
	TBTRAP( FrameOval  ),  // A8B7
	TBTRAP( PaintOval  ),  // A8B8
	TBTRAP( EraseOval  ),  // A8B9
	TBTRAP( InvertOval ),  // A8BA
	TBTRAP( FillOval   ),  // A8BB
	
	TBTRAP( SlopeFromAngle ),  // A8BC
	
	TBTRAP( StdArc     ),  // A8BD
	TBTRAP( FrameArc   ),  // A8BE
	TBTRAP( PaintArc   ),  // A8BF
	TBTRAP( EraseArc   ),  // A8C0
	TBTRAP( InvertArc  ),  // A8C1
	TBTRAP( FillArc    ),  // A8C2
	
	TBTRAP( PtToAngle      ),  // A8C3
	TBTRAP( AngleFromSlope ),  // A8C4
	
	TBTRAP( StdPoly    ),  // A8C5
	TBTRAP( FramePoly  ),  // A8C6
	TBTRAP( PaintPoly  ),  // A8C7
	TBTRAP( ErasePoly  ),  // A8C8
	TBTRAP( InvertPoly ),  // A8C9
	TBTRAP( FillPoly   ),  // A8CA
	TBTRAP( OpenPoly   ),  // A8CB
	TBTRAP( ClosePoly  ),  // A8CC
	TBTRAP( KillPoly   ),  // A8CD
	TBTRAP( OffsetPoly ),  // A8CE
	
	TBTRAP( PackBits   ),  // A8CF
	TBTRAP( UnpackBits ),  // A8D0
	
	TBTRAP( StdRgn      ),  // A8D1
	TBTRAP( FrameRgn    ),  // A8D2
	TBTRAP( PaintRgn    ),  // A8D3
	TBTRAP( EraseRgn    ),  // A8D4
	TBTRAP( InverRgn    ),  // A8D5
	TBTRAP( FillRgn     ),  // A8D6
	
	TBTRAP( BitMapToRegion ),  // A8D7
	
	TBTRAP( NewRgn      ),  // A8D8
	TBTRAP( DisposeRgn  ),  // A8D9
	TBTRAP( OpenRgn     ),  // A8DA
	TBTRAP( CloseRgn    ),  // A8DB
	TBTRAP( CopyRgn     ),  // A8DC
	TBTRAP( SetEmptyRgn ),  // A8DD
	TBTRAP( SetRectRgn  ),  // A8DE
	TBTRAP( RectRgn     ),  // A8DF
	TBTRAP( OffsetRgn   ),  // A8E0
	TBTRAP( InsetRgn    ),  // A8E1
	TBTRAP( EmptyRgn    ),  // A8E2
	TBTRAP( EqualRgn    ),  // A8E3
	TBTRAP( SectRgn     ),  // A8E4
	TBTRAP( UnionRgn    ),  // A8E5
	TBTRAP( DiffRgn     ),  // A8E6
	TBTRAP( XOrRgn      ),  // A8E7
	TBTRAP( PtInRgn     ),  // A8E8
	TBTRAP( RectInRgn   ),  // A8E9
	TBTRAP( SetStdProcs ),  // A8EA
	TBTRAP( StdBits     ),  // A8EB
	TBTRAP( CopyBits    ),  // A8EC
	TBTRAP( StdTxMeas   ),  // A8ED
	
	TBTRAP( ScrollRect  ),  // A8EF
	
	TBTRAP( PicComment   ),  // A8F2
	TBTRAP( OpenPicture  ),  // A8F3
	TBTRAP( ClosePicture ),  // A8F4
	TBTRAP( KillPicture  ),  // A8F5
	TBTRAP( DrawPicture  ),  // A8F6
	
	TBTRAP( MapPt       ),  // A8F9
	TBTRAP( MapRect     ),  // A8FA
	TBTRAP( MapRgn      ),  // A8FB
	
	TBTRAP( PinRect     ),  // A94E
	TBTRAP( DeltaPoint  ),  // A94F
};

static const trap_patch Fonts[] =
{
	TBTRAP( InitFonts ),  // A8FE
	
	TBTRAP( GetFName   ),  // A8FF
	TBTRAP( GetFNum    ),  // A900
	TBTRAP( FMSwapFont ),  // A901
	TBTRAP( RealFont   ),  // A902
	
	TBTRAP( SetFontLock ),  // A903
};

int main( int argc, char** argv )
{
	if ( argc > 1 )
	{
		WARN( "no arguments allowed" );
		
		_exit( 1 );
	}
	
	INSTALL( QuickDraw );
	INSTALL( Fonts );
	
	module_suspend();  // doesn't return
}
