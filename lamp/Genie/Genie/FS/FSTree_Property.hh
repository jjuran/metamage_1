/*	==================
 *	FSTree_Property.hh
 *	==================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_PROPERTY_HH
#define GENIE_FILESYSTEM_FSTREE_PROPERTY_HH

// POSIX
#include <sys/stat.h>

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	typedef plus::string (*Property_ReadHook)( const FSTree* that, bool binary );
	
	typedef void (*Property_WriteHook)( const FSTree  *that,
	                                    const char    *begin,
	                                    const char    *end,
	                                    bool           binary );
	
	class FSTree_Property : public FSTree
	{
		public:
			typedef Property_ReadHook   ReadHook;
			typedef Property_WriteHook  WriteHook;
		
		private:
			size_t     itsSize;
			ReadHook   itsReadHook;
			WriteHook  itsWriteHook;
		
		public:
			struct Undefined {};
			
			FSTree_Property( const FSTreePtr&     parent,
			                 const plus::string&  name,
			                 size_t               size,
			                 ReadHook             readHook,
			                 WriteHook            writeHook = NULL )
			:
				FSTree( parent, name ),
				itsSize( size ),
				itsReadHook ( readHook  ),
				itsWriteHook( writeHook )
			{
			}
			
			bool Exists() const;
			
			mode_t FilePermMode() const
			{
				return (itsReadHook  ? S_IRUSR : 0)
				     | (itsWriteHook ? S_IWUSR : 0);
			}
			
			off_t GetEOF() const;
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
		
		private:
			IOHandle* OpenForRead( OpenFlags flags ) const;
			
			IOHandle* OpenForWrite( OpenFlags flags ) const;
	};
	
	FSTreePtr New_FSTree_Property( const FSTreePtr&     parent,
	                               const plus::string&  name,
	                               size_t               size,
	                               Property_ReadHook    readHook,
	                               Property_WriteHook   writeHook = NULL );
	
	inline FSTreePtr New_FSTree_Property( const FSTreePtr&     parent,
	                                      const plus::string&  name,
	                                      Property_ReadHook    readHook,
	                                      Property_WriteHook   writeHook = NULL )
	{
		return New_FSTree_Property( parent, name, 0, readHook, writeHook );
	}
	
}

#endif

