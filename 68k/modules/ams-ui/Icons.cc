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

// log-of-war
#include "logofwar/report.hh"

// ams-common
#include "QDGlobals.hh"


#pragma exceptions off


enum
{
	Inverse = 2,
};

BitMap IconBitmap : 0x0A0E;
OSErr  ResErr     : 0x0A60;


static inline
void CopyIconBits( Ptr baseAddr, const Rect* r, short mode )
{
	IconBitmap.baseAddr = baseAddr;
	
	GrafPtr port = *get_addrof_thePort();
	
	CopyBits( &IconBitmap, &port->portBits, &IconBitmap.bounds, r, mode, NULL );
}

pascal void PlotIcon_patch( const Rect* rect, char** icon )
{
	IconBitmap.rowBytes = 4;
	
	IconBitmap.bounds.bottom = 32;
	IconBitmap.bounds.right  = 32;
	
	CopyIconBits( *icon, rect, srcCopy );
}

static
pascal OSErr PlotIconID_call( const Rect*        rect,
                              IconAlignmentType  align,
                              IconTransformType  xform,
                              short              resID )
{
	Handle h;
	short px;
	short mask_offset;
	
	const bool small = rect->bottom - rect->top
	                 + rect->right - rect->left < 32 * 2;
	
	if ( small  &&  (h = GetResource( 'ics#', resID )) )
	{
		px = 16;
		
		mask_offset = 32;
	}
	else if ( (h = GetResource( 'ICN#', resID )) )
	{
		px = 32;
		
		mask_offset = 128;
	}
	else
	{
		if ( ResErr != noErr )
		{
			return ResErr;
		}
		
		return resNotFound;
	}
	
	IconBitmap.rowBytes = px / 8u;
	
	IconBitmap.bounds.bottom = px;
	IconBitmap.bounds.right  = px;
	
	const GrafPort& port = **get_addrof_thePort();
	
	/*
		Regarding fore-color and back-color, in a monobit environment
		we have to consider four scenarious:
		
		Black on white:
			This is the norm:  BIC the mask and XOR the face.
		
		White on black:
			Inverse video mode:  OR the mask and XOR the face.
		
		Black on black:
			Unusual, though "black" could actually be one of the six colors.
			But in a basic graphics port, just OR the mask.
		
		White on white:
			Rare if ever, but supported:  Just BIC the mask.
		
		NOTE:  Always pass srcBic, because StdBits() will
		substitute srcOr when there's a black background.
	*/
	
	CopyIconBits( *h + mask_offset, rect, srcBic );
	
	if ( (port.fgColor ^ port.bkColor) & Inverse )
	{
		/*
			Either black on white or white on black,
			so draw the icon's face over the mask.
		*/
		
		CopyIconBits( *h, rect, srcXor );
	}
	else
	{
		/*
			Either black on black or white on white --
			leave the mask drawn as is, as a silhouette.
		*/
	}
	
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
	
	OffsetRgn( rgn, iconRect->left, iconRect->top );
	
	return noErr;
}

static
void unimplemented_call( short selector : __D0 )
{
	FATAL = "unimplemented IconDispatch call ", selector;
	
	asm { ILLEGAL }
}

asm void IconDispatch_patch( short method : __D0 )
{
	CMPI.W   #0x0500,D0
	BEQ      dispatch_PlotIconID
	
	CMPI.W   #0x0613,D0
	BEQ      dispatch_IconIDToRgn
	
	JMP      unimplemented_call
	
dispatch_PlotIconID:
	JMP      PlotIconID_call
	
dispatch_IconIDToRgn:
	JMP      IconIDToRgn_call
}
