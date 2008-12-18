/*	==================
 *	FSTree_Property.hh
 *	==================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_PROPERTYFILE_HH
#define GENIE_FILESYSTEM_FSTREE_PROPERTYFILE_HH

// POSIX
#include <sys/stat.h>

// Genie
#include "Genie/FileSystem/FSTree.hh"


namespace Genie
{
	
	class FSTree_Property : public FSTree
	{
		private:
			typedef const FSTree* Key;
			
			typedef std::string (*ReadHook)( Key key );
			
			typedef void (*WriteHook)( Key          key,
			                           const char  *begin,
			                           const char  *end );
			
			ReadHook   itsReadHook;
			WriteHook  itsWriteHook;
		
		public:
			struct Undefined {};
			
			FSTree_Property( const FSTreePtr&    parent,
			                 const std::string&  name,
			                 ReadHook            readHook,
			                 WriteHook           writeHook )
			:
				FSTree( parent, name ),
				itsReadHook ( readHook  ),
				itsWriteHook( writeHook )
			{
			}
			
			Key GetKey() const  { return Parent().get(); }
			
			mode_t FilePermMode() const
			{
				return (itsReadHook  ? S_IRUSR : 0)
				     | (itsWriteHook ? S_IWUSR : 0);
			}
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
		
		private:
			IOHandle* OpenForRead( OpenFlags flags ) const;
			
			IOHandle* OpenForWrite( OpenFlags flags ) const;
	};
	
}

#endif

