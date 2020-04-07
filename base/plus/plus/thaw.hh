/*
	thaw.hh
	-------
*/

#ifndef PLUS_THAW_HH
#define PLUS_THAW_HH

// Standard C
#include <stddef.h>

// plus
#include "plus/string.hh"


#define INLINE_UNLESS_BROKEN inline

#ifdef __GNUC__
#if __GNUC__ < 3  ||  __GNUC__ == 3 && __GNUC_MINOR__ < 3
#undef INLINE_UNLESS_BROKEN
#define INLINE_UNLESS_BROKEN  /* empty */
#endif
#endif

namespace plus
{
	
	class thaw_size_error {};
	
	void assign_pod_from_stream( void* pod, size_t size, const char* begin, const char* end );
	
	template < class POD >
	struct thaw_pod
	{
		typedef POD result_type;
		
		static POD apply( const char* begin, const char* end );
	};
	
	template < class POD >
	
	/*
		Inlining this function yields bad codegen in Apple's g++ 3.1, at least.
		It appears that assign_pod_from_stream() works correctly, but `result`
		doesn't get populated.
	*/
	
	INLINE_UNLESS_BROKEN
	POD thaw_pod< POD >::apply( const char* begin, const char* end )
	{
		POD result;
		
		assign_pod_from_stream( &result, sizeof result, begin, end );
		
		return result;
	}
	
	template <>
	struct thaw_pod< bool >
	{
		// A specialization is required, since (sizeof (bool) == 1) isn't.
		
		typedef bool result_type;
		
		static bool apply( const char* begin, const char* end )
		{
			if ( end - begin != 1 )
			{
				throw thaw_size_error();
			}
			
			return *begin;
		}
	};
	
}

#undef INLINE_UNLESS_BROKEN

#endif
