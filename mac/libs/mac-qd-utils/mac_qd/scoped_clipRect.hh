/*
	scoped_clipRect.hh
	------------------
*/

#ifndef MACQD_SCOPEDCLIPRECT_HH
#define MACQD_SCOPEDCLIPRECT_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// missing-macos
#if ! __LP64__
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif
#endif

// iota
#include "iota/class.hh"

// mac-qd-utils
#include "mac_qd/globals/thePort.hh"


namespace mac {
namespace qd  {
	
	class scoped_clipRect
	{
		NON_COPYABLE( scoped_clipRect )
		NO_NEW_DELETE
		
		private:
			::RgnHandle saved;
		
		public:
			scoped_clipRect( const Rect& rect, ::RgnHandle tmp );
			~scoped_clipRect();
	};
	
#if ! OPAQUE_TOOLBOX_STRUCTS
	
	inline
	scoped_clipRect::scoped_clipRect( const Rect& rect, ::RgnHandle tmp )
	{
		tmp[0]->rgnSize = sizeof (MacRegion);
		tmp[0]->rgnBBox = rect;
		
		saved = thePort()->clipRgn;
		
		thePort()->clipRgn = tmp;
	}
	
	inline
	scoped_clipRect::~scoped_clipRect()
	{
		thePort()->clipRgn = saved;
	}
	
#else
#if ! __LP64__
	
	inline
	scoped_clipRect::scoped_clipRect( const Rect& rect, ::RgnHandle tmp )
	{
		saved = GetPortClipRegion( thePort(), NewRgn() );
		
		ClipRect( &rect );
	}
	
	inline
	scoped_clipRect::~scoped_clipRect()
	{
		SetPortClipRegion( thePort(), saved );
		
		DisposeRgn( saved );
	}
	
#endif
#endif  // #if ! OPAQUE_TOOLBOX_STRUCTS
	
}
}

#endif
