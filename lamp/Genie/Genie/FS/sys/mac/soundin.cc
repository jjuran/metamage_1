/*
	Genie/FS/sys/mac/soundin.cc
	---------------------------
*/

#include "Genie/FS/sys/mac/soundin.hh"

// Standard C++
#include <algorithm>

// iota
#include "iota/decimal.hh"

// plus
#include "plus/make_string.hh"
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/Sound.hh"

// Genie
#include "Genie/FS/basic_directory.hh"
#include "Genie/FS/FSTreeCache.hh"
#include "Genie/FS/FSTree_Generated.hh"
#include "Genie/FS/FSTree_IconSuite.hh"
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/Utilities/canonical_positive_integer.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	static UInt16 GetKeyFromParent( const FSTree* parent )
	{
		return iota::parse_unsigned_decimal( parent->Name().c_str() );
	}
	
	static UInt16 GetKey( const FSTree* that )
	{
		return GetKeyFromParent( that->ParentRef().get() );
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
		catch ( const N::OSStatus& err )
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
			return well_formed_name::applies( name )  &&  index_is_valid_soundin_device( iota::parse_unsigned_decimal( name.c_str() ) );
		}
	};
	
	extern const FSTree_Premapped::Mapping sys_mac_soundin_REF_Mappings[];
	
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
			
			FSNode operator()()
			{
				const ino_t inode = ++its_index;
				
				plus::string name = iota::inscribe_decimal( its_index );
				
				return FSNode( inode, name );
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
	
	static void soundin_Iterate( const FSTreePtr& parent, FSTreeCache& cache )
	{
		const std::size_t n = Count_SoundInputDevices();
		
		std::generate_n( std::back_inserter( cache ),
		                 n,
		                 soundin_IteratorGenerator() );
	}
	
	
	static plus::string string_from_stringhandle( N::Handle h )
	{
		N::Str255 result;
		
		if ( N::Size size = N::GetHandleSize( h ) )
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
		
		return plus::make_string( result );
	}
	
	struct sys_mac_soundin_REF_name : readonly_property
	{
		static void get( plus::var_string& result, const FSTree* that, bool binary )
		{
			const UInt16 index = GetKeyFromParent( that );
			
			const N::Str255 name = N::SPBGetIndexedDevice_Name( index );
			
			result = plus::make_string( name );
		}
	};
	
	struct sys_mac_soundin_N_icon
	{
		static plus::string Get( const FSTree* that )
		{
			const UInt16 index = GetKey( that );
			
			N::SPBGetIndexedDevice_Result device = N::SPBGetIndexedDevice( index );
			
			N::Handle icon = device.get().deviceIconHandle;
			
			const N::Size size = N::GetHandleSize( icon );
			
			if ( size == 0 )
			{
				p7::throw_errno( ENOENT );
			}
			
			plus::var_string result;
			
			char* p = result.reset( size );
			
			std::memcpy( p, *icon.Get(), size );
			
			return result;
		}
	};
	
	template < class Property >
	static FSTreePtr Generated_Factory( const FSTreePtr&     parent,
	                                    const plus::string&  name,
	                                    const void*          args )
	{
		return New_FSTree_Generated( parent,
		                             name,
		                             &Property::Get );
	}
	
	#define PROPERTY( prop )  &new_property, &property_params_factory< prop >::value
	
	const FSTree_Premapped::Mapping sys_mac_soundin_REF_Mappings[] =
	{
		{ "name", PROPERTY( sys_mac_soundin_REF_name ) },
		
		{ "icon", &Generated_Factory< sys_mac_soundin_N_icon  > },
		
		{ NULL, NULL }
	};
	
	FSTreePtr New_FSTree_sys_mac_soundin( const FSTreePtr&     parent,
	                                      const plus::string&  name,
	                                      const void*          args )
	{
		return new_basic_directory( parent, name, soundin_Lookup, soundin_Iterate );
	}
	
}

