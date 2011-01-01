/*
	nucleus/string_scribe.hh
	------------------------
*/

#ifndef NUCLEUS_STRINGSCRIBE_HH
#define NUCLEUS_STRINGSCRIBE_HH

// plus
#ifndef PLUS_VARSTRING_HH
#include "plus/var_string.hh"
#endif

// nucleus
#ifndef NUCLEUS_SCRIBE_HH
#include "nucleus/scribe.hh"
#endif
#ifndef NUCLEUS_STRING_HH
#include "nucleus/string.hh"
#endif


namespace nucleus
{
	
	template <>
	struct POD_vector_scribe< plus::string >
	{
		typedef const plus::string& Put_Parameter;
		
		template < class Putter >
		static void Put( Put_Parameter param, const Putter& putter )
		{
			const char* data = param.data();
			
			putter( data, data + param.size() );
		}
		
		typedef plus::string Get_Result;
		
		template < class Getter >
		static Get_Result Get( const Getter& getter )
		{
			const std::size_t size = getter.size();
			
			plus::var_string result;
			
			char* p = result.reset( size );
			
			getter( p, p + size );
			
			return result;
		}
		
		typedef Put_Parameter Parameter;
		typedef Get_Result  Result;
		
		static const bool hasStaticSize = false;
		struct Buffer {};
	};
	
	template <>
	struct POD_vector_scribe< plus::var_string > : POD_vector_scribe< plus::string >
	{
	};
	
	typedef POD_vector_scribe< string > string_scribe;
	
}

#endif

