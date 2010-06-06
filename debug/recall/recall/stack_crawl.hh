/*	==============
 *	stack_crawl.hh
 *	==============
 */

#ifndef RECALL_STACKCRAWL_HH
#define RECALL_STACKCRAWL_HH

// recall
#include "recall/return_address.hh"


namespace recall
{
	
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
	
	unsigned make_stack_crawl( frame_data*          result,
	                           unsigned             capacity,
	                           stack_frame_pointer  top = 0L );  // NULL
	
}

#endif

