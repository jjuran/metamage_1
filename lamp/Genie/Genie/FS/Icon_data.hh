/*	============
 *	Icon_data.hh
 *	============
 */

#ifndef GENIE_FILESYSTEM_ICON_DATA_HH
#define GENIE_FILESYSTEM_ICON_DATA_HH

// POSIX
#include <sys/stat.h>

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// plus
#include "plus/ref_count.hh"

// Nitrogen
#ifndef NITROGEN_ICONS_HH
#include "Nitrogen/Icons.hh"
#endif

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	class IconData : public plus::ref_count< IconData >
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
			
			void Plot( const Rect&                  area,
			           Nitrogen::IconAlignmentType  align,
			           Nitrogen::IconTransformType  transform );
			
			size_t GetSize() const;
			
			ssize_t Read( char* buffer, std::size_t n_bytes, off_t mark ) const;
			
			ssize_t Write( const char* buffer, std::size_t n_bytes );
	};
	
	
	class FSTree_Icon_data : public FSTree
	{
		private:
			boost::intrusive_ptr< IconData > itsData;
		
		public:
			FSTree_Icon_data( const FSTreePtr&                         parent,
			                  const plus::string&                      name,
			                  const boost::intrusive_ptr< IconData >&  data );
			
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			off_t GetEOF() const;
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
			
			void Attach( const FSTreePtr& target ) const;
	};
	
}

#endif

