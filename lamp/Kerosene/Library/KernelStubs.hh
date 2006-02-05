/*	==============
 *	KernelStubs.hh
 *	==============
 */

#ifndef KERNELSTUBS_HH
#define KERNELSTUBS_HH

// Nitrogen
#include "Nitrogen/CodeFragments.h"
#include "Nitrogen/Str.h"

// Silver
#include "Silver/ProcTypes.hh"


namespace
{
	
	#pragma mark -
	#pragma mark ¥ Glue ¥
	
	namespace N = Nitrogen;
	namespace Ag = Silver;
	
	N::CFragConnectionID KernelLib();
	
	template < class Output, class Input >
	Output ProcPtr_Cast( Input input )
	{
		return reinterpret_cast< Output >( reinterpret_cast< long >( input ) );
	}
	
	template < class ProcPtr >
	class ProcSymbol
	{
		private:
			const char* name;
			ProcPtr ptr;
			
			ProcPtr InternalGet()
			{
				if ( ptr == NULL )
				{
					ptr = N::FindSymbol< ProcPtr >( KernelLib(), N::Str255( name ) );
				}
				
				return ptr;
			}
		
		public:
			ProcSymbol( const char* name ) : name( name ), ptr( NULL )  {}
			
			ProcPtr Get()
			{
				try
				{
					return InternalGet();
				}
				catch ( ... ) {}
				
				return NULL;
			}
			
			ProcPtr operator()()
			{
				return InternalGet();
			}
	};
	
	template < class Result >
	class Stub0
	{
		public:
			typedef typename Ag::ProcType0< Result >::CProcPtr CProcPtr;
			
			Stub0( const char* name ) : proc( name )  {}
			
			Result operator()()  { return proc()(); }
		
		private:
			ProcSymbol< CProcPtr > proc;
	};
	
	template < class Result, class Arg1 >
	class Stub1
	{
		public:
			typedef typename Ag::ProcType1< Result, Arg1 >::CProcPtr CProcPtr;
			
			Stub1( const char* name ) : proc( name )  {}
			
			Result operator()( Arg1 a1 )  { return proc()( a1 ); }
		
		private:
			ProcSymbol< CProcPtr > proc;
	};
	
	template < class Result, class Arg1, class Arg2 >
	class Stub2
	{
		public:
			typedef typename Ag::ProcType2< Result, Arg1, Arg2 >::CProcPtr CProcPtr;
			
			Stub2( const char* name ) : proc( name )  {}
			
			Result operator()( Arg1 a1, Arg2 a2 )  { return proc()( a1, a2 ); }
		
		private:
			ProcSymbol< CProcPtr > proc;
	};
	
	template < class Result, class Arg1, class Arg2, class Arg3 >
	class Stub3
	{
		public:
			typedef typename Ag::ProcType3< Result, Arg1, Arg2, Arg3 >::CProcPtr CProcPtr;
			
			Stub3( const char* name ) : proc( name )  {}
			
			Result operator()( Arg1 a1, Arg2 a2, Arg3 a3 )  { return proc()( a1, a2, a3 ); }
		
		private:
			ProcSymbol< CProcPtr > proc;
	};
	
	template < class Result, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5 >
	class Stub5
	{
		public:
			typedef typename Ag::ProcType5< Result, Arg1, Arg2, Arg3, Arg4, Arg5 >::CProcPtr CProcPtr;
			
			Stub5( const char* name ) : proc( name )  {}
			
			Result operator()( Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5 )  { return proc()( a1, a2, a3, a4, a5 ); }
		
		private:
			ProcSymbol< CProcPtr > proc;
	};
	
}

#endif

