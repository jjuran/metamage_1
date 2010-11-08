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
#include "Genie/FS/property.hh"


namespace Genie
{
	
	typedef property_get_hook Property_ReadHook;
	typedef property_set_hook Property_WriteHook;
	
	
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
	
	
	// Can be used in premapped directory maps
	
	FSTreePtr new_property( const FSTreePtr&     parent,
	                        const plus::string&  name,
	                        const void*          params );
	
}

#endif

