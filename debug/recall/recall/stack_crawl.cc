/*	==============
 *	stack_crawl.cc
 *	==============
 */

#include "recall/stack_crawl.hh"


namespace recall
{
	
	struct stack_frame_68k
	{
		stack_frame_68k*    next;
		return_address_68k  return_address;
	};
	
	struct stack_frame_ppc
	{
		stack_frame_ppc*    next;
		const void*         saved_CR;
		return_address_ppc  return_address;
	};
	
	struct stack_frame_x86
	{
		stack_frame_x86*       next;
		return_address_native  return_address;
	};
	
	
#ifdef __MC68K__
	
	typedef stack_frame_68k stack_frame;
	
	#pragma parameter __D0 get_A6
	
	static inline char* get_A6() = { 0x200e };
	
	static inline const stack_frame_68k* get_top_frame()
	{
		return (const stack_frame_68k*) get_A6();
	}
	
#endif

#ifdef __POWERPC__
	
	typedef stack_frame_ppc stack_frame;
	
	#ifdef __MWERKS__
		
		static asm char *get_r1( void )
		{
			mr		r3,r1
			blr
		}
		
	#endif
	
	#ifdef __GNUC__
		
		static char *get_r1( void )
		{
			__asm__( "mr r3,r1; blr" );
		}
		
	#endif
	
	static inline const stack_frame_ppc* get_top_frame()
	{
		return ( (const stack_frame_ppc*) get_r1() )->next;
	}
	
#endif
	
#ifdef __i386__
	
	typedef stack_frame_x86 stack_frame;
	
	static char *get_ebp( void )
	{
		__asm__( "mov  %ebp,%eax" );
	}
	
	static inline const stack_frame_x86* get_top_frame()
	{
		return (const stack_frame_x86*) get_ebp();
	}
	
#endif
	
	stack_frame_pointer get_stack_frame_pointer( int levels_to_skip )
	{
		stack_frame* frame = get_top_frame()->next;
		
		while ( frame != NULL  &&  --levels_to_skip >= 0 )
		{
			frame = frame->next;  // FIXME:  This needs to perform the usual checks
		}
		
		return reinterpret_cast< stack_frame_pointer >( frame );
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
	static void crawl_stack( unsigned                     level,
	                         const StackFrame            *frame,
	                         std::vector< frame_data >&   result )
	{
	next:
		
		if ( frame == NULL  ||  level > 63 )
		{
			return;
		}
		
		++level;
		
		typedef switch_frame_traits< StackFrame > traits;
		
		if ( traits::can_switch )
		{
			if ( typename traits::next_frame_type next = traits::check( frame ) )
			{
				crawl_stack( level, next, result );
				
				return;
			}
		}
		
		result.push_back( frame_data( (stack_frame_pointer) frame,
		                              frame->return_address ) );
		
		if ( frame->next < frame )
		{
			return;
		}
		
		
		frame = frame->next;
		
		goto next;
	}
	
	static std::vector< frame_data > make_stack_crawl( const stack_frame* top )
	{
		std::vector< frame_data > result;
		
		try
		{
			crawl_stack( 0, top, result );
		}
		catch ( const std::bad_alloc& )
		{
		}
		
		return result;
	}
	
	std::vector< frame_data > make_stack_crawl_from_top( stack_frame_pointer top )
	{
		const stack_frame* frame = (const stack_frame*) top;
		
		return make_stack_crawl( frame );
	}
	
	std::vector< frame_data > make_stack_crawl()
	{
		return make_stack_crawl( get_top_frame() );
	}
	
}

