/*	==============
 *	stack_crawl.cc
 *	==============
 */

#include "recall/stack_crawl.hh"

// recall
#include "recall/stack_frame.hh"


#ifndef NULL
#define NULL  0L
#endif


#if defined( __ELF__ )  ||  defined(__INTERIX)
#define CONFIG_STACK_CRAWLS 0
#else
#define CONFIG_STACK_CRAWLS 1
#endif


namespace recall
{
	
	stack_frame_pointer get_stack_frame_pointer( int levels_to_skip )
	{
	#if CONFIG_STACK_CRAWLS
		
		const stack_frame* frame = get_top_frame();
		
		while ( frame != NULL  &&  --levels_to_skip >= 0 )
		{
			frame = frame->next;  // FIXME:  This needs to perform the usual checks
		}
		
		return reinterpret_cast< stack_frame_pointer >( frame );
		
	#else
		
		return NULL;
		
	#endif
	}
	
	template < class StackFrame > struct switch_frame_traits
	{
		static const bool can_switch = false;
		
		typedef const StackFrame *this_frame_type;
		typedef const StackFrame *next_frame_type;
		
		static next_frame_type check( this_frame_type frame )  { return NULL; }
	};
	
#ifdef __MACOS__
	
	// PPC calls 68K
	template <> struct switch_frame_traits< stack_frame_68k >
	{
		static const bool can_switch = true;
		
		typedef const stack_frame_68k *this_frame_type;
		typedef const stack_frame_ppc *next_frame_type;
		
		static next_frame_type check( this_frame_type frame );
	};
	
	// 68K calls PPC
	template <> struct switch_frame_traits< stack_frame_ppc >
	{
		static const bool can_switch = true;
		
		typedef const stack_frame_ppc *this_frame_type;
		typedef const stack_frame_68k *next_frame_type;
		
		static next_frame_type check( this_frame_type frame );
	};
	
	template <>
	inline const stack_frame_ppc*
	//
	switch_frame_traits< stack_frame_68k >::check( this_frame_type frame )
	{
		const stack_frame_68k* next = frame->next;
		
		if ( next != NULL  &&  ((unsigned long*) next)[-1] == 0xffffffff )
		{
			const void* addr = frame->return_address;
			
			if ( frame < addr  &&  addr < next )
			{
				return (const stack_frame_ppc*) next;
			}
		}
		
		return NULL;
	}
	
	template <>
	inline const stack_frame_68k*
	//
	switch_frame_traits< stack_frame_ppc >::check( this_frame_type frame )
	{
		const long addr = (long) frame;
		
		return addr & 0x00000001 ? (const stack_frame_68k*) (addr - 1)
		                         : NULL;
	}
	
#endif
	
	
	template < class StackFrame >
	static unsigned crawl_stack( const StackFrame*  frame,
	                             frame_data*        result,
	                             unsigned           capacity )
	{
	next:
		
		if ( frame == NULL  ||  capacity == 0 )
		{
			return capacity;
		}
		
		typedef switch_frame_traits< StackFrame > traits;
		
		if ( traits::can_switch )
		{
			if ( typename traits::next_frame_type next = traits::check( frame ) )
			{
				crawl_stack( next, result, capacity );
				
				return capacity;
			}
		}
		
		*result++ = frame_data( (stack_frame_pointer) frame,
		                        frame->return_address );
		
		--capacity;
		
		
		frame = frame->next;
		
		goto next;
	}
	
	unsigned make_stack_crawl( frame_data*          result,
	                           unsigned             capacity,
	                           stack_frame_pointer  top )
	{
	#if CONFIG_STACK_CRAWLS
		
		const stack_frame* top_frame = top ? (const stack_frame*) top
		                                   : get_top_frame();
		
		return capacity - crawl_stack( top_frame, result, capacity );
		
	#endif
		
		return 0;
	}
	
}
