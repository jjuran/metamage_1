/*
	gui/new/port.hh
	---------------
*/

#ifndef GENIE_FS_GUI_NEW_PORT_HH
#define GENIE_FS_GUI_NEW_PORT_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	class FSTree_new_port : public FSTree
	{
		public:
			FSTree_new_port( const FSTreePtr&     parent,
			                 const plus::string&  name );
			
			IOPtr ChangeToDirectory() const;
	};
	
}

#endif

