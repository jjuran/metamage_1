/*	=============
 *	FSTree_new.cc
 *	=============
 */

#include "Genie/FileSystem/FSTree_new.hh"

// Genie
#include "Genie/IO/Directory.hh"
#include "Genie/FileSystem/FSTree_sys_window.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	class FSTree_new_window : public FSTree
	{
		public:
			FSTree_new_window( const FSTreePtr&    parent,
			                   const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			bool IsDirectory() const  { return true; }
			
			boost::shared_ptr< IOHandle > OpenDirectory() const;
	};
	
	boost::shared_ptr< IOHandle > FSTree_new_window::OpenDirectory() const
	{
		FSTreePtr dir = NewWindow();
		
		return boost::shared_ptr< IOHandle >( new DirHandle( dir ) );
	}
	
	
	const Singleton_Mapping new_Mappings[] =
	{
		{ "window", &Singleton_Factory< FSTree_new_window > },
		
		{ NULL, NULL }
	};
	
}

