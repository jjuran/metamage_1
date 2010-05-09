/*
	sys_app_exe.cc
	--------------
	
	Copyright 2009, Joshua Juran
*/

#include "Genie/FS/sys_app_exe.hh"

// Genie
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/ResolvableSymLink.hh"
#include "Genie/Utilities/GetAppFile.hh"


namespace Genie
{
	
	class FSTree_sys_app_exe : public FSTree_ResolvableSymLink
	{
		public:
			FSTree_sys_app_exe( const FSTreePtr&     parent,
			                    const plus::string&  name )
			:
				FSTree_ResolvableSymLink( parent, name )
			{
			}
			
			FSTreePtr ResolveLink() const;
	};
	
	
	FSTreePtr FSTree_sys_app_exe::ResolveLink() const
	{
		return FSTreeFromFSSpec( GetAppFile(), false );
	}
	
	FSTreePtr New_FSTree_sys_app_exe( const FSTreePtr& parent, const plus::string& name )
	{
		return seize_ptr( new FSTree_sys_app_exe( parent, name ) );
	}
	
}

