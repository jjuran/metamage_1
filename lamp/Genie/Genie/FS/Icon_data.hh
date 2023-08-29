/*	============
 *	Icon_data.hh
 *	============
 */

#ifndef GENIE_FILESYSTEM_ICON_DATA_HH
#define GENIE_FILESYSTEM_ICON_DATA_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __ICONS__
#include <Icons.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// vxo
#include "vxo/ref_count.hh"

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace Genie
{
	
	class IconData : public vxo::ref_count< IconData >
	{
		private:
			typedef void (*Deleter)( void* );
			
			Deleter  itsDeleter;
			void*    itsRef;
			::ResID  itsResID;
			bool     itIsSet;
			bool     itIsPOD;
			
		private:
			// Non-copyable
			IconData           ( const IconData& );
			IconData& operator=( const IconData& );
			
		private:
			::Handle GetHandle() const  { return (::Handle) itsRef; }
		
		public:
			IconData() : itsDeleter(), itsRef(), itIsSet()
			{
			}
			
			~IconData();
			
			void Destroy();
			
			void Plot( const Rect&        area,
			           IconAlignmentType  align,
			           IconTransformType  transform );
			
			size_t GetSize() const;
			
			ssize_t Read( char* buffer, std::size_t n_bytes, off_t mark ) const;
			
			ssize_t Write( const char* buffer, std::size_t n_bytes );
	};
	
	
	vfs::node_ptr New_FSTree_Icon_data( const vfs::node*     parent,
	                                    const plus::string&  name,
	                                    IconData*            data );
	
}

#endif
