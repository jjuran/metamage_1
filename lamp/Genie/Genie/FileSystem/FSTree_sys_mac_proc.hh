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
		typedef Nitrogen::ProcessSerialNumber Key;
		
		static std::string NameFromKey( const Key& psn );
		
		static Key KeyFromName( const std::string& name );
	};
	
	struct sys_mac_proc_Details : public ProcessSerialNumber_KeyName_Traits
	{
		typedef Nitrogen::Process_Container Sequence;
		
		static const Sequence& ItemSequence()  { return Nitrogen::Processes(); }
		
		static Key KeyFromValue( const Sequence::value_type& value )  { return value; }
		
		static bool KeyIsValid( const Key& key );
		
		static FSTreePtr GetChildNode( const FSTreePtr&    parent,
		                               const std::string&  name,
		                               const Key&          key );
	};
	
	typedef FSTree_Sequence< sys_mac_proc_Details > FSTree_sys_mac_proc;
	
}

#endif

