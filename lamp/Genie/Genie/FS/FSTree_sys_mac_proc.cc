/*	======================
 *	FSTree_sys_mac_proc.cc
 *	======================
 */

#include "Genie/FS/FSTree_sys_mac_proc.hh"

// Nitrogen
#include "Nitrogen/Processes.h"

// Genie
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/FSTree_Directory.hh"
#include "Genie/FS/FSTree_Property.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
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
		using BitsAndBytes::EncodeAsHex;
		
		std::string name = EncodeAsHex( psn.lowLongOfPSN );
		
		if ( psn.highLongOfPSN != 0 )
		{
			name = EncodeAsHex( psn.highLongOfPSN ) + "-" + name;
		}
		
		return name;
	}
	
	ProcessSerialNumber_KeyName_Traits::Key ProcessSerialNumber_KeyName_Traits::KeyFromName( const std::string& name )
	{
		ProcessSerialNumber psn = { 0, 0 };
		
		const char* begin = name.c_str();
		const char* end   = name.c_str() + name.size();
		
		if ( const char* hyphen = std::strchr( begin, '-' ) )
		{
			psn.highLongOfPSN = DecodeHex32( begin, hyphen );
			
			begin = hyphen + 1;
		}
		
		psn.lowLongOfPSN = DecodeHex32( begin, end );
		
		return psn;
	}
	
	
	bool sys_mac_proc_Details::KeyIsValid( const Key& key )
	{
		try
		{
			(void) N::GetProcessInformation( key );
		}
		catch ( const N::ProcNotFound& err )
		{
		#ifdef __MWERKS__
			
			if ( err.Get() != procNotFound )
			{
				throw;
			}
			
		#endif
			
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
			                             const std::string&  name ) : FSTree( parent, name )
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

