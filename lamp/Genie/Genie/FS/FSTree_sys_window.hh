/*	====================
 *	FSTree_sys_window.hh
 *	====================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSWINDOW_HH
#define GENIE_FILESYSTEM_FSTREESYSWINDOW_HH

// Standard C++
#include <map>

// Genie
#include "Genie/FS/FSTree_Directory.hh"


namespace Genie
{
	
	class FSTree_sys_window : public FSTree_Directory
	{
		public:
			FSTree_sys_window( const FSTreePtr&    parent,
			                   const std::string&  name ) : FSTree_Directory( parent, name )
			{
			}
			
			FSTreePtr Lookup_Child( const std::string& name ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
	};
	
	FSTreePtr NewWindow();
	
	void RemoveWindow( const FSTree* window );
	
	
	FSTreePtr New_FSTree_sys_window( const FSTreePtr& parent, const std::string& name );
	
}

#endif

