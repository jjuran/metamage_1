/*	=============
 *	StackCrawl.cc
 *	=============
 */

#include "Backtrace/StackCrawl.hh"

// Mac OS Universal Interfaces
#if defined( __MACOS__ ) && defined( __POWERPC__ )
#include <MachineExceptions.h>
#endif

// Standard C/C++
#include <cstdio>

// Standard C
#include <setjmp.h>


namespace Backtrace
{
	
#if defined( __MACOS__ ) && defined( __POWERPC__ )
	
	class ScopedExceptionHandler
	{
		private:
			ExceptionHandlerTPP itsSavedHandler;
		
		public:
			static ExceptionHandlerTPP Install( ExceptionHandlerTPP handler );
			
			ScopedExceptionHandler( ExceptionHandlerTPP handler ) : itsSavedHandler( Install( handler ) )  {}
			
			~ScopedExceptionHandler()  { Install( itsSavedHandler ); }
	};
	
	ExceptionHandlerTPP ScopedExceptionHandler::Install( ExceptionHandlerTPP handler )
	{
		return ::InstallExceptionHandler( handler );
	}
	
	class ScopeToTrapBadAddresses
	{
		private:
			ExceptionHandlerUPP     itsUPP;
			ScopedExceptionHandler  itsScope;
		
		public:
			ScopeToTrapBadAddresses( ExceptionHandlerProcPtr handler ) : itsUPP( ::NewExceptionHandlerUPP( handler ) ),
			                                                             itsScope( itsUPP )
			{
			}
			
			~ScopeToTrapBadAddresses()
			{
				::DisposeExceptionHandlerUPP( itsUPP );
			}
	};
	
	
	static jmp_buf gStackCrawlJmpBuf;
	
	static OSStatus BadAddressTrappingHandler( ExceptionInformation* exception )
	{
		if ( exception->theKind == kTraceException )
		{
			return -1;  // handled by debugger
		}
		
		switch ( exception->theKind )
		{
			case kAccessException:
			case kUnmappedMemoryException:
			case kUnresolvablePageFaultException:
				longjmp( gStackCrawlJmpBuf, exception->theKind );
				// not reached
			
			default:
				break;
		}
		
		return -1;
	}
	
#endif
	
	struct StackFrame68K
	{
		StackFrame68K*  next;
		ReturnAddr68K   returnAddr;
	};
	
	struct StackFramePPC
	{
		StackFramePPC*  next;
		const void*     savedCR;
		ReturnAddrPPC   returnAddr;
	};
	
	struct StackFrameX86
	{
		StackFrameX86*    next;
		ReturnAddrNative  returnAddr;
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
	
	StackFramePtr GetStackFramePointer( int levelsToSkip )
	{
		StackFrame* frame = GetTopFrame()->next;
		
		while ( frame != NULL  &&  --levelsToSkip >= 0 )
		{
			frame = frame->next;  // FIXME:  This needs to perform the usual checks
		}
		
		return reinterpret_cast< StackFramePtr >( frame );
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
	static void CrawlStack( unsigned level, const StackFrame* frame, const void* limit, std::vector< ReturnAddress >& result )
	{
	next:
		
		if ( frame == NULL  ||  frame >= limit  ||  level > 99 )
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
		
		result.push_back( ReturnAddress( frame->returnAddr ) );
		
		if ( frame->next < frame )
		{
			return;
		}
		
		
		frame = frame->next;
		
		goto next;
	}
	
#if defined( __MACOS__ ) && defined( __POWERPC__ )
	
	class MachineException
	{
		private:
			::ExceptionKind itsKind;
		
		public:
			MachineException() : itsKind()  {}
			
			MachineException( ::ExceptionKind kind ) : itsKind( kind )  {}
			
			::ExceptionKind Get() const  { return itsKind; }
	};
	
#endif
	
	static std::vector< ReturnAddress > MakeStackCrawl( const StackFrame* top, const void* limit )
	{
		std::vector< ReturnAddress > result;
		
		try
		{
		#if defined( __MACOS__ ) && defined( __POWERPC__ )
			
			ScopeToTrapBadAddresses trappingUnmappedMemoryExceptions( BadAddressTrappingHandler );
			
			if ( ::ExceptionKind kind = setjmp( gStackCrawlJmpBuf ) )
			{
				// exception handler invoked
				
				throw MachineException( kind );
			}
			
		#endif
			
			CrawlStack( 0, top, limit, result );
		}
		
	#if defined( __MACOS__ ) && defined( __POWERPC__ )
		
		catch ( const MachineException& e )
		{
			switch ( e.Get() )
			{
				case kAccessException:
				case kUnmappedMemoryException:
				case kUnresolvablePageFaultException:
					// nothing
					break;
				
				default:
					//std::fprintf( stderr, "Machine exception type %d caught during stack crawl\n", e.Get() );
					break;
			}
		}
		
	#endif
		
		catch ( const std::bad_alloc& )
		{
		}
		
		return result;
	}
	
	std::vector< ReturnAddress > MakeStackCrawlFromTopToBottom( StackFramePtr top, const void* limit )
	{
		const StackFrame* frame = reinterpret_cast< const StackFrame* >( top );
		
		return MakeStackCrawl( frame, limit );
	}
	
	std::vector< ReturnAddress > MakeStackCrawlToBottom( const void* limit )
	{
		return MakeStackCrawl( GetTopFrame(), limit );
	}
	
}

