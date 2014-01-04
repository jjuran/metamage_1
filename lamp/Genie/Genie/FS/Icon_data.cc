/*	============
 *	Icon_data.cc
 *	============
 */

#include "Genie/FS/Icon_data.hh"

// POSIX
#include <fcntl.h>
#include <sys/stat.h>

// Standard C++
#include <algorithm>

// gear
#include "gear/inscribe_decimal.hh"

// gear
#include "gear/parse_decimal.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#ifndef MAC_TOOLBOX_TYPES_OSSTATUS_HH
#include "Mac/Toolbox/Types/OSStatus.hh"
#endif

#include "Nitrogen/Icons.hh"

// vfs
#include "vfs/filehandle/methods/bstore_method_set.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"

// relix-kernel
#include "relix/config/color.hh"
#include "relix/config/iconsuites.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/FSTree_IconSuite.hh"
#include "Genie/FS/Views.hh"
#include "Genie/FS/data_method_set.hh"
#include "Genie/FS/file_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/IO/RegularFile.hh"
#include "Genie/IO/Stream.hh"
#include "Genie/Utilities/Copy_IconSuite.hh"


#ifndef O_BINARY
#define O_BINARY  0
#endif


namespace Nitrogen
{
	
	static void PlotIconHandle( const Rect&        area,
	                            IconAlignmentType  align,
	                            IconTransformType  transform,
	                            Handle             icon )
	{
		Mac::ThrowOSStatus( ::PlotIconHandle( &area, align, transform, icon ) );
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
			catch ( const Mac::OSStatus& err )
			{
				// No such icon family resource, try a cicn
			}
			
		#if CONFIG_COLOR
			
			try
			{
				N::PlotCIconHandle( area,
				                    align,
				                    transform,
				                    N::GetCIcon( resID ) );
				
				return;
			}
			catch ( const Mac::OSStatus& err )
			{
				// No such color icon, try an ICON
			}
			
		#endif
			
			try
			{
				N::PlotIconHandle( area,
				                   align,
				                   transform,
				                   N::GetIcon( resID ) );
				
				return;
			}
			catch ( const Mac::OSStatus& err )
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
			
		#if CONFIG_ICONSUITES
			
			case 76:
				N::PlotIconSuite( area,
				                  align,
				                  transform,
				                  N::IconSuiteRef( GetHandle() ) );
				break;
			
		#endif
			
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
	
	
	class IconDataFileHandle : public RegularFileHandle
	{
		private:
			boost::intrusive_ptr< IconData > itsData;
		
		public:
			IconDataFileHandle( const FSTreePtr&                         file,
			                    int                                      flags,
			                    const boost::intrusive_ptr< IconData >&  data );
			
			ssize_t Positioned_Read( char* buffer, size_t n_bytes, off_t offset );
			
			off_t GetEOF()  { return itsData->GetSize(); }
	};
	
	
	static ssize_t icon_data_pread( vfs::filehandle* file, char* buffer, size_t n, off_t offset )
	{
		return static_cast< IconDataFileHandle& >( *file ).Positioned_Read( buffer, n, offset );
	}
	
	static off_t icon_data_geteof( vfs::filehandle* file )
	{
		return static_cast< IconDataFileHandle& >( *file ).GetEOF();
	}
	
	static const vfs::bstore_method_set icon_data_bstore_methods =
	{
		&icon_data_pread,
		&icon_data_geteof,
	};
	
	static const vfs::filehandle_method_set icon_data_filehandle_methods =
	{
		&icon_data_bstore_methods,
	};
	
	
	IconDataFileHandle::IconDataFileHandle( const FSTreePtr&                         file,
	                                        int                                      flags,
	                                        const boost::intrusive_ptr< IconData >&  data )
	:
		RegularFileHandle( file, flags, &icon_data_filehandle_methods ),
		itsData( data )
	{
		ASSERT( itsData.get() != NULL );
	}
	
	
	class IconDataWriterHandle : public StreamHandle
	{
		private:
			boost::intrusive_ptr< IconData > itsData;
		
		public:
			IconDataWriterHandle( const vfs::node&                         file,
			                      int                                      flags,
			                      const boost::intrusive_ptr< IconData >&  data )
			:
				StreamHandle( &file, flags ),
				itsData( data )
			{
				ASSERT( itsData.get() != NULL );
			}
			
			const FSTree* ViewKey();
			
			ssize_t SysWrite( const char* buffer, size_t n_bytes );
	};
	
	ssize_t IconDataFileHandle::Positioned_Read( char* buffer, size_t byteCount, off_t offset )
	{
		ASSERT( itsData.get() != NULL );
		
		ssize_t bytes_read = itsData->Read( buffer, byteCount, offset );
		
		if ( bytes_read == sizeof (::ResID)  &&  (get_flags() & O_BINARY) == 0 )
		{
			short resID = 0;
			
			std::memcpy( &resID, buffer, sizeof (::ResID) );
			
			char decimal[ sizeof "-12345\n" ];
			
			char* end = gear::inscribe_decimal_r( resID, decimal );
			
			*end++ = '\n';
			
			 bytes_read = end - decimal;
			
			if ( bytes_read > byteCount )
			{
				// Here's a nickel, kid.  Get yourself a larger buffer.
				p7::throw_errno( ERANGE );
			}
			
			std::copy( decimal, end, buffer );
		}
		
		return bytes_read;
	}
	
	const FSTree* IconDataWriterHandle::ViewKey()
	{
		return GetFile()->owner();
	}
	
	ssize_t IconDataWriterHandle::SysWrite( const char* buffer, size_t byteCount )
	{
		ASSERT( itsData.get() != NULL );
		
		SInt16 resID;
		
		size_t actual_byte_count = byteCount;
		
		if ( (get_flags() & O_BINARY) == 0  &&  byteCount >= 2  && byteCount <= 7 )
		{
			resID = gear::parse_decimal( buffer );
			
			buffer = (const char*) &resID;
			
			actual_byte_count = sizeof (::ResID);
		}
		
		itsData->Write( buffer, actual_byte_count );
		
		const FSTree* view = ViewKey();
		
		InvalidateWindowForView( view );
		
		return byteCount;
	}
	
	
	struct icon_data_extra
	{
		IconData* data;
	};
	
	static void dispose_icon_data( const FSTree* that )
	{
		icon_data_extra& extra = *(icon_data_extra*) that->extra();
		
		intrusive_ptr_release( extra.data );
	}
	
	
	static IOPtr icon_data_open( const FSTree* that, int flags, mode_t mode )
	{
		icon_data_extra& extra = *(icon_data_extra*) that->extra();
		
		const int accmode = flags & O_ACCMODE;
		
		IOHandle* result = NULL;
		
		switch ( accmode )
		{
			case O_RDONLY:
				result = new IconDataFileHandle( that, flags, extra.data );
				break;
			
			case O_WRONLY:
				result = new IconDataWriterHandle( *that, flags, extra.data );
				break;
			
			default:
				p7::throw_errno( EACCES );
		}
		
		return IOPtr( result );
	}
	
	static off_t icon_data_geteof( const FSTree* that )
	{
		icon_data_extra& extra = *(icon_data_extra*) that->extra();
		
		return extra.data->GetSize();
	}
	
	static void icon_data_attach( const FSTree* that, const FSTree* target )
	{
	#if CONFIG_ICONSUITES
		
		icon_data_extra& extra = *(icon_data_extra*) that->extra();
		
		extra.data->SetIconSuite( Copy_IconSuite( Fetch_IconSuite() ) );
		
		InvalidateWindowForView( that->owner() );
		
	#else
		
		// FIXME:  Support monochrome icons
		p7::throw_errno( ENOSYS );
		
	#endif
	}
	
	static const data_method_set icon_data_data_methods =
	{
		&icon_data_open,
		&icon_data_geteof
	};
	
	static const file_method_set icon_data_file_methods =
	{
		&icon_data_attach
	};
	
	static const node_method_set icon_data_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&icon_data_data_methods,
		NULL,
		NULL,
		&icon_data_file_methods
	};
	
	
	FSTreePtr New_FSTree_Icon_data( const FSTree*                            parent,
	                                const plus::string&                      name,
	                                const boost::intrusive_ptr< IconData >&  data )
	{
		ASSERT( data.get() != NULL );
		
		FSTree* result = new FSTree( parent,
		                             name,
		                             S_IFREG | 0600,
		                             &icon_data_methods,
		                             sizeof (icon_data_extra),
		                             &dispose_icon_data );
		
		icon_data_extra& extra = *(icon_data_extra*) result->extra();
		
		intrusive_ptr_add_ref( data.get() );
		
		extra.data = data.get();
		
		return result;
	}
	
}

