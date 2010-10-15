/*
	recurse.hh
	----------
*/

#ifndef LAMP_RECURSE_HH
#define LAMP_RECURSE_HH

// Lamp
#include "lamp/restack.h"
#include "lamp/stack.h"


namespace lamp
{

const unsigned stack_margin = 12 * 1024;

inline bool stack_is_sufficient()
{
#ifndef __LAMP__
	
	return true;
	
#endif
	
	return _lamp_stack_space() >= stack_margin;
}


template < class T >
struct raw_parameter
{
	typedef T type;
	
	static const type& convert( const T& input )
	{
		return input;
	}
};

template < class T >
struct raw_parameter< T& >
{
	// convert references to pointers
	typedef T* type;
	
	static type convert( T& input )
	{
		return &input;
	}
};

/*
	Legend:
	
	F:    Function type
	T:    Trampoline function type
	An:   type of nth Argument to f
	Tn:   Traits for nth argument
	F_n:  type of nth Parameter from F's prototype
	T_n:  type of nth argument after replacing references with pointers
*/

template < class R >
inline R recurse( R (*f)() )
{
	if ( stack_is_sufficient() )
	{
		return f();
	}
	
	typedef R (*F)(        );
	typedef R (*T)( int, F );
	
	const T restack = (T) _lamp_restack;
	
	const unsigned size = 0;
	
	return restack( size, f );
}

template < class R, class F_1, class A1 >
inline R recurse( R (*f)( F_1 ), A1 a1 )
{
	if ( stack_is_sufficient() )
	{
		return f( a1 );
	}
	
	typedef raw_parameter< A1 > T1;
	
	typedef typename T1::type T_1;
	
	typedef R (*F)(         F_1 );
	typedef R (*T)( int, F, T_1 );
	
	const T restack = (T) _lamp_restack;
	
	const unsigned size = sizeof (T_1);
	
	return restack( size, f, T1::convert( a1 ) );
}

template < class R, class F_1, class A1, class F_2, class A2 >
inline R recurse( R (*f)( F_1, F_2 ), A1 a1, A2 a2 )
{
	if ( stack_is_sufficient() )
	{
		return f( a1, a2 );
	}
	
	typedef raw_parameter< A1 > T1;
	typedef raw_parameter< A2 > T2;
	
	typedef typename T1::type T_1;
	typedef typename T2::type T_2;
	
	typedef R (*F)(         F_1, F_2 );
	typedef R (*T)( int, F, T_1, T_2 );
	
	const T restack = (T) _lamp_restack;
	
	const unsigned size = sizeof (T_1)
	                    + sizeof (T_2);
	
	return restack( size, f, T1::convert( a1 ),
	                         T2::convert( a2 ) );
}

}

#endif

