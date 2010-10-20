/*
	sys/type.cc
	-----------
*/

#include "Genie/FS/sys/type.hh"

// iota
#include "iota/quad.hh"

// plus
#include "plus/quad.hh"
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FileSignature.hh"
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/premapped.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	struct sys_type_text_wildcard
	{
		static void get( plus::var_string& result, const FSTree* that, bool binary )
		{
			result = plus::encode_quad( gTextFileCreator );
		}
		
		static void set( const FSTree* that, const char* begin, const char* end, bool binary )
		{
			const size_t length = end - begin;
			
			if ( length != sizeof gTextFileCreator )
			{
				throw p7::errno_t( EINVAL );
			}
			
			gTextFileCreator = iota::decode_quad( begin );
		}
	};
	
	template < class Property >
	static FSTreePtr Property_Factory( const FSTreePtr&     parent,
	                                   const plus::string&  name,
	                                   const void*          args )
	{
		return New_FSTree_Property( parent,
		                            name,
		                            sizeof gTextFileCreator,
		                            &Property::get,
		                            &Property::set );
	}
	
	extern const FSTree_Premapped::Mapping sys_type_text_Mappings[] =
	{
		{ "DEFAULT", &Property_Factory< sys_type_text_wildcard > },
		
		{ NULL, NULL }
	};
	
	#define PREMAPPED( map )  &premapped_factory, (const void*) map
	
	const FSTree_Premapped::Mapping sys_type_Mappings[] =
	{
		{ "text", PREMAPPED( sys_type_text_Mappings ) },
		
		{ NULL, NULL }
		
	};
	
}

