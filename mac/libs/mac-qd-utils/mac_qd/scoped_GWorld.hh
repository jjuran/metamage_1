/*
	scoped_GWorld.hh
	----------------
*/

#ifndef MACQD_SCOPEDGWORLD_HH
#define MACQD_SCOPEDGWORLD_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef MAC_OS_X_VERSION_10_7
#ifndef __QDOFFSCREEN__
#include <QDOffscreen.h>
#endif
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QDOFFSCREEN_H
#include "missing/QDOffscreen.h"
#endif
#endif

// iota
#include "iota/class.hh"


namespace mac {
namespace qd  {
	
	class scoped_GWorld
	{
		NON_COPYABLE( scoped_GWorld )
		NO_NEW_DELETE
		
		private:
			GWorldPtr its_saved_port;
			GDHandle  its_saved_gdev;
		
		public:
			scoped_GWorld();
			scoped_GWorld( GWorldPtr gworld, GDHandle device = NULL );
			
			~scoped_GWorld();
	};
	
#if ! __LP64__
	
	inline
	scoped_GWorld::scoped_GWorld()
	{
		GetGWorld( &its_saved_port, &its_saved_gdev );
	}
	
	inline
	scoped_GWorld::scoped_GWorld( GWorldPtr gworld, GDHandle device )
	{
		GetGWorld( &its_saved_port, &its_saved_gdev );
		
		SetGWorld( gworld, device );
	}
	
	inline
	scoped_GWorld::~scoped_GWorld()
	{
		SetGWorld( its_saved_port, its_saved_gdev );
	}
	
#endif  // #if ! __LP64__
	
}
}

#endif
