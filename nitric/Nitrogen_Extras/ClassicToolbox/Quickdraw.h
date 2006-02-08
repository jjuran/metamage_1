/*	===========
 *	Quickdraw.h
 *	===========
 */

#ifndef CLASSICTOOLBOX_QUICKDRAW_H
#define CLASSICTOOLBOX_QUICKDRAW_H

// Universal Interfaces
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// Nitrogen core
#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif


namespace Nucleus
{
	
#if !OPAQUE_TOOLBOX_STRUCTS
	
	template <> struct Disposer< ::GrafPtr > : public std::unary_function< ::GrafPtr, void >
	{
		void operator()( ::GrafPtr port ) const
		{
			::ClosePort( port );
		}
	};
	
	template <> struct Disposer< ::CGrafPtr > : public std::unary_function< ::CGrafPtr, void >
	{
		void operator()( ::CGrafPtr port ) const
		{
			::CloseCPort( port );
		}
	};
	
#endif
	
}

namespace Nitrogen
{
	
#if CALL_NOT_IN_CARBON
	
	using ::InitGraf;
	
	Nucleus::Owned< ::GrafPtr > OpenPort( ::GrafPtr port );
	
	using ::InitPort;
	
	inline void ClosePort( Nucleus::Owned< ::GrafPtr > )  {}
	
	Nucleus::Owned< ::CGrafPtr > OpenCPort( ::CGrafPtr port );
	
	using ::InitCPort;
	
	inline void CloseCPort( Nucleus::Owned< ::CGrafPtr > )  {}
	
#endif  // CALL_NOT_IN_CARBON
	
}

#endif

