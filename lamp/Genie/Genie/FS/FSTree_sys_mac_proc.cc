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
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/FSTree_Directory.hh"
#include "Genie/FS/FSTree_Property.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	
	struct ProcessSerialNumber_KeyName_Traits
	{
		typedef Nitrogen::ProcessSerialNumber Key;
		
		static std::string NameFromKey( const Key& psn );
		
		static Key KeyFromName( const std::string& name );
	};
	
	struct sys_mac_proc_Details : public ProcessSerialNumber_KeyName_Traits
	{
		typedef Nitrogen::Process_Container Sequence;
		
		static Sequence ItemSequence()  { return Nitrogen::Processes(); }
		
		static Key KeyFromValue( const Sequence::value_type& value )  { return value; }
		
		static bool KeyIsValid( const Key& key );
		
		static FSTreePtr GetChildNode( const FSTreePtr&    parent,
		                               const std::string&  name,
		                               const Key&          key );
	};
	
	typedef FSTree_Sequence< sys_mac_proc_Details > FSTree_sys_mac_proc;
	
	
	static ProcessSerialNumber GetKeyFromParent( const FSTreePtr& parent )
	{
		return ProcessSerialNumber_KeyName_Traits::KeyFromName( parent->Name() );
	}
	
	static ProcessSerialNumber GetKey( const FSTree* that )
	{
		return GetKeyFromParent( that->ParentRef() );
	}
	
	
	std::string ProcessSerialNumber_KeyName_Traits::NameFromKey( const Key& psn )
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
	
	
	ProcessSerialNumber_KeyName_Traits::Key ProcessSerialNumber_KeyName_Traits::KeyFromName( const std::string& name )
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
	
	
	bool sys_mac_proc_Details::KeyIsValid( const Key& key )
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
	
	FSTreePtr sys_mac_proc_Details::GetChildNode( const FSTreePtr&    parent,
		                                          const std::string&  name,
		                                          const Key&          key )
	{
		return Premapped_Factory< sys_mac_proc_PSN_Mappings >( parent, name );
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
	
	class FSTree_sys_mac_proc_PSN_exe : public FSTree
	{
		public:
			FSTree_sys_mac_proc_PSN_exe( const FSTreePtr&    parent,
			                             const std::string&  name )
			:
				FSTree( parent, name )
			{
			}
			
			bool IsLink() const  { return true; }
			
			std::string ReadLink() const  { return ResolveLink()->Pathname(); }
			
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
		return FSTreePtr( new FSTree_sys_mac_proc( parent, name ) );
	}
	
}

