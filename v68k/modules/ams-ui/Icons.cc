/*
	Icons.cc
	--------
*/

#include "Icons.hh"

// Mac OS
#ifndef __ICONS__
#include <Icons.h>
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

// ams-common
#include "QDGlobals.hh"


BitMap IconBitmap : 0x0A0E;
OSErr  ResErr     : 0x0A60;


pascal char** GetIcon_patch( short iconID )
{
	return GetResource( 'ICON', iconID );
}

static inline
void CopyIconBits( Ptr baseAddr, const Rect* r, short mode )
{
	IconBitmap.baseAddr = baseAddr;
	
	GrafPtr port = *get_addrof_thePort();
	
	CopyBits( &IconBitmap, &port->portBits, &IconBitmap.bounds, r, mode, NULL );
}

pascal void PlotIcon_patch( const Rect* rect, char** icon )
{
	CopyIconBits( *icon, rect, srcCopy );
}

static
pascal OSErr PlotIconID_call( const Rect*        rect,
                              IconAlignmentType  align,
                              IconTransformType  xform,
                              short              resID )
{
	Handle h = GetResource( 'ICN#', resID );
	
	if ( h == NULL )
	{
		if ( ResErr != noErr )
		{
			return ResErr;
		}
		
		return resNotFound;
	}
	
	CopyIconBits( *h + 128, rect, srcBic );
	CopyIconBits( *h,       rect, srcXor );
	
	ReleaseResource( h );
	
	return noErr;
}

static
pascal OSErr IconIDToRgn_call( RgnHandle          rgn,
                               const Rect*        iconRect,
                               IconAlignmentType  align,
                               short              resID )
{
	Handle h = GetResource( 'ICN#', resID );
	
	if ( h == NULL )
	{
		if ( ResErr != noErr )
		{
			return ResErr;
		}
		
		return resNotFound;
	}
	
	IconBitmap.baseAddr = *h + 128;
	
	BitMapToRegion( rgn, &IconBitmap );
	
	ReleaseResource( h );
	
	OffsetRgn( rgn, iconRect->left, iconRect->top );
	
	return noErr;
}

asm void IconDispatch_patch( short method : __D0 )
{
	CMPI.W   #0x0500,D0
	BEQ      dispatch_PlotIconID
	
	CMPI.W   #0x0613,D0
	BEQ      dispatch_IconIDToRgn
	
	_Debugger
	_ExitToShell
	
dispatch_PlotIconID:
	JMP      PlotIconID_call
	
dispatch_IconIDToRgn:
	JMP      IconIDToRgn_call
}
