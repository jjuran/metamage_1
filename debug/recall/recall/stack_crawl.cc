/*	=============
 *	StackCrawl.cc
 *	=============
 */

#include "recall/stack_crawl.hh"


namespace recall
{
	
	struct StackFrame68K
	{
		StackFrame68K*  next;
		return_address_68k   returnAddr;
	};
	
	struct StackFramePPC
	{
		StackFramePPC*  next;
		const void*     savedCR;
		return_address_ppc   returnAddr;
	};
	
	struct StackFrameX86
	{
		StackFrameX86*    next;
		return_address_native  returnAddr;
	};
	
	
#ifdef __MC68K__
	
	typedef StackFrame68K StackFrame;
	
	#pragma parameter __D0 GetA6
	
	inline char* GetA6() = { 0x200e };
	
	inline const StackFrame68K* GetTopFrame()  { return (const StackFrame68K*) GetA6(); }
	
#endif

#ifdef __POWERPC__
	
	typedef StackFramePPC StackFrame;
	
	#ifdef __MWERKS__
		
		static asm char *GetSP( void )
		{
			mr		r3,r1
			blr
		}
		
	#endif
	
	#ifdef __GNUC__
		
		static char *GetSP( void )
		{
			__asm__( "mr r3,r1; blr" );
		}
		
	#endif
	
	inline const StackFramePPC* GetTopFrame()  { return ( (const StackFramePPC*) GetSP() )->next; }
	
#endif
	
#ifdef __i386__
	
	typedef StackFrameX86 StackFrame;
	
	static char *GetEBP( void )
	{
		__asm__( "mov  %ebp,%eax" );
	}
	
	inline const StackFrameX86* GetTopFrame()  { return (const StackFrameX86*) GetEBP(); }
	
#endif
	
	stack_frame_pointer get_stack_frame_pointer( int levels_to_skip )
	{
		StackFrame* frame = GetTopFrame()->next;
		
		while ( frame != NULL  &&  --levels_to_skip >= 0 )
		{
			frame = frame->next;  // FIXME:  This needs to perform the usual checks
		}
		
		return reinterpret_cast< stack_frame_pointer >( frame );
	}
	
	template < class StackFrame > struct SwitchFrame_Traits
	{
		static const bool can_switch = false;
		
		typedef const StackFrame *ThisPtr;
		typedef const StackFrame *NextPtr;
		
		static NextPtr Check( ThisPtr frame )  { return NULL; }
	};
	
#ifdef __MACOS__
	
	// PPC calls 68K
	template <> struct SwitchFrame_Traits< StackFrame68K >
	{
		static const bool can_switch = true;
		
		typedef const StackFrame68K *ThisPtr;
		typedef const StackFramePPC *NextPtr;
		
		static NextPtr Check( ThisPtr frame );
	};
	
	// 68K calls PPC
	template <> struct SwitchFrame_Traits< StackFramePPC >
	{
		static const bool can_switch = true;
		
		typedef const StackFramePPC *ThisPtr;
		typedef const StackFrame68K *NextPtr;
		
		static NextPtr Check( ThisPtr frame );
	};
	
	template <>
	inline const StackFramePPC* SwitchFrame_Traits< StackFrame68K >::Check( ThisPtr frame )
	{
		const StackFrame68K* next = frame->next;
		
		if ( next != NULL  &&  ((unsigned long*) next)[-1] == 0xffffffff )
		{
			const void* addr = frame->returnAddr;
			
			if ( frame < addr  &&  addr < next )
			{
				return (const StackFramePPC*) next;
			}
		}
		
		return NULL;
	}
	
	template <>
	inline const StackFrame68K* SwitchFrame_Traits< StackFramePPC >::Check( ThisPtr frame )
	{
		return (long) frame & 0x00000001 ? (const StackFrame68K*) ((long) frame - 1)
		                                 : NULL;
	}
	
#endif
	
	
	template < class StackFrame >
	static void CrawlStack( unsigned level, const StackFrame* frame, const void* limit, std::vector< frame_data >& result )
	{
	next:
		
		if ( frame == NULL  ||  frame >= limit  ||  level > 63 )
		{
			return;
		}
		
		++level;
		
		typedef SwitchFrame_Traits< StackFrame > Traits;
		
		if ( Traits::can_switch )
		{
			if ( typename Traits::NextPtr next = Traits::Check( frame ) )
			{
				CrawlStack( level, next, limit, result );
				
				return;
			}
		}
		
		result.push_back( frame_data( (stack_frame_pointer) frame, frame->returnAddr ) );
		
		if ( frame->next < frame )
		{
			return;
		}
		
		
		frame = frame->next;
		
		goto next;
	}
	
	static std::vector< frame_data > make_stack_crawl( const StackFrame* top, const void* limit )
	{
		std::vector< frame_data > result;
		
		try
		{
			CrawlStack( 0, top, limit, result );
		}
		catch ( const std::bad_alloc& )
		{
		}
		
		return result;
	}
	
	std::vector< frame_data > make_stack_crawl_from_top_to_bottom( stack_frame_pointer top, const void* limit )
	{
		const StackFrame* frame = reinterpret_cast< const StackFrame* >( top );
		
		return make_stack_crawl( frame, limit );
	}
	
	std::vector< frame_data > make_stack_crawl_to_bottom( const void* limit )
	{
		return make_stack_crawl( GetTopFrame(), limit );
	}
	
}

