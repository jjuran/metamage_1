/*
	Genie/FS/sys/app/dir.cc
	-----------------------
*/

#include "Genie/FS/sys/app/dir.hh"

// Genie
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/ResolvableSymLink.hh"
#include "Genie/Utilities/GetAppFolder.hh"


namespace Genie
{
	
	class FSTree_sys_app_dir : public FSTree_ResolvableSymLink
	{
		public:
			FSTree_sys_app_dir( const FSTreePtr&     parent,
			                    const plus::string&  name )
			:
				FSTree_ResolvableSymLink( parent, name )
			{
			}
			
			FSTreePtr ResolveLink() const;
	};
	
	
	FSTreePtr FSTree_sys_app_dir::ResolveLink() const
	{
		return FSTreeFromFSDirSpec( GetAppFolder(), false );
	}
	
	FSTreePtr New_FSTree_sys_app_dir( const FSTreePtr&     parent,
	                                  const plus::string&  name,
	                                  const void*          args )
	{
		return seize_ptr( new FSTree_sys_app_dir( parent, name ) );
	}
	
}

