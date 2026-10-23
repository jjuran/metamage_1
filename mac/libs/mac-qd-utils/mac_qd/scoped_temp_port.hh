/*
	scoped_temp_port.hh
	-------------------
*/

#ifndef SCOPEDTEMPPORT_HH
#define SCOPEDTEMPPORT_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// iota
#include "iota/class.hh"


namespace mac {
namespace qd  {
	
#if TARGET_API_MAC_CARBON
	
	class scoped_temp_port
	{
		NON_COPYABLE( scoped_temp_port )
		NO_NEW_DELETE
		
		private:
			CGrafPtr port;
		
		public:
			scoped_temp_port()  { port = CreateNewPort(); }
			
			~scoped_temp_port()  { DisposePort( port ); }
			
			CGrafPtr get()       { return port; }
			operator CGrafPtr()  { return port; }
	};
	
	typedef scoped_temp_port scoped_temp_port_basic;
	typedef scoped_temp_port scoped_temp_port_color;
	
#else
	
	class scoped_temp_port_basic
	{
		NON_COPYABLE( scoped_temp_port_basic )
		NO_NEW_DELETE
		
		private:
			GrafPort port;
		
		public:
			scoped_temp_port_basic()   { OpenPort ( &port ); }
			~scoped_temp_port_basic()  { ClosePort( &port ); }
			
			GrafPtr get()       { return &port; }
			operator GrafPtr()  { return &port; }
	};
	
	class scoped_temp_port_color
	{
		NON_COPYABLE( scoped_temp_port_color )
		NO_NEW_DELETE
		
		private:
			CGrafPort port;
		
		public:
			scoped_temp_port_color()   { OpenCPort ( &port ); }
			~scoped_temp_port_color()  { CloseCPort( &port ); }
			
			CGrafPtr get()       { return &port; }
			operator CGrafPtr()  { return &port; }
	};
	
#endif
	
}
}

#endif
