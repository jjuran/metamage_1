/*	==============
 *	FSTree_Proc.hh
 *	==============
 */

#ifndef GENIE_FILESYSTEM_FSTREE_PROC_HH
#define GENIE_FILESYSTEM_FSTREE_PROC_HH

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/Process.hh"


namespace Genie
{
	
	struct pid_KeyName_Traits : Integer_KeyName_Traits< pid_t >
	{
	};
	
	
	struct proc_Details : public pid_KeyName_Traits
	{
		typedef ProcessList::Map Sequence;
		
		static const Sequence& ItemSequence()  { return GetProcessList().GetMap(); }
		
		static Key KeyFromValue( const Sequence::value_type& value )  { return value.first; }
		
		static bool KeyIsValid( const Key& key )
		{
			const Sequence& sequence = ItemSequence();
			
			return key == 0  ||  sequence.find( key ) != sequence.end();
		}
		
		static FSTreePtr GetChildNode( const FSTreePtr&    parent,
		                               const std::string&  name,
		                               const Key&          key );
	};
	
	typedef FSTree_Sequence< proc_Details > FSTree_proc;
	
}

#endif

