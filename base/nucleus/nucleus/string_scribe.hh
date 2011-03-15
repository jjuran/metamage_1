/*
	nucleus/string_scribe.hh
	------------------------
*/

#ifndef NUCLEUS_STRINGSCRIBE_HH
#define NUCLEUS_STRINGSCRIBE_HH

// plus
#ifndef PLUS_VARSTRING_HH
#include "plus/string.hh"
#endif

// nucleus
#ifndef NUCLEUS_SCRIBE_HH
#include "nucleus/scribe.hh"
#endif
#ifndef NUCLEUS_STRING_HH
#include "nucleus/string.hh"
#endif


namespace plus
{
	
	class var_string;
	
}

namespace nucleus
{
	
	template <>
	struct POD_vector_scribe< plus::string >
	{
		typedef const plus::string&  argument_type;
		typedef       plus::string   result_type;
		
		template < class Putter >
		static void Put( argument_type param, const Putter& putter )
		{
			const char* data = param.data();
			
			putter( data, data + param.size() );
		}
		
		template < class Getter >
		static result_type Get( const Getter& getter )
		{
			const std::size_t size = getter.size();
			
			plus::string result;
			
			char* p = result.reset( size );
			
			getter( p, p + size );
			
			return result;
		}
		
		static const std::size_t static_size = 0;
	};
	
	template <>
	struct POD_vector_scribe< plus::var_string > : POD_vector_scribe< plus::string >
	{
	};
	
	typedef POD_vector_scribe< string > string_scribe;
	
}

#endif

