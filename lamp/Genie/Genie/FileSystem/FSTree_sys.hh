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
	
	class FSTree_sys : public FSTree_Functional_Singleton
	{
		public:
			FSTree_sys( const FSTreePtr& parent ) : FSTree_Functional_Singleton( parent )
			{
			}
			
			void Init();
			
			std::string Name() const  { return "sys"; }
			
			FSTreePtr Parent() const  { return FSRoot(); }
	};
	
	FSTreePtr Get_sys_mac_vol_N( Nitrogen::FSVolumeRefNum vRefNum );
}

#endif

