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
			
			ReadHook itsReadHook;
		
		public:
			FSTree_Property( const FSTreePtr&    parent,
			                 const std::string&  name,
			                 ReadHook            hook )
			:
				FSTree( parent, name ),
				itsReadHook( hook )
			{
			}
			
			std::string Get() const
			{
				return itsReadHook( Parent().get() );
			}
			
			off_t GetEOF() const  { return Get().size() + sizeof '\n'; }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
		
		private:
			IOHandle* OpenForRead( OpenFlags flags ) const;
			
			virtual IOHandle* OpenForWrite( OpenFlags flags ) const;
	};
	
	
	class FSTree_Mutable_Property : public FSTree_Property
	{
		private:
			typedef void (*WriteHook)( const FSTree  *view,
			                           const char    *begin,
			                           const char    *end );
			
			WriteHook itsWriteHook;
		
		public:
			FSTree_Mutable_Property( const FSTreePtr&    parent,
			                         const std::string&  name,
			                         ReadHook            readHook,
			                         WriteHook           writeHook )
			:
				FSTree_Property( parent, name, readHook ),
				itsWriteHook( writeHook )
			{
			}
			
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
		
		private:
			IOHandle* OpenForWrite( OpenFlags flags ) const;
	};
	
}

#endif

