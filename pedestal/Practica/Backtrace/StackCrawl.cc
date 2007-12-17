/*	=============
 *	StackCrawl.cc
 *	=============
 */

#include "Backtrace/StackCrawl.hh"

// Mac OS Universal Interfaces
#if defined( __MACOS__ ) && defined( __POWERPC__ )
#include <MachineExceptions.h>
#endif

// Standard C
#include <setjmp.h>


namespace Backtrace
{
	
#if defined( __MACOS__ ) && defined( __POWERPC__ )
	
	static jmp_buf gStackCrawlJmpBuf;
	
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
	
	static OSStatus UnmappedMemoryExceptionTrappingHandler( ExceptionInformation* exception )
	{
		if ( exception->theKind == kTraceException )
		{
			return -1;  // handled by debugger
		}
		
		if ( exception->theKind == kUnmappedMemoryException )
		{
			longjmp( gStackCrawlJmpBuf, 1 );
		}
		
		return -1;
	}
	
	class ScopeToTrapUnmappedMemoryExceptions : public ScopedExceptionHandler
	{
		private:
			ExceptionHandlerUPP itsUPP;
		
		public:
			ScopeToTrapUnmappedMemoryExceptions() : ScopedExceptionHandler( itsUPP = ::NewExceptionHandlerUPP( UnmappedMemoryExceptionTrappingHandler ) )
			{
			}
			
			~ScopeToTrapUnmappedMemoryExceptions()
			{
				::DisposeExceptionHandlerUPP( itsUPP );
			}
	};
	
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
		StackFrameX86*   next;
		ReturnAddrMachO  returnAddr;
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
	
	StackFramePtr GetStackFramePointer()
	{
		return reinterpret_cast< StackFramePtr >( GetTopFrame() );
	}
	
	
	static const StackFramePPC* SwitchBackToPPCFrom68K( const StackFrame68K* frame )
	{
		const StackFramePPC* switchFrame = (const StackFramePPC*) frame;
		
		return switchFrame;
	}
	
	static const StackFrame68K* SwitchBackTo68KFromPPC( const StackFramePPC* frame )
	{
		const StackFrame68K* switchFrame = (const StackFrame68K*) ((long) frame - 1);
		
		return switchFrame;
	}
	
	static void CrawlStackPPC( unsigned level, const StackFramePPC* frame, std::vector< ReturnAddress >& result );
	
#if defined( __MC68K__ )  ||  defined( __MACOS__ ) && !defined( __MACH__ )
	
	static void CrawlStack68K( unsigned level, const StackFrame68K* frame, std::vector< ReturnAddress >& result )
	{
		if ( frame == NULL )
		{
			return;
		}
		
	#if defined( __MACOS__ ) && !defined( __MACH__ )
		
		if ( *((unsigned long*) frame - 1) == 0xffffffff )
		{
			const StackFramePPC* switchFrame = SwitchBackToPPCFrom68K( frame );
			
			CrawlStackPPC( level, switchFrame, result );
			
			return;
		}
		
	#endif
		
		ReturnAddr68K addr = frame->returnAddr;
		
		result.push_back( ReturnAddress( addr ) );
		
		if ( frame->next < frame )
		{
			return;
		}
		
		CrawlStack68K( level + 1, frame->next, result );
	}
	
	inline void CrawlStack( const StackFrame68K* frame, std::vector< ReturnAddress >& result )
	{
		CrawlStack68K( 0, frame, result );
	}
	
#endif
	
#if defined( __POWERPC__ )  ||  defined( __MACOS__ ) && !defined( __MACH__ )
	
	static void CrawlStackPPC( unsigned level, const StackFramePPC* frame, std::vector< ReturnAddress >& result )
	{
		if ( frame == NULL )
		{
			return;
		}
		
	#if defined( __MACOS__ ) && !defined( __MACH__ )
		
		if ( level > 100 )
		{
			return;
		}
		
		if ( (long) frame & 0x00000001 )
		{
			const StackFrame68K* switchFrame = SwitchBackTo68KFromPPC( frame );
			
			CrawlStack68K( level, switchFrame, result );
			
			return;
		}
		
	#endif
		
		ReturnAddrPPC addr = frame->returnAddr;
		
		result.push_back( ReturnAddress( addr ) );
		
		if ( frame->next < frame )
		{
			return;
		}
		
		CrawlStackPPC( level + 1, frame->next, result );
	}
	
	inline void CrawlStack( const StackFramePPC* frame, std::vector< ReturnAddress >& result )
	{
		CrawlStackPPC( 0, frame, result );
	}
	
#endif
	
#ifdef __i386__
	
	static void CrawlStackX86( unsigned level, const StackFrameX86* frame, std::vector< ReturnAddress >& result )
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
		
		CrawlStackX86( level, frame->next, result );
	}
	
	inline void CrawlStack( const StackFrameX86* frame, std::vector< ReturnAddress >& result )
	{
		CrawlStackX86( 0, frame, result );
	}
	
#endif
	
	class UnmappedMemoryException {};
	
	static std::vector< ReturnAddress > MakeStackCrawl( const StackFrame* top )
	{
		std::vector< ReturnAddress > result;
		
		try
		{
		#if defined( __MACOS__ ) && defined( __POWERPC__ )
			
			ScopeToTrapUnmappedMemoryExceptions trappingUnmappedMemoryExceptions;
			
			if ( setjmp( gStackCrawlJmpBuf ) )
			{
				// exception handler invoked
				
				throw UnmappedMemoryException();
			}
			
		#endif
			
			CrawlStack( top, result );
		}
		catch ( const UnmappedMemoryException& )
		{
			//std::fprintf( stderr, "Unmapped memory exception caught during stack crawl\n" );
		}
		catch ( const std::bad_alloc& )
		{
		}
		
		return result;
	}
	
	std::vector< ReturnAddress > MakeStackCrawl( StackFramePtr top )
	{
		const StackFrame* frame = reinterpret_cast< const StackFrame* >( top );
		
		return MakeStackCrawl( frame );
	}
	
	std::vector< ReturnAddress > MakeStackCrawl()
	{
		return MakeStackCrawl( GetTopFrame() );
	}
	
}

