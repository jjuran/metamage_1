/*
	Mac/Threads/Types/ThreadID.hh
	-----------------------------
*/

#ifndef MAC_THREADS_TYPES_THREADID_HH
#define MAC_THREADS_TYPES_THREADID_HH

// Mac OS
#ifndef __THREADS__
#include <Threads.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif
#ifndef NUCLEUS_DISPOSER_HH
#include "nucleus/disposer.hh"
#endif


namespace Mac
{
	
	enum ThreadID
	{
		kNoThreadID          = ::kNoThreadID,
		kCurrentThreadID     = ::kCurrentThreadID,
		kApplicationThreadID = ::kApplicationThreadID,
		
		kThreadID_Max = nucleus::enumeration_traits< ::ThreadID >::max
	};
	
}

namespace nucleus
{
	
	template <> struct disposer< Mac::ThreadID >
	{
		typedef Mac::ThreadID  argument_type;
		typedef void           result_type;
		
		void operator()( Mac::ThreadID thread ) const
		{
			::DisposeThread( thread, NULL, false );
		}
	};
	
}

#endif

