// Nitrogen/UTFStrings.hh
// ----------------------

// Written by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_UTFSTRINGS_HH
#define NITROGEN_UTFSTRINGS_HH

// Standard C++
#include <string>

// Mac OS
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// nucleus
#include "nucleus/scribe.hh"


namespace Nitrogen
{
	
	using ::UnicodeScalarValue;
	using ::UTF32Char;
	using ::UniChar;
	using ::UTF16Char;
	using ::UTF8Char;
	
	typedef std::basic_string< UTF32Char > UTF32String;
	typedef std::basic_string< UTF16Char > UTF16String;
	typedef std::basic_string< UTF8Char  > UTF8String;
	typedef std::basic_string< UniChar   > UniString;
	
	template < class UTFChar >
	struct UnicodeFlattener
	{
		typedef const std::basic_string< UTFChar >& Put_Parameter;
		
		template < class Putter >
		static void Put( Put_Parameter toPut, Putter put )
		{
			const UTFChar* begin = &toPut[0];
			const std::size_t size = toPut.size() * sizeof (UTFChar);
			
			put( begin, begin + size );
		}
		
		typedef std::basic_string< UTFChar > Get_Result;
		
		template < class Getter >
		static Get_Result Get( Getter get )
		{
			const std::size_t size = get.size();
			Get_Result result;
			
			result.resize( size / sizeof (UTFChar) );
			
			UTFChar* begin = &result[0];
			
			get( begin, begin + size );
			
			return result;
		}
		
		typedef Put_Parameter Parameter;
		typedef Get_Result    Result;
		
		static const bool hasStaticSize = false;
		struct Buffer {};
	};
	
}

#endif
