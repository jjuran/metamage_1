/*
	scoped_port.hh
	--------------
*/

#ifndef MACQD_SCOPEDPORT_HH
#define MACQD_SCOPEDPORT_HH

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif
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


namespace mac {
namespace qd  {
	
	class scoped_port
	{
		NON_COPYABLE( scoped_port )
		NO_NEW_DELETE
		
		private:
			GrafPtr its_saved_port;
		
		public:
			scoped_port();
			scoped_port( CGrafPtr port );
			scoped_port( WindowRef window );
			
			~scoped_port();
	};
	
#if ! __LP64__
	
	inline
	scoped_port::scoped_port()
	{
		GetPort( &its_saved_port );
	}
	
	inline
	scoped_port::scoped_port( CGrafPtr port )
	{
		GetPort( &its_saved_port );
		SetPort( (GrafPtr) port );
	}
	
	inline
	scoped_port::scoped_port( WindowRef window )
	{
		GetPort( &its_saved_port );
		SetPortWindowPort( window );
	}
	
	inline
	scoped_port::~scoped_port()
	{
		SetPort( its_saved_port );
	}
	
#endif  // #if ! __LP64__
	
}
}

#endif
