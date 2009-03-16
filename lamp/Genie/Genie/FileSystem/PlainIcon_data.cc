/*	=================
 *	PlainIcon_data.cc
 *	=================
 */

#include "Genie/FileSystem/PlainIcon_data.hh"

// POSIX
#include <fcntl.h>

// POSeven
#include "POSeven/Errno.hh"

// Nitrogen
#include "Nitrogen/Icons.h"

// Genie
#include "Genie/IO/VirtualFile.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	
	class IconDataFileHandle : public VirtualFileHandle
	{
		private:
			N::Handle itsData;
		
		public:
			IconDataFileHandle( const FSTreePtr&  file,
			                    OpenFlags         flags,
			                    N::Handle         data )
			:
				VirtualFileHandle( file, flags ),
				itsData( data )
			{
			}
			
			boost::shared_ptr< IOHandle > Clone();
			
			const FSTree* ViewKey() const;
			
			ssize_t SysRead( char* buffer, std::size_t byteCount );
			
			ssize_t SysWrite( const char* buffer, std::size_t byteCount );
			
			off_t GetEOF() const  { return N::GetHandleSize( itsData ); }
			
			void SetEOF( off_t length )  {}
	};
	
	boost::shared_ptr< IOHandle > IconDataFileHandle::Clone()
	{
		return boost::shared_ptr< IOHandle >( new IconDataFileHandle( GetFile(), GetFlags(), itsData ) );
	}
	
	const FSTree* IconDataFileHandle::ViewKey() const
	{
		return GetFile()->ParentRef().get();
	}
	
	ssize_t IconDataFileHandle::SysRead( char* buffer, std::size_t byteCount )
	{
		ASSERT( itsData != NULL );
		
		const std::size_t size = N::GetHandleSize( itsData );
		
		if ( size == 0 )
		{
			p7::throw_errno( EIO );
		}
		
		ASSERT( GetFileMark() <= size );
		
		byteCount = std::min( byteCount, size - GetFileMark() );
		
		char* p = *itsData.Get();
		
		std::copy( p + GetFileMark(),
		           p + GetFileMark() + byteCount,
		           buffer );
		
		return Advance( byteCount );
	}
	
	ssize_t IconDataFileHandle::SysWrite( const char* buffer, std::size_t byteCount )
	{
		if ( byteCount != sizeof (N::PlainIcon)  &&  byteCount != sizeof (N::MaskedIcon) )
		{
			p7::throw_errno( EINVAL );
		}
		
		ASSERT( itsData != NULL );
		
		N::SetHandleSize( itsData, byteCount );
		
		char* p = *itsData.Get();
		
		std::copy( buffer,
		           buffer + byteCount,
		           p );
		
		const FSTree* view = ViewKey();
		
		InvalidateWindowForView( view );
		
		// We ignore the file mark
		
		//return Advance( byteCount );
		return byteCount;
	}
	
	
	FSTree_Icon_data::FSTree_Icon_data( const FSTreePtr&                parent,
	                                    const std::string&              name,
	                                    const NN::Shared< N::Handle >&  data )
	:
		FSTree( parent, name ),
		itsData( data )
	{
		ASSERT( data.Get() != NULL );
	}
	
	off_t FSTree_Icon_data::GetEOF() const
	{
		return N::GetHandleSize( itsData );
	}
	
	boost::shared_ptr< IOHandle > FSTree_Icon_data::Open( OpenFlags flags ) const
	{
		IOHandle* result = new IconDataFileHandle( Self(), flags, itsData );
		
		return boost::shared_ptr< IOHandle >( result );
	}
	
}

