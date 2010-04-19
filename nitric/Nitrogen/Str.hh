// Nitrogen/Str.hh
// ---------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2008 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_STR_HH
#define NITROGEN_STR_HH

// Standard C/C++
#include <cstring>

// iota
#include "iota/string_traits.hh"

// nucleus
#ifndef NUCLEUS_CONVERT_HH
#include "nucleus/convert.hh"
#endif
#ifndef NUCLEUS_STRING_HH
#include "nucleus/string.hh"
#endif


namespace Nitrogen
{
	
	class StringTooLong {};
	
	void CopyToPascalString( const char *source,
	                         std::size_t sourceLength,
	                         unsigned char *destination,
	                         unsigned char destinationLength );
	
	template < class String >
	inline  void CopyToPascalString( const String&   source,
	                                 unsigned char*  destination,
	                                 unsigned char   destinationLength )
	{
		CopyToPascalString( iota::get_string_data( source ),
		                    iota::get_string_size( source ),
		                    destination,
		                    destinationLength );
	}
	                         
	template< unsigned char length >
	class Str
	{
		private:
			unsigned char string[ length + 1 ];
		
		public:
			Str()                                                    { string[ 0 ] = 0; }
			Str( const char* s, std::size_t slen )                   { CopyToPascalString( s, slen, string, length ); }
			
			template < class String >
			Str( const String& s )                                   { CopyToPascalString( s, string, length ); }
			
			template < class String >
			Str& operator=( const String& s )                        { CopyToPascalString( s, string, length ); return *this; } 
			
			operator unsigned char*()                                { return string; }
			operator const unsigned char *() const                   { return string; }
	};
	
	typedef Str< 255 > Str255;
	typedef Str<  63 >  Str63;
	typedef Str<  32 >  Str32;
	typedef Str<  31 >  Str31;
	typedef Str<  27 >  Str27;
	typedef Str<  15 >  Str15;
	
}

namespace iota
{
	
	template < unsigned char length >
	struct string_data< Nitrogen::Str< length > > : string_data< const unsigned char* >
	{
	};
	
	template < unsigned char length >
	struct string_size< Nitrogen::Str< length > > : string_size< const unsigned char* >
	{
	};
	
}

namespace nucleus
{
	
	template <>
	struct converter< std::string, const unsigned char * > : public std::unary_function< const unsigned char *, std::string >
	{
		std::string operator()( const unsigned char *input ) const;
	};
	
	template <>
	struct convert_input_traits<       unsigned char*          > : convert_input_traits< const unsigned char* > {};
	
	template < unsigned char length >
	struct convert_input_traits< const unsigned char[ length ] > : convert_input_traits< const unsigned char* > {};
	
#ifdef __APPLE_CC__
	
	// This must be present for gcc and absent for CodeWarrior.
	// If you're inclined to discover why this is, by all means be my guest.
	
	template < unsigned char length >
	struct convert_input_traits<       unsigned char[ length ] > : convert_input_traits< const unsigned char* > {};
	
#endif
	
	template < unsigned char length >
	struct convert_input_traits<       Nitrogen::Str< length > > : convert_input_traits< const unsigned char* > {};
	
	// Convert StringHandle to std::string
	template <>
	struct converter< std::string, unsigned char ** >: public std::unary_function< unsigned char **, std::string >
	{
		std::string operator()( unsigned char **input ) const
		{
			// We don't need to lock the handle because we copy it to the stack
			// before touching the heap, after which point we work from the copy.
			return convert< std::string >( Nitrogen::Str255( *input ) );
		}
	};
	
}

#endif

