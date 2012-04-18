/*
	sys/type.cc
	-----------
*/

#include "Genie/FS/sys/type.hh"

// gear
#include "gear/quad.hh"

// plus
#include "plus/quad.hh"
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FileSignature.hh"
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/premapped.hh"
#include "Genie/FS/property.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	struct sys_type_text_wildcard : readwrite_property
	{
		static const size_t fixed_size = sizeof gTextFileCreator;
		
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
			
			gTextFileCreator = gear::decode_quad( begin );
		}
	};
	
	#define PROPERTY( prop )  &new_property, &property_params_factory< prop >::value
	
	extern const vfs::fixed_mapping sys_type_text_Mappings[] =
	{
		{ "DEFAULT", PROPERTY( sys_type_text_wildcard ) },
		
		{ NULL, NULL }
	};
	
	#define PREMAPPED( map )  &premapped_factory, (const void*) map
	
	const vfs::fixed_mapping sys_type_Mappings[] =
	{
		{ "text", PREMAPPED( sys_type_text_Mappings ) },
		
		{ NULL, NULL }
		
	};
	
}

