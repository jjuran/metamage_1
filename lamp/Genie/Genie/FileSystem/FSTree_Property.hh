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
			
			typedef Key (*KeyHook)( const FSTree* that );
			
			typedef std::string (*ReadHook)( Key key );
			
			typedef void (*WriteHook)( Key          key,
			                           const char  *begin,
			                           const char  *end );
			
			KeyHook    itsKeyHook;
			ReadHook   itsReadHook;
			WriteHook  itsWriteHook;
		
		public:
			struct Undefined {};
			
			template < class Key >
			FSTree_Property( const FSTreePtr&      parent,
			                 const std::string&    name,
			                 Key                 (*keyHook  )( const FSTree*    ),
			                 std::string         (*readHook )( Key          key ),
			                 void                (*writeHook)( Key          key,
			                                                   const char  *begin,
			                                                   const char  *end ) )
			:
				FSTree( parent, name ),
				itsKeyHook  ( (KeyHook  ) keyHook   ),
				itsReadHook ( (ReadHook ) readHook  ),
				itsWriteHook( (WriteHook) writeHook )
			{
			}
			
			template < class Key >
			FSTree_Property( const FSTreePtr&      parent,
			                 const std::string&    name,
			                 Key                 (*keyHook  )( const FSTree*    ),
			                 std::string         (*readHook )( Key          key ) )
			:
				FSTree( parent, name ),
				itsKeyHook  ( (KeyHook  ) keyHook  ),
				itsReadHook ( (ReadHook ) readHook ),
				itsWriteHook( (WriteHook) NULL     )
			{
			}
			
			FSTree_Property( const FSTreePtr&    parent,
			                 const std::string&  name,
			                 ReadHook            readHook,
			                 WriteHook           writeHook )
			:
				FSTree( parent, name ),
				itsKeyHook  ( &GetKey   ),
				itsReadHook ( readHook  ),
				itsWriteHook( writeHook )
			{
			}
			
			static Key GetKey( const FSTree* that )  { return that->Parent().get(); }
			
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

