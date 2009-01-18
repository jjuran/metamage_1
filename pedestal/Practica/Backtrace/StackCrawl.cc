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

// Backtrace
#include "Backtrace/MemoryLimit.hh"


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
	
	
	inline const StackFramePPC* MixedModeSwitchFrame( const StackFrame68K* frame )
	{
		return *(unsigned long*) frame == 0xffffffff ? (const StackFramePPC*) frame + 1
		                                             : NULL;
	}
	
	inline const StackFrame68K* MixedModeSwitchFrame( const StackFramePPC* frame )
	{
		return (long) frame & 0x00000001 ? (const StackFrame68K*) ((long) frame - 1)
		                                 : NULL;
	}
	
	static void CrawlStackPPC( unsigned level, const StackFramePPC* frame, const void* limit, std::vector< ReturnAddress >& result );
	
#if defined( __MC68K__ )  ||  defined( __MACOS__ )
	
	static void CrawlStack68K( unsigned level, const StackFrame68K* frame, const void* limit, std::vector< ReturnAddress >& result )
	{
		if ( frame == NULL  ||  frame >= limit )
		{
			return;
		}
		
	#ifdef __MACOS__
		
		if ( const StackFramePPC* switchFrame = MixedModeSwitchFrame( frame ) )
		{
			CrawlStackPPC( level, switchFrame, limit, result );
			
			return;
		}
		
	#endif
		
		ReturnAddr68K addr = frame->returnAddr;
		
		result.push_back( ReturnAddress( addr ) );
		
		if ( frame->next < frame )
		{
			return;
		}
		
		CrawlStack68K( level + 1, frame->next, limit, result );
	}
	
	inline void CrawlStack( const StackFrame68K* frame, const void* limit, std::vector< ReturnAddress >& result )
	{
		CrawlStack68K( 0, frame, limit, result );
	}
	
#endif
	
	
#ifdef __MACOS__
	
	static bool AddressExceedsMemoryLimit( const StackFramePPC* frame )
	{
		static bool visited     = false;
		static bool shouldCheck = true;
		
		bool failed = shouldCheck  &&  frame >= MemoryLimit();
		
		if ( !visited )
		{
			visited = true;
			
			// If the first return address fails the memory limit check,
			// then the check is worthless.
			// Only continue checking if the first check passes.
			shouldCheck = !failed;
			
			return false;
		}
		
		return failed;
	}
	
#endif
	
#if defined( __POWERPC__ )  ||  defined( __MACOS__ )
	
	static void CrawlStackPPC( unsigned level, const StackFramePPC* frame, const void* limit, std::vector< ReturnAddress >& result )
	{
		if ( frame == NULL  ||  frame >= limit )
		{
			return;
		}
		
	#ifdef __MACOS__
		
		if ( AddressExceedsMemoryLimit( frame ) )
		{
			return;
		}
		
		if ( level > 100 )
		{
			return;
		}
		
		if ( const StackFrame68K* switchFrame = MixedModeSwitchFrame( frame ) )
		{
			CrawlStack68K( level, switchFrame, limit, result );
			
			return;
		}
		
	#endif
		
		ReturnAddrPPC addr = frame->returnAddr;
		
		result.push_back( ReturnAddress( addr ) );
		
		if ( frame->next < frame )
		{
			return;
		}
		
		CrawlStackPPC( level + 1, frame->next, limit, result );
	}
	
	inline void CrawlStack( const StackFramePPC* frame, const void* limit, std::vector< ReturnAddress >& result )
	{
		CrawlStackPPC( 0, frame, limit, result );
	}
	
#endif
	
#ifdef __i386__
	
	static void CrawlStackX86( unsigned level, const StackFrameX86* frame, const void* limit, std::vector< ReturnAddress >& result )
	{
		if ( frame == NULL )
		{
			return;
		}
		
		ReturnAddrX86 addr = frame->returnAddr;
		
		result.push_back( ReturnAddress( addr ) );
		
		if ( frame->next < frame )
		{
			return;
		}
		
		CrawlStackX86( level, frame->next, limit, result );
	}
	
	inline void CrawlStack( const StackFrameX86* frame, const void* limit, std::vector< ReturnAddress >& result )
	{
		CrawlStackX86( 0, frame, limit, result );
	}
	
#endif
	
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
			
			CrawlStack( top, limit, result );
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

