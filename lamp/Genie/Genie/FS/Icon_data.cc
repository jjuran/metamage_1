/*	============
 *	Icon_data.cc
 *	============
 */

#include "Genie/FS/Icon_data.hh"

// POSIX
#include <fcntl.h>
#include <sys/stat.h>

// more-libc
#include "more/string.h"

// mac-glue-utils
#include "mac_glue/Memory.hh"

// mac-sys-utils
#include "mac_sys/has/IconUtilities.hh"

// mac-qd-utils
#include "mac_qd/plot_icon_id.hh"

// gear
#include "gear/inscribe_decimal.hh"
#include "gear/parse_decimal.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#ifndef NITROGEN_MACMEMORY_HH
#include "Nitrogen/MacMemory.hh"
#endif

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/methods/bstore_method_set.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/stream_method_set.hh"
#include "vfs/filehandle/primitives/get_file.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/file_method_set.hh"
#include "vfs/methods/node_method_set.hh"

// relix-kernel
#include "relix/config/color.hh"

// Genie
#include "Genie/FS/Views.hh"


enum
{
	sizeof_plain_icon  = 128,
	sizeof_masked_icon = 256,
};

namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	template < class T >
	static inline T min( T a, T b )
	{
		return b < a ? b : a;
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
	
	void IconData::Plot( const Rect&        area,
	                     IconAlignmentType  align,
	                     IconTransformType  transform )
	{
		OSErr err;
		
		if ( !itIsSet )
		{
			return;
		}
		
		if ( itsRef == NULL )
		{
			if ( ! mac::sys::has_IconUtilities() )
			{
				mac::qd::plot_icon_id( area, itsResID );
				return;
			}
			
			err = ::PlotIconID( &area, align, transform, itsResID );
			
			if ( err == noErr )
			{
				return;
			}
			
			// No such icon family resource, try a cicn
			
		#if CONFIG_COLOR
			
			if ( CIconHandle cicon = ::GetCIcon( itsResID ) )
			{
				err = PlotCIconHandle( &area, align, transform, cicon );
				
				DisposeCIcon( cicon );
				
				if ( err == noErr )
				{
					return;
				}
			}
			
			// No such color icon, try an ICON
			
		#endif
			
			if ( Handle icon = ::GetIcon( itsResID ) )
			{
				err = PlotIconHandle( &area, align, transform, icon );
				
				return;
			}
			
			// No such icon, give up
			return;
		}
		
		const ssize_t size = mac::glue::GetHandleSize_raw( GetHandle() );
		
		switch ( size )
		{
			case sizeof_plain_icon:
			case sizeof_masked_icon:
				PlotIconHandle( &area, align, transform, GetHandle() );
				break;
			
			case 0:
			default:
				// size may be negative, if the handle is somehow invalid
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
		
		return mac::glue::GetHandleSize( h );
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
		
		const Size size = use_handle ? mac::glue::GetHandleSize_raw( h )
		                             : sizeof (::ResID);
		
		if ( size <= 0 )
		{
			p7::throw_errno( EIO );
		}
		
		if ( mark >= size )
		{
			return 0;
		}
		
		n_bytes = min< size_t >( n_bytes, size - mark );
		
		const char* p = use_handle ? *h
		                           : (const char*) &itsResID;
		
		mempcpy( buffer, p + mark, n_bytes );
		
		return n_bytes;
	}
	
	ssize_t IconData::Write( const char* buffer, std::size_t n_bytes )
	{
		if ( n_bytes == sizeof (::ResID) )
		{
			Destroy();
			
			mempcpy( &itsResID, buffer, sizeof (::ResID) );
			
			itIsSet = true;
			
			return sizeof (::ResID);
		}
		
		if ( n_bytes != sizeof_plain_icon  &&  n_bytes != sizeof_masked_icon )
		{
			p7::throw_errno( EINVAL );
		}
		
		if ( !itIsPOD )
		{
			Destroy();
		}
		
		Handle h = GetHandle();
		
		if ( h == NULL )
		{
			if ( (h = NewHandle( n_bytes )) )
			{
				itsRef     = h;
				itsDeleter = dispose_handle;
				
				itIsSet = true;
				itIsPOD = true;
			}
		}
		else
		{
			SetHandleSize( h, n_bytes );
		}
		
		N::MemError();
		
		char* p = *h;
		
		mempcpy( p, buffer, n_bytes );
		
		return n_bytes;
	}
	
	
	struct icon_data_extra
	{
		IconData* data;
	};
	
	static
	void dispose_icon_data( vfs::filehandle* that )
	{
		icon_data_extra& extra = *(icon_data_extra*) that->extra();
		
		intrusive_ptr_release( extra.data );
	}
	
	
	static
	ssize_t icon_data_pread( vfs::filehandle*  that,
	                         char*             buffer,
	                         size_t            n,
	                         off_t             offset );
	
	static off_t icon_data_geteof( vfs::filehandle* file )
	{
		icon_data_extra& extra = *(icon_data_extra*) file->extra();
		
		return extra.data->GetSize();
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
	
	
	static
	vfs::filehandle* new_data_reader( const vfs::node&  file,
	                                  int               flags,
	                                  IconData*         data )
	{
		ASSERT( data != NULL );
		
		vfs::filehandle* result = new vfs::filehandle( &file,
		                                               flags,
		                                               &icon_data_filehandle_methods,
		                                               sizeof (icon_data_extra),
		                                               &dispose_icon_data );
		
		icon_data_extra& extra = *(icon_data_extra*) result->extra();
		
		extra.data = data;
		
		intrusive_ptr_add_ref( data );
		
		return result;
	}
	
	
	static
	ssize_t icondatawriter_write( vfs::filehandle*  that,
	                              const char*       buffer,
	                              size_t            n );
	
	static const vfs::stream_method_set icondatawriter_stream_methods =
	{
		NULL,
		NULL,
		&icondatawriter_write,
	};
	
	static const vfs::filehandle_method_set icondatawriter_methods =
	{
		NULL,
		NULL,
		&icondatawriter_stream_methods,
	};
	
	
	static
	vfs::filehandle* new_data_writer( const vfs::node&  file,
	                                  int               flags,
	                                  IconData*         data )
	{
		ASSERT( data != NULL );
		
		vfs::filehandle* result = new vfs::filehandle( &file,
		                                               flags,
		                                               &icondatawriter_methods,
		                                               sizeof (icon_data_extra),
		                                               &dispose_icon_data );
		
		icon_data_extra& extra = *(icon_data_extra*) result->extra();
		
		extra.data = data;
		
		intrusive_ptr_add_ref( data );
		
		return result;
	}
	
	static
	ssize_t icon_data_pread( vfs::filehandle*  that,
	                         char*             buffer,
	                         size_t            n,
	                         off_t             offset )
	{
		icon_data_extra& extra = *(icon_data_extra*) that->extra();
		
		ASSERT( extra.data != NULL );
		
		ssize_t bytes_read = extra.data->Read( buffer, n, offset );
		
		if ( bytes_read == sizeof (::ResID) )
		{
			short resID = 0;
			
			mempcpy( &resID, buffer, sizeof (::ResID) );
			
			char decimal[ sizeof "-12345\n" ];
			
			char* end = gear::inscribe_decimal_r( resID, decimal );
			
			*end++ = '\n';
			
			 bytes_read = end - decimal;
			
			if ( bytes_read > n )
			{
				// Here's a nickel, kid.  Get yourself a larger buffer.
				p7::throw_errno( ERANGE );
			}
			
			mempcpy( buffer, decimal, bytes_read );
		}
		
		return bytes_read;
	}
	
	static inline
	const vfs::node* view_key( vfs::filehandle* that )
	{
		return get_file( *that )->owner();
	}
	
	static
	ssize_t icondatawriter_write( vfs::filehandle*  that,
	                              const char*       buffer,
	                              size_t            n )
	{
		icon_data_extra& extra = *(icon_data_extra*) that->extra();
		
		ASSERT( extra.data != NULL );
		
		SInt16 resID;
		
		size_t actual_byte_count = n;
		
		if ( n >= 2  && n <= 7 )
		{
			resID = gear::parse_decimal( buffer );
			
			buffer = (const char*) &resID;
			
			actual_byte_count = sizeof (::ResID);
		}
		
		extra.data->Write( buffer, actual_byte_count );
		
		const vfs::node* view = view_key( that );
		
		InvalidateWindowForView( view );
		
		return n;
	}
	
	
	static void dispose_icon_data( const vfs::node* that )
	{
		icon_data_extra& extra = *(icon_data_extra*) that->extra();
		
		intrusive_ptr_release( extra.data );
	}
	
	
	static vfs::filehandle_ptr icon_data_open( const vfs::node* that, int flags, mode_t mode )
	{
		icon_data_extra& extra = *(icon_data_extra*) that->extra();
		
		const int accmode = flags & O_ACCMODE;
		
		vfs::filehandle* result = NULL;
		
		switch ( accmode )
		{
			case O_RDONLY:
				result = new_data_reader( *that, flags, extra.data );
				break;
			
			case O_WRONLY:
				result = new_data_writer( *that, flags, extra.data );
				break;
			
			default:
				p7::throw_errno( EACCES );
		}
		
		return result;
	}
	
	static off_t icon_data_geteof( const vfs::node* that )
	{
		icon_data_extra& extra = *(icon_data_extra*) that->extra();
		
		return extra.data->GetSize();
	}
	
	static const vfs::data_method_set icon_data_data_methods =
	{
		&icon_data_open,
		&icon_data_geteof
	};
	
	static const vfs::node_method_set icon_data_methods =
	{
		NULL,
		&icon_data_data_methods,
	};
	
	
	vfs::node_ptr New_FSTree_Icon_data( const vfs::node*     parent,
	                                    const plus::string&  name,
	                                    IconData*            data )
	{
		ASSERT( data != NULL );
		
		vfs::node* result = new vfs::node( parent,
		                                   name,
		                                   S_IFREG | 0600,
		                                   &icon_data_methods,
		                                   sizeof (icon_data_extra),
		                                   &dispose_icon_data );
		
		icon_data_extra& extra = *(icon_data_extra*) result->extra();
		
		intrusive_ptr_add_ref( data );
		
		extra.data = data;
		
		return result;
	}
	
}
