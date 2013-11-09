/*	=====================
 *	Vectoria/Transform.cc
 *	=====================
 */

#include "Vectoria/Transform.hh"


namespace Vectoria
{
	
	namespace CompileTests
	{
		
		static Pitch NegatePitch( const Pitch& pitch )
		{
			return -pitch;
		}
		
	}
	
}

namespace CompileTests
{
	
	static Vectoria::Pitch NegatePitch( const Vectoria::Pitch& pitch )
	{
		return -pitch;
	}
	
	
}

#ifdef __APPLE__

namespace _
{
	
	void dummy()
	{
	}
	
}

#endif

