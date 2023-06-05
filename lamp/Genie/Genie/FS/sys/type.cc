/*
	sys/type.cc
	-----------
*/

#include "Genie/FS/sys/type.hh"

// iota
#include "iota/endian.hh"

// gear
#include "gear/quad.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/property.hh"
#include "vfs/node/types/fixed_dir.hh"
#include "vfs/node/types/property_file.hh"

// Genie
#include "Genie/FileSignature.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	struct sys_type_text_wildcard
	{
		static const int fixed_size = sizeof gTextFileCreator;
		
		static void get( plus::var_string& result, const vfs::node* that, bool binary )
		{
			char* p = result.reset_nothrow( fixed_size );
			
			OSType* q = (OSType*) p;
			
			*q = iota::big_u32( gTextFileCreator );
		}
		
		static void set( const vfs::node* that, const char* begin, const char* end, bool binary )
		{
			const size_t length = end - begin;
			
			if ( length != sizeof gTextFileCreator )
			{
				throw p7::errno_t( EINVAL );
			}
			
			gTextFileCreator = gear::decode_quad( begin );
		}
	};
	
	static const vfs::property_params sys_type_text_wildcard_params =
	{
		sizeof gTextFileCreator,
		&sys_type_text_wildcard::get,
		&sys_type_text_wildcard::set,
	};
	
	#define PROPERTY( prop )  &vfs::new_property, &prop##_params
	
	extern const vfs::fixed_mapping sys_type_text_Mappings[] =
	{
		{ "DEFAULT", PROPERTY( sys_type_text_wildcard ) },
		
		{ ".~DEFAULT", PROPERTY( sys_type_text_wildcard ) },
		
		{ NULL, NULL }
	};
	
	#define PREMAPPED( map )  &vfs::fixed_dir_factory, (const void*) map
	
	const vfs::fixed_mapping sys_type_Mappings[] =
	{
		{ "text", PREMAPPED( sys_type_text_Mappings ) },
		
		{ NULL, NULL }
		
	};
	
}
