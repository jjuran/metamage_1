/*	======================
 *	FSTree_sys_mac_proc.cc
 *	======================
 */

#include "Genie/FS/FSTree_sys_mac_proc.hh"

// iota
#include "iota/hexidecimal.hh"

// Nitrogen
#include "Nitrogen/Processes.h"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/basic_directory.hh"
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/ResolvableSymLink.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	
	static std::string encoded_ProcessSerialNumber( const ProcessSerialNumber& psn )
	{
		const bool extended = psn.highLongOfPSN != 0;
		
		std::string name;
		
		const size_t eight = sizeof psn.highLongOfPSN * 2;
		
		const size_t more = extended ? eight + sizeof '-' : 0;
		
		name.resize( more + eight );
		
		if ( extended )
		{
			iota::encode_32_bit_hex( psn.highLongOfPSN, &name[0] );
			
			name[ eight ] = '-';
		}
		
		iota::encode_32_bit_hex( psn.lowLongOfPSN, &name[ more ] );
		
		return name;
	}
	
	
	static ProcessSerialNumber decoded_ProcessSerialNumber( const std::string& name )
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
	
	static FSTreePtr psn_lookup( const FSTreePtr& parent, const std::string& name )
	{
		const ProcessSerialNumber psn = decoded_ProcessSerialNumber( name.c_str() );
		
		const bool canonical = encoded_ProcessSerialNumber( psn ) == name;
		
		if ( !canonical  ||  !is_valid_ProcessSerialNumber( psn ) )
		{
			poseven::throw_errno( ENOENT );
		}
		
		return Premapped_Factory< sys_mac_proc_PSN_Mappings >( parent, name );
	}
	
	class psn_IteratorConverter
	{
		public:
			FSNode operator()( const ProcessSerialNumber& psn ) const
			{
				const ino_t inode = 0;
				
				std::string name = encoded_ProcessSerialNumber( psn );
				
				return FSNode( inode, name );
			}
	};
	
	static void psn_iterate( FSTreeCache& cache )
	{
		psn_IteratorConverter converter;
		
		N::Process_Container sequence = N::Processes();
		
		std::transform( sequence.begin(),
		                sequence.end(),
		                std::back_inserter( cache ),
		                converter );
	}
	
	
	class sys_mac_proc_PSN_name
	{
		private:
			typedef ProcessSerialNumber Key;
		
		public:
			static std::string Read( const FSTree* that, bool binary )
			{
				Key key = GetKey( that );
				
				Str255 name;
				
				ProcessInfoRec processInfo;
				
				Nucleus::Initialize< ProcessInfoRec >( processInfo );
				
				processInfo.processName = name;
				
				N::GetProcessInformation( key, processInfo );
				
				return NN::Convert< std::string >( name );
			}
	};
	
	class FSTree_sys_mac_proc_PSN_exe : public FSTree_ResolvableSymLink
	{
		public:
			FSTree_sys_mac_proc_PSN_exe( const FSTreePtr&    parent,
			                             const std::string&  name )
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
	
	
	static FSTreePtr Name_Factory( const FSTreePtr&    parent,
	                               const std::string&  name )
	{
		return New_FSTree_Property( parent,
		                            name,
		                            &sys_mac_proc_PSN_name::Read );
	}
	
	static FSTreePtr Executable_Factory( const FSTreePtr&    parent,
	                                     const std::string&  name )
	{
		return FSTreePtr( new FSTree_sys_mac_proc_PSN_exe( parent, name ) );
	}
	
	const FSTree_Premapped::Mapping sys_mac_proc_PSN_Mappings[] =
	{
		{ "name", &Name_Factory },
		
		{ "exe", &Executable_Factory },
		
		{ NULL, NULL }
	};
	
	FSTreePtr New_FSTree_sys_mac_proc( const FSTreePtr& parent, const std::string& name )
	{
		return new_basic_directory( parent, name, psn_lookup, psn_iterate );
	}
	
}

