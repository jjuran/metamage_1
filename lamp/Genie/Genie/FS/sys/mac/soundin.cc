/*
	Genie/FS/sys/mac/soundin.cc
	---------------------------
*/

#include "Genie/FS/sys/mac/soundin.hh"

// Standard C++
#include <algorithm>

// gear
#include "gear/inscribe_decimal.hh"
#include "gear/parse_decimal.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/Sound.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"

// Genie
#include "Genie/FS/basic_directory.hh"
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/FSTree_Directory.hh"
#include "Genie/FS/FSTree_Generated.hh"
#include "Genie/FS/FSTree_IconSuite.hh"
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/property.hh"
#include "Genie/Utilities/canonical_positive_integer.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	static UInt16 GetKeyFromParent( const FSTree* parent )
	{
		return gear::parse_unsigned_decimal( parent->Name().c_str() );
	}
	
	
	static bool index_is_valid_soundin_device( int i )
	{
		if ( UInt16( i ) != i )
		{
			return false;
		}
		
		try
		{
			(void) N::SPBGetIndexedDevice( i );
		}
		catch ( const Mac::OSStatus& err )
		{
			if ( err != siBadSoundInDevice )
			{
				throw;
			}
			
			return false;
		}
		
		return true;
	}
	
	struct valid_name_of_soundin_device
	{
		typedef canonical_positive_integer well_formed_name;
		
		static bool applies( const plus::string& name )
		{
			return well_formed_name::applies( name )  &&  index_is_valid_soundin_device( gear::parse_unsigned_decimal( name.c_str() ) );
		}
	};
	
	extern const premapped::mapping sys_mac_soundin_REF_Mappings[];
	
	static FSTreePtr soundin_Lookup( const FSTreePtr& parent, const plus::string& name )
	{
		if ( !valid_name_of_soundin_device::applies( name ) )
		{
			poseven::throw_errno( ENOENT );
		}
		
		return Premapped_Factory( parent, name, sys_mac_soundin_REF_Mappings );
	}
	
	class soundin_IteratorGenerator
	{
		private:
			std::size_t its_index;
		
		public:
			soundin_IteratorGenerator() : its_index()
			{
			}
			
			vfs::dir_entry operator()()
			{
				const ino_t inode = ++its_index;
				
				plus::string name = gear::inscribe_decimal( its_index );
				
				return vfs::dir_entry( inode, name );
			}
	};
	
	template < class Sequence >
	static inline std::size_t distance( const Sequence& sequence )
	{
		return std::distance( sequence.begin(), sequence.end() );
	}
	
	static inline std::size_t Count_SoundInputDevices()
	{
		return distance( N::SoundInputDevice_Names() );
	}
	
	static void soundin_Iterate( const FSTreePtr& parent, vfs::dir_contents& cache )
	{
		const std::size_t n = Count_SoundInputDevices();
		
		std::generate_n( std::back_inserter( cache ),
		                 n,
		                 soundin_IteratorGenerator() );
	}
	
	
	static plus::string string_from_stringhandle( N::Handle h )
	{
		N::Str255 result;
		
		if ( std::size_t size = N::GetHandleSize( h ) )
		{
			unsigned const char* str = (StringPtr) *h.Get();
			
			if ( 1 + str[0] > size )
			{
				// corruption; shouldn't happen
				result[0] = size - 1;
				
				std::copy( &str[1], &str[ size ], &result[1] );
			}
			else
			{
				result = str;
			}
		}
		
		return plus::string( result );
	}
	
	struct sys_mac_soundin_REF_name : readonly_property
	{
		static void get( plus::var_string& result, const FSTree* that, bool binary )
		{
			const UInt16 index = GetKeyFromParent( that );
			
			const N::Str255 name = N::SPBGetIndexedDevice_Name( index );
			
			result = name;
		}
	};
	
	struct sys_mac_soundin_N_icon
	{
		static plus::string Get( const FSTree* parent, const plus::string& name )
		{
			const UInt16 index = GetKeyFromParent( parent );
			
			N::SPBGetIndexedDevice_Result device = N::SPBGetIndexedDevice( index );
			
			N::Handle icon = device.get().deviceIconHandle;
			
			const std::size_t size = N::GetHandleSize( icon );
			
			if ( size == 0 )
			{
				p7::throw_errno( ENOENT );
			}
			
			plus::string result;
			
			char* p = result.reset( size );
			
			std::memcpy( p, *icon.Get(), size );
			
			return result;
		}
	};
	
	#define PROPERTY( prop )  &new_property, &property_params_factory< prop >::value
	
	const premapped::mapping sys_mac_soundin_REF_Mappings[] =
	{
		{ "name", PROPERTY( sys_mac_soundin_REF_name ) },
		
		{ "icon", &new_generated, (void*) &sys_mac_soundin_N_icon::Get },
		
		{ NULL, NULL }
	};
	
	FSTreePtr New_FSTree_sys_mac_soundin( const FSTree*        parent,
	                                      const plus::string&  name,
	                                      const void*          args )
	{
		return new_basic_directory( parent, name, soundin_Lookup, soundin_Iterate );
	}
	
}

