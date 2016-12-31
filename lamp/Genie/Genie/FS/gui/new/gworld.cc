/*
	gui/new/gworld.cc
	-----------------
*/

#include "Genie/FS/gui/new/gworld.hh"

// POSIX
#include <sys/stat.h>

// plus
#include "plus/serialize.hh"
#include "plus/simple_map.hh"

// nucleus
#include "nucleus/saved.hh"
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
	unsigned PixMap_n_bytes( PixMapHandle pix_h, short stride )
	{
		const PixMap& pix = **pix_h;
		
		const short n_rows = pix.bounds.bottom - pix.bounds.top;
		
		const short rowBytes = pix.rowBytes & 0x3FFF;
		
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
	
	class Pixels_IO : public vfs::filehandle
	{
		private:
			// non-copyable
			Pixels_IO           ( const Pixels_IO& );
			Pixels_IO& operator=( const Pixels_IO& );
		
		public:
			Pixels_IO( const vfs::node& file, int flags );
			
			const vfs::node* ViewKey();
			
			ssize_t Positioned_Read( char* buffer, size_t n_bytes, off_t offset );
			
			ssize_t Positioned_Write( const char* buffer, size_t n_bytes, off_t offset );
			
			off_t GetEOF()  { return Pixels_GetEOF( ViewKey() ); }
			
			//void Synchronize( bool metadata );
	};
	
	
	static ssize_t pixels_pread( vfs::filehandle* file, char* buffer, size_t n, off_t offset )
	{
		return static_cast< Pixels_IO& >( *file ).Positioned_Read( buffer, n, offset );
	}
	
	static off_t pixels_geteof( vfs::filehandle* file )
	{
		return static_cast< Pixels_IO& >( *file ).GetEOF();
	}
	
	static ssize_t pixels_pwrite( vfs::filehandle* file, const char* buffer, size_t n, off_t offset )
	{
		return static_cast< Pixels_IO& >( *file ).Positioned_Write( buffer, n, offset );
	}
	
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
	
	
	Pixels_IO::Pixels_IO( const vfs::node& file, int flags )
	:
		vfs::filehandle( &file, flags, &pixels_methods )
	{
	}
	
	const vfs::node* Pixels_IO::ViewKey()
	{
		return get_file( *this )->owner();
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
	
	ssize_t Pixels_IO::Positioned_Read( char* buffer, size_t n_bytes, off_t offset )
	{
		const vfs::node* view = ViewKey();
		
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
		
		const short rowBytes = pix[0]->rowBytes & 0x3FFF;
		
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
			
			memcpy( buffer, &baseAddr[ base_offset ], n );
			
			offset  += n;
			buffer  += n;
			n_bytes -= n;
			
			++nth_row;
		}
		
		while ( n_bytes >= stride )
		{
			memcpy( buffer, &baseAddr[ nth_row * rowBytes ], stride );
			
			offset  += stride;
			buffer  += stride;
			n_bytes -= stride;
			
			++nth_row;
		}
		
		if ( n_bytes > 0 )
		{
			memcpy( buffer, &baseAddr[ nth_row * rowBytes ], n_bytes );
		}
		
		if ( locked )
		{
			::UnlockPixels( pix );
		}
		
		return n_read;
	}
	
	ssize_t Pixels_IO::Positioned_Write( const char* buffer, size_t n_bytes, off_t offset )
	{
		const vfs::node* view = ViewKey();
		
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
		
		const short rowBytes = pix[0]->rowBytes & 0x3FFF;
		
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
			
			memcpy( &baseAddr[ base_offset ], buffer, n );
			
			offset  += n;
			buffer  += n;
			n_bytes -= n;
			
			++nth_row;
		}
		
		while ( n_bytes >= stride )
		{
			memcpy( &baseAddr[ nth_row * rowBytes ], buffer, stride );
			
			offset  += stride;
			buffer  += stride;
			n_bytes -= stride;
			
			++nth_row;
		}
		
		if ( n_bytes > 0 )
		{
			memcpy( &baseAddr[ nth_row * rowBytes ], buffer, n_bytes );
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
		return new Pixels_IO( *that, flags );
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
		n::saved< N::GWorld > saved_gworld;
		
		N::SetGWorld( gworld );
		
		N::EraseRect( bounds );
	}
	
	void GWorld::Draw( const Rect& bounds, bool erasing )
	{
		const N::TransferMode mode = N::srcCopy;
		
		if ( erasing  &&  mode != N::srcCopy )
		{
			N::EraseRect( bounds );
		}
		
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
			N::CopyBits( N::GetPortBitMapForCopyBits( gworld                   ),
			             N::GetPortBitMapForCopyBits( N::GetQDGlobalsThePort() ),
			             N::GetPortBounds( gworld ),
			             bounds,
			             mode );
			
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
			Use a grayscale palette at indexed bit depths if requested.
			(Bit depth 2 already defaults to grayscale.)
		*/
		
		n::owned< CTabHandle > colorTable;
		
		if ( params.grayscale  &&  (params.depth == 4  ||  params.depth == 8) )
		{
			colorTable = N::GetCTable( params.depth + 32 );
		}
		
		n::owned< GWorldPtr > temp = params.gworld.unshare();
		
		if ( temp.get() != NULL )
		{
			N::UpdateGWorld( temp, params.depth, params.bounds, colorTable );
		}
		else
		{
			temp = N::NewGWorld( params.depth, params.bounds, colorTable );
		}
		
		Erase_GWorld( temp, params.bounds );
		
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
			
			return pix[0]->rowBytes & 0x3FFF;
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
	
	void PixMap_depth::Set( GWorld_Parameters& params, short depth )
	{
		if ( depth > 32  ||  (depth | depth - 1)  !=  depth + depth - 1 )
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
	
	template < class Accessor >
	struct PixMap_Property : vfs::readwrite_property
	{
		static const int fixed_size = Accessor::fixed_size;
		
		static const bool can_set = Accessor::is_mutable;
		
		typedef typename Accessor::result_type result_type;
		
		static void get( plus::var_string& result, const vfs::node* that, bool binary )
		{
			GWorld_Parameters& params = get_params( that );
			
			const result_type data = Accessor::Get( params );
			
			Accessor::deconstruct::apply( result, data, binary );
		}
		
		static void set( const vfs::node* that, const char* begin, const char* end, bool binary )
		{
			GWorld_Parameters& params = gGWorldMap[ that ];
			
			const result_type data = Accessor::reconstruct::apply( begin, end, binary );
			
			Accessor::Set( params, data );
			
			InvalidateWindowForView( that );
		}
	};
	
	#define PROPERTY( prop )  &vfs::new_property, &vfs::property_params_factory< PixMap_Property< prop > >::value
	
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
