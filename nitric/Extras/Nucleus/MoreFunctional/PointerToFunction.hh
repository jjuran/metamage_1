/*	====================
 *	PointerToFunction.hh
 *	====================
 */

#ifndef MOREFUNCTIONAL_POINTERTOFUNCTION_HH
#define MOREFUNCTIONAL_POINTERTOFUNCTION_HH

// Standard C++
#include <functional>


namespace more
{
	
	template < class Result >
	struct constant_function
	{
		typedef Result result_type;
	};
	
	template < class Result >
	class pointer_to_constant_function : public constant_function< Result >
	{
		public:
			typedef Result ( *function_type )();
		
		private:
			function_type f;
		
		public:
			pointer_to_constant_function( function_type f ) : f( f )  {}
			
			Result operator()() const
			{
				return f();
			}
	};
	
	template < class T1, class Result >
	class pointer_to_unary_function : public std::unary_function< T1, Result >
	{
		public:
			typedef Result ( *function_type )( T1 );
		
		private:
			function_type f;
		
		public:
			pointer_to_unary_function( function_type f ) : f( f )  {}
			
			Result operator()( T1 a ) const
			{
				return f( a );
			}
	};
	
	template < class T1, class Result >
	class pointer_to_unary_function_with_const_arg : public std::unary_function< T1, Result >
	{
		public:
			typedef Result ( *function_type )( const T1& );
		
		private:
			function_type f;
		
		public:
			pointer_to_unary_function_with_const_arg( function_type f ) : f( f )  {}
			
			Result operator()( const T1& a ) const
			{
				return f( a );
			}
	};
	
	template < class T1, class T2, class Result >
	class pointer_to_binary_function : public std::binary_function< T1, T2, Result >
	{
		public:
			typedef Result ( *function_type )( T1, T2 );
		
		private:
			function_type f;
		
		public:
			pointer_to_binary_function( function_type f ) : f( f )  {}
			
			Result operator()( T1 a, T2 b ) const
			{
				return f( a, b );
			}
	};
	
	template < class T1, class T2, class Result >
	class pointer_to_binary_function_with_const_arg1 : public std::binary_function< T1, T2, Result >
	{
		public:
			typedef Result ( *function_type )( const T1&, T2 );
		
		private:
			function_type f;
		
		public:
			pointer_to_binary_function_with_const_arg1( function_type f ) : f( f )  {}
			
			Result operator()( const T1& a, T2 b ) const
			{
				return f( a, b );
			}
	};
	
	template < class T1, class T2, class Result >
	class pointer_to_binary_function_with_const_arg2 : public std::binary_function< T1, T2, Result >
	{
		public:
			typedef Result ( *function_type )( T1, const T2& );
		
		private:
			function_type f;
		
		public:
			pointer_to_binary_function_with_const_arg2( function_type f ) : f( f )  {}
			
			Result operator()( T1 a, const T2& b ) const
			{
				return f( a, b );
			}
	};
	
	template < class T1, class T2, class Result >
	class pointer_to_binary_function_with_const_args : public std::binary_function< T1, T2, Result >
	{
		public:
			typedef Result ( *function_type )( const T1&, const T2& );
		
		private:
			function_type f;
		
		public:
			pointer_to_binary_function_with_const_args( function_type f ) : f( f )  {}
			
			Result operator()( const T1& a, const T2& b ) const
			{
				return f( a, b );
			}
	};
	
	template < class Result >
	inline pointer_to_constant_function< Result >
	ptr_fun( Result (f)() )
	{
		return pointer_to_constant_function< Result >( f );
	}
	
	template < class T1, class Result >
	inline pointer_to_unary_function< T1, Result >
	ptr_fun( Result (f)( T1 ) )
	{
		return pointer_to_unary_function< T1, Result >( f );
	}
	
	template < class T1, class Result >
	inline pointer_to_unary_function_with_const_arg< T1, Result >
	ptr_fun( Result (f)( const T1& ) )
	{
		return pointer_to_unary_function_with_const_arg< T1, Result >( f );
	}
	
	template < class T1, class T2, class Result >
	inline pointer_to_binary_function< T1, T2, Result >
	ptr_fun( Result (f)( T1, T2 ) )
	{
		return pointer_to_binary_function< T1, T2, Result >( f );
	}
	
	template < class T1, class T2, class Result >
	inline pointer_to_binary_function_with_const_arg1< T1, T2, Result >
	ptr_fun( Result (f)( const T1&, T2 ) )
	{
		return pointer_to_binary_function_with_const_arg1< T1, T2, Result >( f );
	}
	
	template < class T1, class T2, class Result >
	inline pointer_to_binary_function_with_const_arg2< T1, T2, Result >
	ptr_fun( Result (f)( T1, const T2& ) )
	{
		return pointer_to_binary_function_with_const_arg2< T1, T2, Result >( f );
	}
	
	template < class T1, class T2, class Result >
	inline pointer_to_binary_function_with_const_args< T1, T2, Result >
	ptr_fun( Result (f)( const T1&, const T2& ) )
	{
		return pointer_to_binary_function_with_const_args< T1, T2, Result >( f );
	}
	
}

#endif

