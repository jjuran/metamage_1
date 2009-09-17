/*	==============
 *	stack_crawl.hh
 *	==============
 */

#ifndef RECALL_STACKCRAWL_HH
#define RECALL_STACKCRAWL_HH

// Standard C++
#include <vector>


namespace recall
{
	
	// The 'native' code type, whatever that is.
	// Most platforms have just one, but Mac OS is special:
	// It's either classic 68K or Mach-O -- never CFM.
	typedef const struct opaque_code_native*  return_address_native;
	
#if defined( __MACOS__ )
	
	typedef const struct opaque_code_cfm*  return_address_cfm;
	
#endif
	
	
#if defined( __MC68K__ )  ||  defined( __MACOS__ )
	
	// 68K is considered native for traditional Mac OS
	// Even Carbon CFM binaries may run on OS 9 and can intermingle with 68K
	typedef return_address_native return_address_68k;
	
#else
	
	typedef const struct opaque_code_alien_68k* return_address_68k;
	
#endif
	
	
#if defined( __MACOS__ )
	
	typedef return_address_cfm return_address_ppc;
	
#elif defined( __POWERPC__ )
	
	typedef return_address_native return_address_ppc;
	
#else
	
	typedef const struct opaque_code_alien_ppc* return_address_ppc;
	
#endif
	
	
#ifdef __i386__
	
	typedef return_address_native return_address_x86;
	
#else
	
	typedef const struct opaque_code_alien_x86* return_address_x86;
	
#endif
	
	
#ifdef __MACH__
	
	typedef return_address_native return_address_mach_o;
	
#else
	
	typedef const struct opaque_code_alien_mach_o* return_address_mach_o;
	
#endif


#if defined( __POWERPC__ ) && !defined( __MACH__ )  ||  defined( __MACOS__ )
	
	typedef return_address_ppc return_address_traceback;
	
#else
	
	typedef const struct opaque_code_alien_traceback* return_address_traceback;
	
#endif
	
	
	typedef const struct opaque_stack_frame* stack_frame_pointer;
	
	struct frame_data
	{
		stack_frame_pointer frame_pointer;
		
		union
		{
			return_address_native  addr_native;
			
		#if defined( __MACOS__ )
			
			return_address_cfm     addr_cfm;
			
		#endif
		};
		
	#if defined( __MACOS__ )
		
		typedef bool cfm_flag_t;
		
	#else
		
		struct cfm_flag_t {};
		
	#endif
		
		cfm_flag_t is_cfm;
		
		frame_data()
		{
		}
		
		frame_data( stack_frame_pointer frame, return_address_native addr )
		:
			frame_pointer( frame ),
			addr_native( addr ),
			is_cfm()
		{
		}
		
	#if defined( __MACOS__ )
		
		frame_data( stack_frame_pointer frame, return_address_cfm addr )
		:
			frame_pointer( frame ),
			addr_cfm( addr ),
			is_cfm( true )
		{
		}
		
	#endif
	};
	
	
	stack_frame_pointer get_stack_frame_pointer( int levels_to_skip = 0 );
	
	std::vector< frame_data > make_stack_crawl_from_top_to_bottom( stack_frame_pointer frame, const void* limit );
	
	inline std::vector< frame_data > make_stack_crawl_from_top( stack_frame_pointer frame )
	{
		return make_stack_crawl_from_top_to_bottom( frame, (const void*) 0xFFFFFFFF );
	}
	
	std::vector< frame_data > make_stack_crawl_to_bottom( const void* limit );
	
	inline std::vector< frame_data > make_stack_crawl()
	{
		return make_stack_crawl_to_bottom( (const void*) 0xFFFFFFFF );
	}
	
}

#endif

