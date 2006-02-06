// Assert.h

#ifndef NITROGEN_ASSERT_H
#define NITROGEN_ASSERT_H


#ifdef NITROGEN_DEBUG

#define ASSERT( cond )  Nitrogen::Assert( cond, #cond,  __FILE__, __LINE__ )

#else

#define ASSERT( cond )  // nothing

#endif

namespace Nitrogen
{
	
	void Assert( bool condition, const char* text, const char* file, int line );
	
}

#endif

