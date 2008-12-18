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
			typedef std::string (*ReadHook)( const FSTree* view );
			
			typedef void (*WriteHook)( const FSTree  *view,
			                           const char    *begin,
			                           const char    *end );
			
			ReadHook   itsReadHook;
			WriteHook  itsWriteHook;
		
		public:
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
			
			mode_t FilePermMode() const
			{
				return (itsReadHook  ? S_IRUSR : 0)
				     | (itsWriteHook ? S_IWUSR : 0);
			}
			
			std::string Get() const
			{
				return itsReadHook( Parent().get() );
			}
			
			off_t GetEOF() const  { return itsReadHook ? Get().size() + sizeof '\n' : 0; }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
		
		private:
			IOHandle* OpenForRead( OpenFlags flags ) const;
			
			IOHandle* OpenForWrite( OpenFlags flags ) const;
	};
	
}

#endif

