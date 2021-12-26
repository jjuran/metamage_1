/*	============
 *	Icon_data.hh
 *	============
 */

#ifndef GENIE_FILESYSTEM_ICON_DATA_HH
#define GENIE_FILESYSTEM_ICON_DATA_HH

// vxo
#include "vxo/ref_count.hh"

// plus
#include "plus/string.hh"

// Nitrogen
#ifndef NITROGEN_ICONS_HH
#include "Nitrogen/Icons.hh"
#endif

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
			
			void SetPlainIcon( nucleus::owned< Nitrogen::Handle > h );
			
			void SetIconID( Nitrogen::ResID id );
			
			void SetIconSuite( nucleus::owned< Nitrogen::IconSuiteRef > suite );
			
			void Plot( const Rect&        area,
			           IconAlignmentType  align,
			           IconTransformType  transform );
			
			size_t GetSize() const;
			
			ssize_t Read( char* buffer, std::size_t n_bytes, off_t mark ) const;
			
			ssize_t Write( const char* buffer, std::size_t n_bytes );
	};
	
	
	vfs::node_ptr New_FSTree_Icon_data( const vfs::node*                         parent,
	                                    const plus::string&                      name,
	                                    const boost::intrusive_ptr< IconData >&  data );
	
}

#endif
