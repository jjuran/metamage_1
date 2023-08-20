/*
	gui/new/gworld.cc
	-----------------
*/

#include "Genie/FS/gui/new/gworld.hh"

// POSIX
#include <sys/stat.h>

// more-libc
#include "more/string.h"

// mac-qd-utils
#include "mac_qd/copy_bits.hh"
#include "mac_qd/get_pix_rowBytes.hh"
#include "mac_qd/get_portRect.hh"
#include "mac_qd/globals/thePort.hh"
#include "mac_qd/scoped_GWorld.hh"

// plus
#include "plus/serialize.hh"
#include "plus/simple_map.hh"

// nucleus
#include "nucleus/shared.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/QDOffscreen.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/methods/bstore_method_set.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/primitives/get_file.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/node/types/property_file.hh"

// Pedestal
#include "Pedestal/View.hh"

// relix-kernel
#include "relix/config/gui_new_gworld.hh"

// Genie
#include "Genie/FS/serialize_qd.hh"
#include "Genie/FS/Views.hh"
#include "Genie/Utilities/HIQuickDraw.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	using mac::qd::get_pix_rowBytes;
	
	
	template < class T >
	static inline T min( T a, T b )
	{
		return b < a ? b : a;
	}
	
	
	struct GWorld_Parameters
	{
		uint16_t                        stride;
		uint8_t                         depth;
		bool                            grayscale;
		bool                            alpha_last;
		bool                            little_endian;
		bool                            bounds_are_valid;
		Rect                            bounds;
		
		n::shared< GWorldPtr >          gworld;
	};
	
	typedef plus::simple_map< const vfs::node*, GWorld_Parameters > GWorldMap;
	
#if CONFIG_GUI_NEW_GWORLD
	
	static GWorldMap gGWorldMap;
	
#else
	
	extern GWorldMap gGWorldMap;
	
