/*
	Genie/FS/sys/mac/proc.cc
	------------------------
*/

#include "Genie/FS/sys/mac/proc.hh"

// iota
#include "iota/hexidecimal.hh"

// plus
#include "plus/make_string.hh"
#include "plus/var_string.hh"

// Nitrogen
#include "Nitrogen/Processes.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/basic_directory.hh"
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/FSTreeCache.hh"
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/ResolvableSymLink.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	static plus::string encoded_ProcessSerialNumber( const ProcessSerialNumber& psn )
	{
		const bool extended = psn.highLongOfPSN != 0;
		
		plus::var_string name;
		
		const size_t eight = sizeof psn.highLongOfPSN * 2;
		
		const size_t more = extended ? eight + sizeof '-' : 0;
		
		char* p = name.reset( more + eight );
		
		if ( extended )
		{
			iota::encode_32_bit_hex( psn.highLongOfPSN, p );
			
			p[ eight ] = '-';
		}
		
		iota::encode_32_bit_hex( psn.lowLongOfPSN, &p[ more ] );
		
		return name;
	}
	
	
	static ProcessSerialNumber decoded_ProcessSerialNumber( const plus::string& name )
	{
		ProcessSerialNumber psn = { 0, 0 };
		
		const char* begin = name.c_str();
		const char* end   = name.c_str() + name.size();
		
		UInt32 high = 0;
		
		if ( const char* hyphen = std::strchr( begin, '-' ) )
		{
			if ( hyphen - begin != sizeof (UInt32) * 2 )
			{
				return psn;
			}
			
			high = iota::decode_32_bit_hex( begin );
			
			if ( high == 0 )
			{
				return psn;
			}
			
			begin = hyphen + 1;
		}
		
		if ( end - begin != sizeof (UInt32) * 2 )
		{
			return psn;
		}
		
		psn.highLongOfPSN = high;
		
		psn.lowLongOfPSN = iota::decode_32_bit_hex( begin );
		
		return psn;
	}
	
	
	static ProcessSerialNumber GetKeyFromParent( const FSTreePtr& parent )
	{
		return decoded_ProcessSerialNumber( parent->Name() );
	}
	
	static ProcessSerialNumber GetKey( const FSTree* that )
	{
		return GetKeyFromParent( that->ParentRef() );
	}
	
	
	static bool is_valid_ProcessSerialNumber( const ProcessSerialNumber& key )
	{
		if ( key.lowLongOfPSN == 0  &&  key.highLongOfPSN == 0 )
		{
			return false;
		}
		
		try
		{
			(void) N::GetProcessInformation( key );
		}
		catch ( const N::OSStatus& err )
		{
			if ( err != procNotFound )
			{
				throw;
			}
			
			return false;
		}
		
		return true;
	}
	
	
	extern const FSTree_Premapped::Mapping sys_mac_proc_PSN_Mappings[];
	
	static FSTreePtr psn_lookup( const FSTreePtr& parent, const plus::string& name )
	{
		const ProcessSerialNumber psn = decoded_ProcessSerialNumber( name.c_str() );
		
		const bool canonical = encoded_ProcessSerialNumber( psn ) == name;
		
		if ( !canonical  ||  !is_valid_ProcessSerialNumber( psn ) )
		{
			poseven::throw_errno( ENOENT );
		}
		
		return Premapped_Factory( parent, name, sys_mac_proc_PSN_Mappings );
	}
	
	class psn_IteratorConverter
	{
		public:
			FSNode operator()( const ProcessSerialNumber& psn ) const
			{
				const ino_t inode = 0;
				
				plus::string name = encoded_ProcessSerialNumber( psn );
				
				return FSNode( inode, name );
			}
	};
	
	static void psn_iterate( const FSTreePtr& parent, FSTreeCache& cache )
	{
		psn_IteratorConverter converter;
		
		N::Process_Container sequence = N::Processes();
		
		std::transform( sequence.begin(),
		                sequence.end(),
		                std::back_inserter( cache ),
		                converter );
	}
	
	
	class sys_mac_proc_PSN_name : public readonly_property
	{
		private:
			typedef ProcessSerialNumber Key;
		
		public:
			static void get( plus::var_string& result, const FSTree* that, bool binary )
			{
				Key key = GetKey( that );
				
				Str255 name;
				
				ProcessInfoRec processInfo;
				
				nucleus::initialize< ProcessInfoRec >( processInfo );
				
				processInfo.processName = name;
				
				N::GetProcessInformation( key, processInfo );
				
				result = plus::make_string( name );
			}
	};
	
	class FSTree_sys_mac_proc_PSN_exe : public FSTree_ResolvableSymLink
	{
		public:
			FSTree_sys_mac_proc_PSN_exe( const FSTreePtr&     parent,
			                             const plus::string&  name )
			:
				FSTree_ResolvableSymLink( parent, name )
			{
			}
			
			FSTreePtr ResolveLink() const
			{
				ProcessSerialNumber psn = GetKeyFromParent( ParentRef() );
				
				const FSSpec file = N::GetProcessAppSpec( psn );
				
				return FSTreeFromFSSpec( file, FileIsOnServer( file ) );
			}
	};
	
	
	static FSTreePtr Executable_Factory( const FSTreePtr&     parent,
	                                     const plus::string&  name,
	                                     const void*          args )
	{
		return seize_ptr( new FSTree_sys_mac_proc_PSN_exe( parent, name ) );
	}
	
	#define PROPERTY( prop )  &new_property, &property_params_factory< prop >::value
	
	const FSTree_Premapped::Mapping sys_mac_proc_PSN_Mappings[] =
	{
		{ "name", PROPERTY( sys_mac_proc_PSN_name ) },
		
		{ "exe", &Executable_Factory },
		
		{ NULL, NULL }
	};
	
	FSTreePtr New_FSTree_sys_mac_proc( const FSTreePtr&     parent,
	                                   const plus::string&  name,
	                                   const void*          args )
	{
		return new_basic_directory( parent, name, psn_lookup, psn_iterate );
	}
	
}

