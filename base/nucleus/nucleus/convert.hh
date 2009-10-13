// nucleus/convert.hh
// ------------------

// Written 2002-2007 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


/*
	Convert: The swiss army knife of conversion functions
	
	Some libraries come with a plethora of conversion functions, and adapting
	a library to C++ will often introduce even more.  Keeping track of the
	names given to these functions can be hard for a programmer, and it's
	essentially impossible for a template.
	
	Therefore, Nucleus provides a single, extensible, template, convert(), to
	encompass all simple conversions:
	
		template < class Output, class Input > Output convert( const Input& );
	
	Since the input parameter can be inferred from a function parameter, one
	can often write convert< DesiredType >( input ).
	
	In general, the conversions provided by convert() are value-preserving,
	but not precision-preserving.  If the parameter is out of range for
	the output type, or the conversion cannot be performed, convert()
	throws an exception.  When the Output type has insufficient
	precision to exactly represent the input value, the result
	is chosen from among the closest output values.
	
	Convert also provides conversions between C++ types, as a way of
	priming the pump.  It can be used for range-checked, rounding conversions
	between the built-in numeric types, and it converts things to and
	from the standard string types by using the streaming operators.
	
	Where no specialization has been provided, convert() attempts an implicit
	conversion.
	
	In order to allow for partial specialization, convert< Input, Output s>
	uses a functor
	
		template < class Output, class Input > struct converter;
	
	to choose the conversion.  One may extend convert() by specializing this
	class, providing a member function operator() to perform the conversion:
	
		template < partial-specialization-parameters >
		struct converter< Output, Input > : public std::unary_function< Input, Output >
		{
			Output operator()( const Input& ) const;
		};
	
	When convert() is called with more than one parameter, the additional
	parameters are passed on to the constructor of the converter functor.
*/

#ifndef NUCLEUS_CONVERT_HH
#define NUCLEUS_CONVERT_HH

// Standard C++
#include <sstream>
#include <string>
#include <ios>


namespace nucleus
{
	
	template < class Output, class Input >
	struct converter: public std::unary_function< Input, Output >
	{
		Output operator()( const Input& in ) const      { return Output( in ); }
	};
	
	template < class Input >
	struct convert_input_traits
	{
		typedef Input converter_input_type;
	};
	
	template < class Output, class Input >
	inline Output convert( const Input& input )
	{
		return converter< Output, typename convert_input_traits< Input >::converter_input_type >()( input );
	}
	
	template < class Output, class Input, class P1 >
	inline Output convert( const Input& input, const P1& p1 )
	{
		return converter< Output, typename convert_input_traits< Input >::converter_input_type >( p1 )( input );
	}
	
	template < class Output, class Input, class P1, class P2 >
	inline Output convert( const Input& input, const P1& p1, const P2& p2 )
	{
		return converter< Output, typename convert_input_traits< Input >::converter_input_type >( p1, p2 )( input );
	}
	
	template < class Output, class Input, class P1, class P2, class P3 >
	inline Output convert( const Input& input, const P1& p1, const P2& p2, const P3& p3 )
	{
		return converter< Output, typename convert_input_traits< Input >::converter_input_type >( p1, p2, p3 )( input );
	}
	
	// Bust partial specialization ambiguity
	template < class Char, class Traits, class Allocator >
	struct converter< std::basic_string< Char, Traits, Allocator >,
	                  std::basic_string< Char, Traits, Allocator > >
	: public std::unary_function< std::basic_string< Char, Traits, Allocator >,
	                              std::basic_string< Char, Traits, Allocator > >
	{
		typedef std::basic_string< Char, Traits, Allocator > String;
		
		const String& operator()( const String& input ) const
		{
			return input;
		}
	};
	
	template < class CharT,
	           class Traits,
	           class Allocator,
	           class Input >
	struct converter< std::basic_string< CharT, Traits, Allocator >, Input >
	           : public std::unary_function< Input, std::basic_string< CharT, Traits, Allocator > >
	{
		std::basic_string< CharT, Traits, Allocator > operator()( const Input& input ) const
		{
			std::basic_ostringstream< CharT, Traits, Allocator > stream;
			
			stream << std::boolalpha << input;
			
			return stream.str(); 
		}
	};
	
	class conversion_from_string_failed {};
	
	template < class Output,
	           class CharT,
	           class Traits,
	           class Allocator >
	struct converter< Output, std::basic_string< CharT, Traits, Allocator > >
	         : public std::unary_function< std::basic_string< CharT, Traits, Allocator >, Output >
	{
		Output operator()( const std::basic_string< CharT, Traits, Allocator >& input ) const
		{
			std::basic_istringstream< CharT, Traits, Allocator > stream( input );
			
			Output output;
			
			stream >> std::boolalpha >> output;
			
			if ( stream.snextc() != Traits::eof() )
			{
				throw conversion_from_string_failed();
			}
			
			return output; 
		}
	};
	
}

#endif