#endif
	
	
	static
	const vfs::node* pixmap_data_view_key( vfs::filehandle* that );
	
	static
	unsigned PixMap_n_bytes( PixMapHandle pix_h, short stride )
	{
		const PixMap& pix = **pix_h;
		
		const short n_rows = pix.bounds.bottom - pix.bounds.top;
		
		const short rowBytes = get_pix_rowBytes( pix_h );
		
		return n_rows * (stride ? stride : rowBytes);
	}
	
	static off_t Pixels_GetEOF( const vfs::node* key )
	{
		GWorld_Parameters& params = gGWorldMap[ key ];
		
		if ( GWorldPtr gworld = params.gworld.get() )
		{
			return PixMap_n_bytes( ::GetGWorldPixMap( gworld ), params.stride );
		}
		
		return 0;
	}
	
	
	static
	ssize_t pixels_pread( vfs::filehandle*  that,
	                      char*             buffer,
	                      size_t            n_bytes,
	                      off_t             offset );
	
	static off_t pixels_geteof( vfs::filehandle* file )
	{
		return Pixels_GetEOF( pixmap_data_view_key( file ) );
	}
	
	static
	ssize_t pixels_pwrite( vfs::filehandle*  that,
	                       const char*       buffer,
	                       size_t            n_bytes,
	                       off_t             offset );
	
	static const vfs::bstore_method_set pixels_bstore_methods =
	{
		&pixels_pread,
		&pixels_geteof,
		&pixels_pwrite,
	};
	
	static const vfs::filehandle_method_set pixels_methods =
	{
		&pixels_bstore_methods,
	};
	
	
	static
	const vfs::node* pixmap_data_view_key( vfs::filehandle* that )
	{
		return get_file( *that )->owner();
	}
	
	static
	void check_stride( const PixMap& pix, short stride )
	{
		const short n_cols = pix.bounds.right - pix.bounds.left;
		
		const short actual_row_bytes = (n_cols * pix.pixelSize + 7) / 8;
		
		if ( stride < actual_row_bytes )
		{
			p7::throw_errno( EIO );
		}
	}
	
	static
	ssize_t pixels_pread( vfs::filehandle*  that,
	                      char*             buffer,
	                      size_t            n_bytes,
	                      off_t             offset )
	{
		const vfs::node* view = pixmap_data_view_key( that );
		
		GWorld_Parameters& params = gGWorldMap[ view ];
		
		GWorldPtr gworld = params.gworld.get();
		
		if ( gworld == NULL )
		{
			p7::throw_errno( EIO );
		}
		
		PixMapHandle pix = N::GetGWorldPixMap( gworld );
		
		const size_t pix_size = PixMap_n_bytes( pix, params.stride );
		
		if ( offset >= pix_size )
		{
			return 0;
		}
		
		const short rowBytes = get_pix_rowBytes( pix );
		
		const short stride = params.stride ? params.stride : rowBytes;
		
		check_stride( **pix, stride );
		
		n_bytes = min< size_t >( n_bytes, pix_size - offset );
		
		const size_t n_read = n_bytes;
		
		const bool locked = ::LockPixels( pix );
		
		const char* baseAddr = pix[0]->baseAddr;
		
		short nth_row = offset / stride;
		
		if ( off_t row_offset = offset % stride )
		{
			const off_t base_offset = nth_row * rowBytes + row_offset;
			
			const size_t n = stride - row_offset;
			
			buffer = (char*) mempcpy( buffer, &baseAddr[ base_offset ], n );
			
			offset  += n;
			n_bytes -= n;
			
			++nth_row;
		}
		
		while ( n_bytes >= stride )
		{
			const char* src = &baseAddr[ nth_row * rowBytes ];
			
			buffer = (char*) mempcpy( buffer, src, stride );
			
			offset  += stride;
			n_bytes -= stride;
			
			++nth_row;
		}
		
		if ( n_bytes > 0 )
		{
			mempcpy( buffer, &baseAddr[ nth_row * rowBytes ], n_bytes );
		}
		
		if ( locked )
		{
			::UnlockPixels( pix );
		}
		
		return n_read;
	}
	
	static
	ssize_t pixels_pwrite( vfs::filehandle*  that,
	                       const char*       buffer,
	                       size_t            n_bytes,
	                       off_t             offset )
	{
		const vfs::node* view = pixmap_data_view_key( that );
		
		GWorld_Parameters& params = gGWorldMap[ view ];
		
		GWorldPtr gworld = params.gworld.get();
		
		if ( gworld == NULL )
		{
			p7::throw_errno( EIO );
		}
		
		PixMapHandle pix = ::GetGWorldPixMap( gworld );
		
		const size_t pix_size = PixMap_n_bytes( pix, params.stride );
		
		if ( offset >= pix_size )
		{
			p7::throw_errno( EFAULT );
		}
		
		if ( n_bytes > pix_size - offset )
		{
			n_bytes = pix_size - offset;
		}
		
		const short rowBytes = get_pix_rowBytes( pix );
		
		const short stride = params.stride ? params.stride : rowBytes;
		
		check_stride( **pix, stride );
		
		const size_t n_written = n_bytes;
		
		const bool locked = ::LockPixels( pix );
		
		char* baseAddr = pix[0]->baseAddr;
		
		short nth_row = offset / stride;
		
		if ( off_t row_offset = offset % stride )
		{
			const off_t base_offset = nth_row * rowBytes + row_offset;
			
			const size_t n = stride - row_offset;
			
			mempcpy( &baseAddr[ base_offset ], buffer, n );
			
			offset  += n;
			buffer  += n;
			n_bytes -= n;
			
			++nth_row;
		}
		
		while ( n_bytes >= stride )
		{
			mempcpy( &baseAddr[ nth_row * rowBytes ], buffer, stride );
			
			offset  += stride;
			buffer  += stride;
			n_bytes -= stride;
			
			++nth_row;
		}
		
		if ( n_bytes > 0 )
		{
			mempcpy( &baseAddr[ nth_row * rowBytes ], buffer, n_bytes );
		}
		
		if ( locked )
		{
			::UnlockPixels( pix );
		}
		
		InvalidateWindowForView( view );
		
		return n_written;
	}
	
	
	static bool has_pixels( const vfs::node* view )
	{
		if ( GWorldPtr gworld = gGWorldMap[ view ].gworld.get() )
		{
			return ::GetGWorldPixMap( gworld ) != NULL;
		}
		
		return false;
	}
	
	static off_t gworld_pixels_geteof( const vfs::node* that )
	{
		return Pixels_GetEOF( that->owner() );
	}
	
	static vfs::filehandle_ptr gworld_pixels_open( const vfs::node* that, int flags, mode_t mode )
	{
		return new vfs::filehandle( that, flags, &pixels_methods );
	}
	
	static const vfs::data_method_set gworld_pixels_data_methods =
	{
		&gworld_pixels_open,
		&gworld_pixels_geteof
	};
	
	static const vfs::node_method_set gworld_pixels_methods =
	{
		NULL,
		&gworld_pixels_data_methods
	};
	
	static vfs::node_ptr gworld_pixels_factory( const vfs::node*     parent,
	                                            const plus::string&  name,
	                                            const void*          args )
	{
		const mode_t mode = has_pixels( parent ) ? S_IFREG | 0600 : 0;
		
		return new vfs::node( parent, name, mode, &gworld_pixels_methods );
	}
	
	
	class GWorld : public Ped::View
	{
		private:
			typedef const vfs::node* Key;
			
			Key itsKey;
		
		public:
			GWorld( Key key ) : itsKey( key )
			{
			}
			
			void Draw( const Rect& bounds, bool erasing );
			
			void DrawInContext( CGContextRef context, CGRect bounds );
	};
	
	static void Erase_GWorld( GWorldPtr gworld, const Rect& bounds )
	{
		mac::qd::scoped_GWorld scoped( gworld );
		
		EraseRect( &bounds );
	}
	
	void GWorld::Draw( const Rect& bounds, bool erasing )
	{
		GWorld_Parameters& params = gGWorldMap[ itsKey ];
		
		GWorldPtr gworld = params.gworld.get();
		
		if ( gworld == NULL )
		{
			return;
		}
		
		PixMapHandle pix = N::GetGWorldPixMap( gworld );
		
		if ( const bool locked = ::LockPixels( pix ) )
		{
			// Copy to dest
			mac::qd::copy_bits( gworld,
			                    mac::qd::thePort(),
			                    mac::qd::get_portRect( gworld ),
			                    bounds );
			
			::UnlockPixels( pix );
		}
	}
	
	void GWorld::DrawInContext( CGContextRef context, CGRect bounds )
	{
		GWorld_Parameters& params = gGWorldMap[ itsKey ];
		
		if ( GWorldPtr gworld = params.gworld.get() )
		{
			HIViewDrawGWorld( context, bounds, gworld );
		}
	}
	
	
	static boost::intrusive_ptr< Ped::View > CreateView( const vfs::node* delegate )
	{
		GWorld_Parameters& params = gGWorldMap[ delegate ];
		
		params.stride           = 0;
		params.depth            = 0;
		params.grayscale        = 0;
		
	#ifdef MAC_OS_X_VERSION_10_4
		
		params.alpha_last       = 0;
		params.little_endian    = 0;
		
	#endif
		
		params.bounds_are_valid = false;
		
		return new GWorld( delegate );
	}
	
	
	static void DestroyDelegate( const vfs::node* delegate )
	{
		gGWorldMap.erase( delegate );
	}
	
	
	static void UpdateGWorld_from_params( GWorld_Parameters& params )
	{
		if ( ! params.bounds_are_valid )
		{
			return;
		}
		
		/*
			We need to distinguish between 1/5/5/5 and 5/6/5.  To do so,
			we'll redefine "depth" to mean the number of bits that contribute
			to the number of viewable colors.  Alpha modifies which colors are
			displayed (when it's considered at all) but doesn't add to the
			gamut, so 1/5/5/5 is only 15 bits deep.  Its "weight" remains 16.
		*/
		
		short weight = params.depth;
		
		if ( weight == 15 )
		{
			weight = 16;
		}
		else if ( weight == 24 )
		{
			weight = 32;
		}
		
		/*
			Use a grayscale palette at indexed bit depths if requested.
		*/
		
		n::owned< CTabHandle > colorTable;
		
		short clut_id = weight;
		
		if ( params.grayscale  &&  (weight == 4  ||  weight == 8) )
		{
			clut_id += 32;
		}
		
		if ( weight > 1  &&  weight <= 8 )
		{
			colorTable = N::GetCTable( clut_id );
		}
		
		n::owned< GWorldPtr > temp = params.gworld.unshare();
		
		if ( temp.get() != NULL )
		{
			N::UpdateGWorld( temp, weight, params.bounds, colorTable );
		}
		else
		{
			temp = N::NewGWorld( weight, params.bounds, colorTable );
		}
		
		Erase_GWorld( temp, params.bounds );
		
	#ifdef MAC_OS_X_VERSION_10_4
		
		/*
			Try to handle little-endian image data.  This works for creating a
			CGImage in 10.4 and later.  It also works with CopyBits() on x86,
			but not on PPC.
			
			TODO:  Manually byte-swap pixels when the OS won't do it.
		*/
		
		if ( params.little_endian )
		{
			PixMapHandle pix = GetGWorldPixMap( temp );
			
			short weight = pix[0]->pixelSize;
			
			if ( weight == 16 )
			{
				pix[0]->pixelFormat = params.depth == 15 ? k16LE555PixelFormat
				                                         : k16LE565PixelFormat;
			}
			else if ( weight == 32 )
			{
				pix[0]->pixelFormat = k32BGRAPixelFormat;
			}
		}
		else if ( params.alpha_last )
		{
			PixMapHandle pix = GetGWorldPixMap( temp );
			
			pix[0]->pixelFormat = k32RGBAPixelFormat;
		}
		
	#endif
		
		params.gworld = temp;
	}
	
	static
	PixMapHandle get_pixmap( const GWorld_Parameters& params )
	{
		return ::GetGWorldPixMap( params.gworld.get() );
	}
	
	struct PixMap_rowBytes : plus::serialize_unsigned< short >
	{
		static const bool is_mutable = false;
		
		static short Get( const GWorld_Parameters& params )
		{
			PixMapHandle pix = get_pixmap( params );
			
			if ( pix == NULL )
			{
				p7::throw_errno( ENOENT );
			}
			
			return get_pix_rowBytes( pix );
		}
		
		static void Set( GWorld_Parameters& params, short depth );
	};
	
	struct PixMap_stride : plus::serialize_unsigned< short >
	{
		static const bool is_mutable = true;
		
		static short Get( const GWorld_Parameters& params )
		{
			return params.stride;
		}
		
		static void Set( GWorld_Parameters& params, short stride )
		{
			params.stride = stride;
		}
	};
	
	struct PixMap_depth : plus::serialize_unsigned< uint8_t >
	{
		static const bool is_mutable = true;
		
		static short Get( const GWorld_Parameters& params )
		{
			PixMapHandle pix = get_pixmap( params );
			
			if ( pix == NULL )
			{
				return params.depth;
			}
			
			return pix[0]->pixelSize;
		}
		
		static void Set( GWorld_Parameters& params, short depth );
	};
	
	struct PixMap_alpha_last : plus::serialize_unsigned< bool >
	{
		static const bool is_mutable = true;
		
		static short Get( const GWorld_Parameters& params )
		{
			return params.alpha_last;
		}
		
		static void Set( GWorld_Parameters& params, bool alpha_last )
		{
			params.alpha_last = alpha_last;
		}
	};
	
	struct PixMap_little_endian : plus::serialize_unsigned< bool >
	{
		static const bool is_mutable = true;
		
		static short Get( const GWorld_Parameters& params )
		{
			return params.little_endian;
		}
		
		static void Set( GWorld_Parameters& params, bool little_endian )
		{
			params.little_endian = little_endian;
		}
	};
	
	struct PixMap_grayscale : plus::serialize_unsigned< bool >
	{
		static const bool is_mutable = true;
		
		static bool Get( const GWorld_Parameters& params )
		{
			return params.grayscale;
		}
		
		static void Set( GWorld_Parameters& params, bool grayscale )
		{
			params.grayscale = grayscale;
			
			UpdateGWorld_from_params( params );
		}
	};
	
	struct PixMap_bounds : serialize_Rect
	{
		static const bool is_mutable = true;
		
		static Rect Get( const GWorld_Parameters& params )
		{
			PixMapHandle pix = get_pixmap( params );
			
			if ( pix == NULL )
			{
				p7::throw_errno( ENOENT );
			}
			
			return pix[0]->bounds;
		}
		
		static void Set( GWorld_Parameters& params, const Rect& bounds );
	};
	
	static
	bool is_valid_depth( short depth )
	{
		if ( depth > 32 )
		{
			return false;
		}
		
		if ( depth == 15  ||  depth == 24 )
		{
			return true;
		}
		
		return (depth | depth - 1)  ==  depth + depth - 1;
	}
	
	void PixMap_depth::Set( GWorld_Parameters& params, short depth )
	{
		if ( ! is_valid_depth( depth ) )
		{
			p7::throw_errno( EINVAL );
		}
		
		if ( !params.gworld.sole() )
		{
			p7::throw_errno( EAGAIN );
		}
		
		params.depth = depth;
		
		UpdateGWorld_from_params( params );
	}
	
	void PixMap_bounds::Set( GWorld_Parameters& params, const Rect& bounds )
	{
		if ( ::EmptyRect( &bounds ) )
		{
			p7::throw_errno( EINVAL );
		}
		
		if ( !params.gworld.sole() )
		{
			p7::throw_errno( EAGAIN );
		}
		
		params.bounds = bounds;
		params.bounds_are_valid = true;
		
		UpdateGWorld_from_params( params );
	}
	
	struct PixMap_size : serialize_Point
	{
		static const bool is_mutable = true;
		
		static Point Get( const GWorld_Parameters& params )
		{
			const Rect bounds = PixMap_bounds::Get( params );
			
			const short height = bounds.bottom - bounds.top;
			const short width  = bounds.right - bounds.left;
			
			Point point = { height, width };
			return point;
		}
		
		static void Set( GWorld_Parameters& params, const Point& size )
		{
			const Rect bounds = { 0, 0, size.v, size.h };
			
			PixMap_bounds::Set( params, bounds );
		}
	};
	
	
	static
	GWorld_Parameters& get_params( const vfs::node* that )
	{
		GWorld_Parameters* it = gGWorldMap.find( that );
		
		if ( it == NULL )
		{
			p7::throw_errno( ENOENT );
		}
		
		return *it;
	}
	
	#define DEFINE_GETTER( p )  \
	static void p##_get( plus::var_string& result, const vfs::node* that, bool binary )  \
	{  \
		GWorld_Parameters& params = get_params( that );  \
		const p::result_type data = p::Get( params );    \
		p::deconstruct::apply( result, data, binary );   \
	}
	
	#define DEFINE_SETTER( p )  \
	static void p##_set( const vfs::node* that, const char* begin, const char* end, bool binary )  \
	{  \
		GWorld_Parameters& params = gGWorldMap[ that ];                 \
		p::Set( params, p::reconstruct::apply( begin, end, binary ) );  \
		InvalidateWindowForView( that );                                \
	}
	
	#define DEFINE_PARAMS( p )  \
	static const vfs::property_params p##_params = {p::fixed_size, &p##_get, &p##_set}
	
	DEFINE_GETTER( PixMap_rowBytes      );
	DEFINE_GETTER( PixMap_bounds        );
	DEFINE_GETTER( PixMap_depth         );
	DEFINE_GETTER( PixMap_size          );
	DEFINE_GETTER( PixMap_stride        );
	DEFINE_GETTER( PixMap_grayscale     );
	DEFINE_GETTER( PixMap_alpha_last    );
	DEFINE_GETTER( PixMap_little_endian );
	
	DEFINE_SETTER( PixMap_bounds        );
	DEFINE_SETTER( PixMap_depth         );
	DEFINE_SETTER( PixMap_size          );
	DEFINE_SETTER( PixMap_stride        );
	DEFINE_SETTER( PixMap_grayscale     );
	DEFINE_SETTER( PixMap_alpha_last    );
	DEFINE_SETTER( PixMap_little_endian );
	
	static const vfs::property_params PixMap_rowBytes_params =
	{
		PixMap_rowBytes::fixed_size,
		&PixMap_rowBytes_get,
	};
	
	DEFINE_PARAMS( PixMap_bounds        );
	DEFINE_PARAMS( PixMap_depth         );
	DEFINE_PARAMS( PixMap_size          );
	DEFINE_PARAMS( PixMap_stride        );
	DEFINE_PARAMS( PixMap_grayscale     );
	DEFINE_PARAMS( PixMap_alpha_last    );
	DEFINE_PARAMS( PixMap_little_endian );
	
	#define PROPERTY( prop )  &vfs::new_property, &prop##_params
	
	static const vfs::fixed_mapping local_mappings[] =
	{
		{ "rowBytes", PROPERTY( PixMap_rowBytes ) },
		
		{ "bounds", PROPERTY( PixMap_bounds ) },
		{ "depth",  PROPERTY( PixMap_depth  ) },
		{ ".~depth", PROPERTY( PixMap_depth ) },
		{ "size",   PROPERTY( PixMap_size   ) },
		{ ".~size", PROPERTY( PixMap_size   ) },
		
		{ "stride",   PROPERTY( PixMap_stride ) },
		{ ".~stride", PROPERTY( PixMap_stride ) },
		
		{ "grayscale",   PROPERTY( PixMap_grayscale ) },
		{ ".~grayscale", PROPERTY( PixMap_grayscale ) },
		
	#ifdef MAC_OS_X_VERSION_10_4
		
		{ "alpha-last",   PROPERTY( PixMap_alpha_last ) },
		{ ".~alpha-last", PROPERTY( PixMap_alpha_last ) },
		
		{ "little-endian",   PROPERTY( PixMap_little_endian ) },
		{ ".~little-endian", PROPERTY( PixMap_little_endian ) },
		
	#endif
		
		{ "data", &gworld_pixels_factory },
		{ "pixels", &gworld_pixels_factory },
		
		{ NULL, NULL }
	};
	
	vfs::node_ptr New_FSTree_new_gworld( const vfs::node*     parent,
	                                     const plus::string&  name,
	                                     const void* )
	{
		return New_new_view( parent,
		                     name,
		                     &CreateView,
		                     local_mappings,
		                     &DestroyDelegate );
	}
	
}
