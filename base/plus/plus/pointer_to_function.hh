/*
	pointer_to_function.hh
	----------------------
*/

#ifndef PLUS_POINTERTOFUNCTION_HH
#define PLUS_POINTERTOFUNCTION_HH


namespace plus
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
	class pointer_to_unary_function
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
	class pointer_to_unary_function_with_const_arg
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
	class pointer_to_binary_function
	{
		public:
			typedef T1      first_argument_type;
			typedef T2      second_argument_type;
			typedef Result  result_type;
			
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
	class pointer_to_binary_function_with_const_arg1
	{
		public:
			typedef T1      first_argument_type;
			typedef T2      second_argument_type;
			typedef Result  result_type;
			
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
	class pointer_to_binary_function_with_const_arg2
	{
		public:
			typedef T1      first_argument_type;
			typedef T2      second_argument_type;
			typedef Result  result_type;
			
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
	class pointer_to_binary_function_with_const_args
	{
		public:
			typedef T1      first_argument_type;
			typedef T2      second_argument_type;
			typedef Result  result_type;
			
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
