/*	==============
 *	KernelStubs.hh
 *	==============
 */

#ifndef KERNELSTUBS_HH
#define KERNELSTUBS_HH

// Silver
#include "Silver/ProcTypes.hh"


namespace Kerosene
{
	//namespace N = Nitrogen;
	
	//N::CFragConnectionID KernelLib();
	
	void* GetSymbol( const char* name );
	
}

namespace
{
	
	#pragma mark -
	#pragma mark ¥ Glue ¥
	
	//namespace N = Nitrogen;
	namespace Ag = Silver;
	
	using Kerosene::GetSymbol;
	
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
					//ptr = N::FindSymbol< ProcPtr >( KernelLib(), N::Str255( name ) );
					ptr = reinterpret_cast< ProcPtr >( GetSymbol( name ) );
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
	
	template < class R, class A1, class A2, class A3, class A4 >
	class Stub4
	{
		public:
			typedef typename Ag::ProcType4< R, A1, A2, A3, A4 >::CProcPtr CProcPtr;
			
			Stub4( const char* name ) : proc( name )  {}
			
			R operator()( A1 a1, A2 a2, A3 a3, A4 a4 )
			{
				return proc()( a1, a2, a3, a4 );
			}
		
		private:
			ProcSymbol< CProcPtr > proc;
	};
	
	template < class R, class A1, class A2, class A3, class A4, class A5 >
	class Stub5
	{
		public:
			typedef typename Ag::ProcType5< R, A1, A2, A3, A4, A5 >::CProcPtr CProcPtr;
			
			Stub5( const char* name ) : proc( name )  {}
			
			R operator()( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5 )
			{
				return proc()( a1, a2, a3, a4, a5 );
			}
		
		private:
			ProcSymbol< CProcPtr > proc;
	};
	
	template < class R, class A1, class A2, class A3, class A4, class A5, class A6 >
	class Stub6
	{
		public:
			typedef typename Ag::ProcType6< R, A1, A2, A3, A4, A5, A6 >::CProcPtr CProcPtr;
			
			Stub6( const char* name ) : proc( name )  {}
			
			R operator()( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6 )
			{
				return proc()( a1, a2, a3, a4, a5, a6 );
			}
		
		private:
			ProcSymbol< CProcPtr > proc;
	};
	
}

#endif

