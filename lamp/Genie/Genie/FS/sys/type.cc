/*
	sys/type.cc
	-----------
*/

#include "Genie/FS/sys/type.hh"

// iota
#include "iota/quad.hh"

// plus
#include "plus/quad.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FileSignature.hh"
#include "Genie/FS/FSTree_Property.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	struct sys_type_text_wildcard
	{
		static plus::string Read( const FSTree* that, bool binary )
		{
			return plus::encode_quad( gTextFileCreator );
		}
		
		static void Write( const FSTree* that, const char* begin, const char* end, bool binary )
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
		                            &Property::Read,
		                            &Property::Write );
	}
	
	extern const FSTree_Premapped::Mapping sys_type_text_Mappings[] =
	{
		{ "DEFAULT", &Property_Factory< sys_type_text_wildcard > },
		
		{ NULL, NULL }
	};
	
	const FSTree_Premapped::Mapping sys_type_Mappings[] =
	{
		{ "text", &Premapped_Factory< sys_type_text_Mappings > },
		
		{ NULL, NULL }
		
	};
	
}

