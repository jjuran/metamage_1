/*	============
 *	Icon_data.cc
 *	============
 */

#include "Genie/FS/Icon_data.hh"

// Standard C++
#include <algorithm>

// Iota
#include "iota/decimal.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/Icons.hh"

// Genie
#include "Genie/FS/FSTree_IconSuite.hh"
#include "Genie/FS/Views.hh"
#include "Genie/IO/RegularFile.hh"
#include "Genie/IO/VirtualFile.hh"


namespace Nitrogen
{
	
	static void PlotIconHandle( const Rect&        area,
	                            IconAlignmentType  align,
	                            IconTransformType  transform,
	                            Handle             icon )
	{
		ThrowOSStatus( ::PlotIconHandle( &area, align, transform, icon ) );
	}
	
}

namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	static void Plot_Null( const Rect&           area,
	                       N::IconAlignmentType  align,
			               N::IconTransformType  transform )
	{
		// do nothing
	}
	
	static void dispose_handle( void* h )
	{
		if ( h != NULL )
		{
			::DisposeHandle( (::Handle) h );
		}
	}
	
	static void dispose_cicon( void* h )
	{
		if ( h != NULL )
		{
			::DisposeCIcon( (::CIconHandle) h );
		}
	}
	
	static void dispose_iconsuite( void* h )
	{
		if ( h != NULL )
		{
			::DisposeIconSuite( (::Handle) h, true );
		}
	}
	
	IconData::~IconData()
	{
		Destroy();
	}
	
	void IconData::Destroy()
	{
		if ( itsDeleter && itsRef )
		{
			itsDeleter( itsRef );
			
			itsRef = NULL;
		}
	}
	
	void IconData::SetPlainIcon( n::owned< N::Handle > h )
	{
		Destroy();
		
		itsRef = h.release().Get();
		
		itsDeleter = dispose_handle;
		
		itIsSet = true;
		itIsPOD = true;
	}
	
	void IconData::SetIconID( N::ResID id )
	{
		Destroy();
		
		itsResID = id;
		
		itIsSet = true;
	}
	
	void IconData::SetIconSuite( n::owned< N::IconSuiteRef > suite )
	{
		Destroy();
		
		itsRef = suite.release().Get();
		
		itsDeleter = dispose_iconsuite;
		
		itIsSet = true;
		itIsPOD = false;
	}
	
	void IconData::Plot( const Rect&           area,
	                     N::IconAlignmentType  align,
	                     N::IconTransformType  transform )
	{
		if ( !itIsSet )
		{
			return;
		}
		
		if ( itsRef == NULL )
		{
			N::ResID resID = N::ResID( itsResID );
			
			try
			{
				N::PlotIconID( area, align, transform, resID );
				
				return;
			}
			catch ( const N::OSStatus& err )
			{
				// No such icon family resource, try a cicn
			}
			
			try
			{
				N::PlotCIconHandle( area,
				                    align,
				                    transform,
				                    N::GetCIcon( resID ) );
				
				return;
			}
			catch ( const N::OSStatus& err )
			{
				// No such color icon, try an ICON
			}
			
			try
			{
				N::PlotIconHandle( area,
				                   align,
				                   transform,
				                   N::GetIcon( resID ) );
				
				return;
			}
			catch ( const N::OSStatus& err )
			{
				// No such icon, give up
			}
			
			return;
		}
		
		const std::size_t size = N::GetHandleSize( GetHandle() );
		
		switch ( size )
		{
			case 0:
				return;
			
			case sizeof (N::PlainIcon):
			case sizeof (N::MaskedIcon):
				N::PlotIconHandle( area,
				                   align,
				                   transform,
				                   N::Handle( GetHandle() ) );
				break;
			
			case 76:
				N::PlotIconSuite( area,
				                  align,
				                  transform,
				                  N::IconSuiteRef( GetHandle() ) );
				break;
			
			default:
				break;
		}
	}
	
	size_t IconData::GetSize() const
	{
		::Handle h = GetHandle();
		
		if ( !itIsSet )
		{
			return 0;
		}
		
		if ( h == NULL )
		{
			return sizeof (::ResID);
		}
		
		return N::GetHandleSize( N::Handle( h ) );
	}
	
	ssize_t IconData::Read( char* buffer, std::size_t n_bytes, off_t mark ) const
	{
		::Handle h = GetHandle();
		
		if ( !itIsSet )
		{
			return 0;
		}
		
		const bool use_handle = h != NULL;
		
		if ( use_handle  &&  !itIsPOD )
		{
			p7::throw_errno( EPERM );
		}
		
		const std::size_t size = use_handle ? N::GetHandleSize( N::Handle( h ) )
		                                    : sizeof (::ResID);
		
		if ( size == 0 )
		{
			p7::throw_errno( EIO );
		}
		
		if ( mark >= size )
		{
			return 0;
		}
		
		n_bytes = std::min< size_t >( n_bytes, size - mark );
		
		const char* p = use_handle ? *h
		                           : (const char*) &itsResID;
		
		std::memcpy( buffer, p + mark, n_bytes );
		
		return n_bytes;
	}
	
	ssize_t IconData::Write( const char* buffer, std::size_t n_bytes )
	{
		if ( n_bytes == sizeof (::ResID) )
		{
			Destroy();
			
			std::memcpy( &itsResID, buffer, sizeof (::ResID) );
			
			itIsSet = true;
			
			return sizeof (::ResID);
		}
		
		if ( n_bytes != sizeof (N::PlainIcon)  &&  n_bytes != sizeof (N::MaskedIcon) )
		{
			p7::throw_errno( EINVAL );
		}
		
		if ( !itIsPOD )
		{
			Destroy();
		}
		
		N::Handle h = GetHandle();
		
		if ( h == NULL )
		{
			h = N::NewHandle( n_bytes ).release();
			
			itsRef     = h.Get();
			itsDeleter = dispose_handle;
			
			itIsSet = true;
			itIsPOD = true;
		}
		else
		{
			N::SetHandleSize( h, n_bytes );
		}
		
		char* p = *h.Get();
		
		std::copy( buffer,
		           buffer + n_bytes,
		           p );
		
		return n_bytes;
	}
	
	
	class IconDataFileHandle : public VirtualFileHandle< RegularFileHandle >
	{
		private:
			boost::shared_ptr< IconData > itsData;
		
		public:
			IconDataFileHandle( const FSTreePtr&                      file,
			                    OpenFlags                             flags,
			                    const boost::shared_ptr< IconData >&  data )
			:
				VirtualFileHandle< RegularFileHandle >( file, flags ),
				itsData( data )
			{
				ASSERT( itsData.get() != NULL );
			}
			
			boost::shared_ptr< IOHandle > Clone();
			
			ssize_t Positioned_Read( char* buffer, size_t n_bytes, off_t offset );
			
			off_t GetEOF()  { return itsData->GetSize(); }
	};
	
	class IconDataWriterHandle : public VirtualFileHandle< StreamHandle >
	{
		private:
			boost::shared_ptr< IconData > itsData;
		
		public:
			IconDataWriterHandle( const FSTreePtr&                      file,
			                      OpenFlags                             flags,
			                      const boost::shared_ptr< IconData >&  data )
			:
				VirtualFileHandle< StreamHandle >( file, flags ),
				itsData( data )
			{
				ASSERT( itsData.get() != NULL );
			}
			
			unsigned int SysPoll()  { return kPollRead | kPollWrite; }
			
			const FSTree* ViewKey();
			
			ssize_t SysWrite( const char* buffer, size_t n_bytes );
	};
	
	boost::shared_ptr< IOHandle > IconDataFileHandle::Clone()
	{
		return seize_ptr( new IconDataFileHandle( GetFile(), GetFlags(), itsData ) );
	}
	
	ssize_t IconDataFileHandle::Positioned_Read( char* buffer, size_t byteCount, off_t offset )
	{
		ASSERT( itsData.get() != NULL );
		
		ssize_t bytes_read = itsData->Read( buffer, byteCount, offset );
		
		if ( bytes_read == sizeof (::ResID)  &&  (GetFlags() & O_BINARY) == 0 )
		{
			short resID = 0;
			
			std::memcpy( &resID, buffer, sizeof (::ResID) );
			
			std::string result = iota::inscribe_decimal( resID );
			
			result += '\n';
			
			if ( result.length() > byteCount )
			{
				// Here's a nickel, kid.  Get yourself a larger buffer.
				p7::throw_errno( ERANGE );
			}
			
			std::copy( result.begin(), result.end(), buffer );
			
			bytes_read = result.length();
		}
		
		return bytes_read;
	}
	
	const FSTree* IconDataWriterHandle::ViewKey()
	{
		return GetFile()->ParentRef().get();
	}
	
	ssize_t IconDataWriterHandle::SysWrite( const char* buffer, size_t byteCount )
	{
		ASSERT( itsData.get() != NULL );
		
		SInt16 resID;
		
		if ( (GetFlags() & O_BINARY) == 0  &&  byteCount >= 2  && byteCount <= 7 )
		{
			resID = iota::parse_decimal( buffer );
			
			buffer = (const char*) &resID;
			
			byteCount = sizeof (::ResID);
		}
		
		itsData->Write( buffer, byteCount );
		
		const FSTree* view = ViewKey();
		
		InvalidateWindowForView( view );
		
		return byteCount;
	}
	
	
	FSTree_Icon_data::FSTree_Icon_data( const FSTreePtr&                      parent,
	                                    const std::string&                    name,
	                                    const boost::shared_ptr< IconData >&  data )
	:
		FSTree( parent, name ),
		itsData( data )
	{
		ASSERT( data.get() != NULL );
	}
	
	off_t FSTree_Icon_data::GetEOF() const
	{
		return itsData->GetSize();
	}
	
	boost::shared_ptr< IOHandle > FSTree_Icon_data::Open( OpenFlags flags ) const
	{
		const int mode = flags & O_ACCMODE;
		
		IOHandle* result = NULL;
		
		switch ( mode )
		{
			case O_RDONLY:
				result = new IconDataFileHandle( Self(), flags, itsData );
				break;
			
			case O_WRONLY:
				result = new IconDataWriterHandle( Self(), flags, itsData );
				break;
			
			default:
				p7::throw_errno( EACCES );
		}
		
		return boost::shared_ptr< IOHandle >( result );
	}
	
	static pascal OSErr CopyIconToSuite( ::ResType type, ::Handle* icon, void* userData )
	{
		::IconSuiteRef newSuite = (::IconSuiteRef) userData;
		
		::Handle copy = *icon;
		
		if ( copy == NULL )
		{
			return noErr;
		}
		
		OSErr err = ::HandToHand( &copy );
		
		if ( err != noErr )
		{
			return err;
		}
		
		err = ::AddIconToSuite( copy, newSuite, type );
		
		if ( err != noErr )
		{
			::DisposeHandle( copy );
		}
		
		return err;
	}
	
	static ::IconActionUPP gCopyIconToSuiteUPP = ::NewIconActionUPP( &CopyIconToSuite );
	
	static n::owned< N::IconSuiteRef > Copy_IconSuite( N::IconSuiteRef iconSuite )
	{
		n::owned< N::IconSuiteRef > copy = N::NewIconSuite();
		
		N::ThrowOSStatus( ::ForEachIconDo( iconSuite,
		                                   kSelectorAllAvailableData,
		                                   gCopyIconToSuiteUPP,
		                                   copy.get().Get() ) );
		
		return copy;
	}
	
	void FSTree_Icon_data::Attach( const FSTreePtr& target ) const
	{
		const FSTree* view = ParentRef().get();
		
		itsData->SetIconSuite( Copy_IconSuite( Fetch_IconSuite() ) );
		
		InvalidateWindowForView( view );
	}
	
}

