/*	======================
 *	FSTree_sys_mac_proc.hh
 *	======================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSMACPROC_HH
#define GENIE_FILESYSTEM_FSTREESYSMACPROC_HH

// Nitrogen
#include "Nitrogen/Processes.h"

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"


namespace Genie
{
	
	struct ProcessSerialNumber_KeyName_Traits
	{
		typedef N::ProcessSerialNumber Key;
		
		static std::string NameFromKey( const Key& psn );
		
		static Key KeyFromName( const std::string& name );
	};
	
	class FSTree_sys_mac_proc_PSN;
	
	struct sys_mac_proc_Details : public ProcessSerialNumber_KeyName_Traits
	{
		typedef N::Process_Container Sequence;
		
		typedef FSTree_sys_mac_proc_PSN ChildNode;
		
		static std::string Name()  { return "proc"; }
		
		static FSTreePtr Parent();
		
		static const Sequence& ItemSequence()  { return N::Processes(); }
		
		static Key KeyFromValue( const Sequence::value_type& value )  { return value; }
		
		static bool KeyIsValid( const Key& key );
	};
	
	typedef FSTree_Sequence< sys_mac_proc_Details > FSTree_sys_mac_proc;
	
	class FSTree_sys_mac_proc_PSN : public FSTree_Virtual,
	                                public ProcessSerialNumber_KeyName_Traits
	{
		private:
			Key itsKey;
		
		public:
			FSTree_sys_mac_proc_PSN( const Key& key );
			
			void Init();
			
			std::string Name() const  { return NameFromKey( itsKey ); }
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_sys_mac_proc >(); }
	};
	
}

#endif

