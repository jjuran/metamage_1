/*	===================
 *	PointerToFunction.h
 *	===================
 */

#ifndef TEMPLATES_POINTERTOFUNCTION_H
#define TEMPLATES_POINTERTOFUNCTION_H

// Standard C++
#include <functional>


namespace Nitrogen
{
	
	template < class Result >
	struct ConstantFunction
	{
		typedef Result result_type;
	};
	
	template < class Result >
	class PointerToConstantFunction : public ConstantFunction< Result >
	{
		public:
			typedef Result ( *function_type )();
		
		private:
			function_type f;
		
		public:
			PointerToConstantFunction( function_type f ) : f( f )  {}
			
			Result operator()() const
			{
				return f();
			}
	};
	
	template < class T1, class Result >
	class PointerToUnaryFunction : public std::unary_function< T1, Result >
	{
		public:
			typedef Result ( *function_type )( T1 );
		
		private:
			function_type f;
		
		public:
			PointerToUnaryFunction( function_type f ) : f( f )  {}
			
			Result operator()( T1 a ) const
			{
				return f( a );
			}
	};
	
	template < class T1, class Result >
	class PointerToUnaryFunctionWithConstArg : public std::unary_function< T1, Result >
	{
		public:
			typedef Result ( *function_type )( const T1& );
		
		private:
			function_type f;
		
		public:
			PointerToUnaryFunctionWithConstArg( function_type f ) : f( f )  {}
			
			Result operator()( const T1& a ) const
			{
				return f( a );
			}
	};
	
	template < class T1, class T2, class Result >
	class PointerToBinaryFunction : public std::binary_function< T1, T2, Result >
	{
		public:
			typedef Result ( *function_type )( T1, T2 );
		
		private:
			function_type f;
		
		public:
			PointerToBinaryFunction( function_type f ) : f( f )  {}
			
			Result operator()( T1 a, T2 b ) const
			{
				return f( a, b );
			}
	};
	
	template < class T1, class T2, class Result >
	class PointerToBinaryFunctionWithConstArg1 : public std::binary_function< T1, T2, Result >
	{
		public:
			typedef Result ( *function_type )( const T1&, T2 );
		
		private:
			function_type f;
		
		public:
			PointerToBinaryFunctionWithConstArg1( function_type f ) : f( f )  {}
			
			Result operator()( const T1& a, T2 b ) const
			{
				return f( a, b );
			}
	};
	
	template < class T1, class T2, class Result >
	class PointerToBinaryFunctionWithConstArg2 : public std::binary_function< T1, T2, Result >
	{
		public:
			typedef Result ( *function_type )( T1, const T2& );
		
		private:
			function_type f;
		
		public:
			PointerToBinaryFunctionWithConstArg2( function_type f ) : f( f )  {}
			
			Result operator()( T1 a, const T2& b ) const
			{
				return f( a, b );
			}
	};
	
	template < class T1, class T2, class Result >
	class PointerToBinaryFunctionWithConstArgs : public std::binary_function< T1, T2, Result >
	{
		public:
			typedef Result ( *function_type )( const T1&, const T2& );
		
		private:
			function_type f;
		
		public:
			PointerToBinaryFunctionWithConstArgs( function_type f ) : f( f )  {}
			
			Result operator()( const T1& a, const T2& b ) const
			{
				return f( a, b );
			}
	};
	
	template < class Result >
	PointerToConstantFunction< Result >
	PtrFun( Result (f)() )
	{
		return PointerToConstantFunction< Result >( f );
	}
	
	template < class T1, class Result >
	PointerToUnaryFunction< T1, Result >
	PtrFun( Result (f)( T1 ) )
	{
		return PointerToUnaryFunction< T1, Result >( f );
	}
	
	template < class T1, class Result >
	PointerToUnaryFunctionWithConstArg< T1, Result >
	PtrFun( Result (f)( const T1& ) )
	{
		return PointerToUnaryFunctionWithConstArg< T1, Result >( f );
	}
	
	template < class T1, class T2, class Result >
	PointerToBinaryFunction< T1, T2, Result >
	PtrFun( Result (f)( T1, T2 ) )
	{
		return PointerToBinaryFunction< T1, T2, Result >( f );
	}
	
	template < class T1, class T2, class Result >
	PointerToBinaryFunctionWithConstArg1< T1, T2, Result >
	PtrFun( Result (f)( const T1&, T2 ) )
	{
		return PointerToBinaryFunctionWithConstArg1< T1, T2, Result >( f );
	}
	
	template < class T1, class T2, class Result >
	PointerToBinaryFunctionWithConstArg2< T1, T2, Result >
	PtrFun( Result (f)( T1, const T2& ) )
	{
		return PointerToBinaryFunctionWithConstArg2< T1, T2, Result >( f );
	}
	
	template < class T1, class T2, class Result >
	PointerToBinaryFunctionWithConstArgs< T1, T2, Result >
	PtrFun( Result (f)( const T1&, const T2& ) )
	{
		return PointerToBinaryFunctionWithConstArgs< T1, T2, Result >( f );
	}
	
}

#endif

