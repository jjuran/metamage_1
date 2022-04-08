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
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif

// mac-qd-utils
#include "mac_qd/globals/thePort.hh"


namespace mac {
namespace qd  {
	
	class scoped_clipRect
	{
		private:
			::RgnHandle saved;
			
			// non-copyable
			scoped_clipRect           ( const scoped_clipRect& );
			scoped_clipRect& operator=( const scoped_clipRect& );
		
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
