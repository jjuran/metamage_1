/*	=============
 *	FSTree_sys.hh
 *	=============
 */

#ifndef GENIE_FILESYSTEM_FSTREE_SYS_HH
#define GENIE_FILESYSTEM_FSTREE_SYS_HH

// Nitrogen
#include "Nitrogen/Files.h"

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"


namespace Genie
{
	
	class FSTree_sys : public FSTree_Functional< void >
	{
		public:
			FSTree_sys( const FSTreePtr&    parent,
			            const std::string&  name ) : FSTree_Functional< void >( parent, name )
			{
			}
			
			void Init();
	};
	
}

#endif

