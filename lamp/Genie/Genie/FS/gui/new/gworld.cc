/*
	gui/new/gworld.cc
	-----------------
*/

#include "Genie/FS/gui/new/gworld.hh"

// POSIX
#include <sys/stat.h>

// Standard C++
#include <algorithm>

// plus
#include "plus/serialize.hh"

// nucleus
#include "nucleus/saved.hh"
#include "nucleus/shared.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/QDOffscreen.hh"

// Pedestal
#include "Pedestal/View.hh"

// Genie
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/serialize_qd.hh"
#include "Genie/FS/Views.hh"
#include "Genie/FS/data_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/IO/Handle.hh"
#include "Genie/Utilities/simple_map.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	struct GWorld_Parameters
	{
		short                           depth;
		bool                            bounds_are_valid;
		Rect                            bounds;
		
		n::shared< GWorldPtr >          gworld;
	};
	
	typedef simple_map< const FSTree*, GWorld_Parameters > GWorldMap;
	
	static GWorldMap gGWorldMap;
	
	
	static unsigned PixMap_n_bytes( PixMapHandle pix_h )
	{
		const PixMap& pix = **pix_h;
		
		const short n_rows = pix.bounds.bottom - pix.bounds.top;
		
		const short rowBytes = pix.rowBytes & 0x3FFF;
		
		return n_rows * rowBytes;
	}
	
	static off_t Pixels_GetEOF( const FSTree* key )
	{
		if ( GWorldPtr gworld = gGWorldMap[ key ].gworld.get() )
		{
			return PixMap_n_bytes( ::GetGWorldPixMap( gworld ) );
		}
		
		return 0;
	}
	
	class Pixels_IO : public VirtualFileHandle< RegularFileHandle >
	{
		private:
			// non-copyable
			Pixels_IO           ( const Pixels_IO& );
			Pixels_IO& operator=( const Pixels_IO& );
		
		public:
			Pixels_IO( const FSTreePtr& file, OpenFlags flags )
			:
				VirtualFileHandle< RegularFileHandle >( file, flags )
			{
			}
			
			const FSTree* ViewKey();
			
			IOPtr Clone();
			
			ssize_t Positioned_Read( char* buffer, size_t n_bytes, off_t offset );
			
			ssize_t Positioned_Write( const char* buffer, size_t n_bytes, off_t offset );
			
			off_t GetEOF()  { return Pixels_GetEOF( ViewKey() ); }
			
			//void Synchronize( bool metadata );
	};
	
	const FSTree* Pixels_IO::ViewKey()
	{
		return GetFile()->ParentRef().get();
	}
	
	IOPtr Pixels_IO::Clone()
	{
		return new Pixels_IO( GetFile(), GetFlags() );
	}
	
	ssize_t Pixels_IO::Positioned_Read( char* buffer, size_t n_bytes, off_t offset )
	{
		const FSTree* view = ViewKey();
		
		GWorld_Parameters& params = gGWorldMap[ view ];
		
		GWorldPtr gworld = params.gworld.get();
		
		if ( gworld == NULL )
		{
			p7::throw_errno( EIO );
		}
		
		PixMapHandle pix = N::GetGWorldPixMap( gworld );
		
		const size_t pix_size = PixMap_n_bytes( pix );
		
		if ( offset >= pix_size )
		{
			return 0;
		}
		
		n_bytes = std::min< size_t >( n_bytes, pix_size - offset );
		
		const bool locked = ::LockPixels( pix );
		
		const char* baseAddr = pix[0]->baseAddr;
		
		memcpy( buffer, &baseAddr[ offset ], n_bytes );
		
		if ( locked )
		{
			::UnlockPixels( pix );
		}
		
		return n_bytes;
	}
	
	ssize_t Pixels_IO::Positioned_Write( const char* buffer, size_t n_bytes, off_t offset )
	{
		const FSTree* view = ViewKey();
		
		GWorld_Parameters& params = gGWorldMap[ view ];
		
		GWorldPtr gworld = params.gworld.get();
		
		if ( gworld == NULL )
		{
			p7::throw_errno( EIO );
		}
		
		PixMapHandle pix = ::GetGWorldPixMap( gworld );
		
		const size_t pix_size = PixMap_n_bytes( pix );
		
		if ( offset + n_bytes > pix_size )
		{
			if ( offset >= pix_size )
			{
				p7::throw_errno( EFAULT );
			}
			
			n_bytes = pix_size - offset;
		}
		
		const bool locked = ::LockPixels( pix );
		
		char* baseAddr = pix[0]->baseAddr;
		
		memcpy( &baseAddr[ offset ], buffer, n_bytes );
		
		if ( locked )
		{
			::UnlockPixels( pix );
		}
		
		InvalidateWindowForView( view );
		
		return n_bytes;
	}
	
	
	static bool has_pixels( const FSTree* view )
	{
		if ( GWorldPtr gworld = gGWorldMap[ view ].gworld.get() )
		{
			return ::GetGWorldPixMap( gworld ) != NULL;
		}
		
		return false;
	}
	
	static off_t gworld_pixels_geteof( const FSTree* node )
	{
		return Pixels_GetEOF( node->owner() );
	}
	
	static IOPtr gworld_pixels_open( const FSTree* node, int flags, mode_t mode )
	{
		return new Pixels_IO( node, flags );
	}
	
	static const data_method_set gworld_pixels_data_methods =
	{
		&gworld_pixels_open,
		&gworld_pixels_geteof
	};
	
	static const node_method_set gworld_pixels_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&gworld_pixels_data_methods
	};
	
	static FSTreePtr gworld_pixels_factory( const FSTreePtr&     parent,
	                                        const plus::string&  name,
	                                        const void*          args )
	{
		const mode_t mode = has_pixels( parent.get() ) ? S_IFREG | 0600 : 0;
		
		return new FSTree( parent, name, mode, &gworld_pixels_methods );
	}
	
	
	class GWorld : public Ped::View
	{
		private:
			typedef const FSTree* Key;
			
			Key itsKey;
		
		public:
			GWorld( Key key ) : itsKey( key )
			{
			}
			
			void Draw( const Rect& bounds, bool erasing );
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
		
		if ( !params.bounds_are_valid )
		{
			params.bounds           = bounds;
			params.bounds_are_valid = true;
		}
		
		GWorldPtr gworld = params.gworld.get();
		
		if ( gworld == NULL )
		{
			params.gworld = N::NewGWorld( params.depth, params.bounds );
			
			gworld = params.gworld.get();
			
			Erase_GWorld( gworld, bounds );
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
	
	
	static boost::intrusive_ptr< Ped::View > CreateView( const FSTree* delegate )
	{
		GWorld_Parameters& params = gGWorldMap[ delegate ];
		
		params.depth            = 0;
		params.bounds_are_valid = false;
		
		return new GWorld( delegate );
	}
	
	
	static void DestroyDelegate( const FSTree* delegate )
	{
		gGWorldMap.erase( delegate );
	}
	
	
	static void UpdateGWorld_from_params( GWorld_Parameters& params )
	{
		if ( params.gworld.get() == NULL )
		{
			return;
		}
		
		n::owned< GWorldPtr > temp = params.gworld.unshare();
		
		N::UpdateGWorld( temp, params.depth, params.bounds );
		
		Erase_GWorld( temp, params.bounds );
		
		params.gworld = temp;
	}
	
	struct PixMap_rowBytes : plus::serialize_unsigned< short >
	{
		static const bool is_mutable = false;
		
		static short Get( PixMapHandle pix )
		{
			if ( pix == NULL )
			{
				p7::throw_errno( ENOENT );
			}
			
			return pix[0]->rowBytes & 0x3FFF;
		}
		
		static void Set( GWorld_Parameters& params, short depth );
	};
	
	struct PixMap_depth : plus::serialize_unsigned< short >
	{
		static const bool is_mutable = true;
		
		static short Get( PixMapHandle pix )
		{
			if ( pix == NULL )
			{
				p7::throw_errno( ENOENT );
			}
			
			return pix[0]->pixelSize;
		}
		
		static void Set( GWorld_Parameters& params, short depth );
	};
	
	struct PixMap_bounds : serialize_Rect
	{
		static const bool is_mutable = true;
		
		static Rect Get( PixMapHandle pix )
		{
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
		
		UpdateGWorld_from_params( params );
	}
	
	static n::shared< GWorldPtr >& get_gworldptr( const FSTree* node )
	{
		GWorld_Parameters* it = gGWorldMap.find( node );
		
		if ( it == NULL )
		{
			p7::throw_errno( ENOENT );
		}
		
		return it->gworld;
	}
	
	static PixMapHandle get_pixmap( const FSTree* that )
	{
		return ::GetGWorldPixMap( get_gworldptr( that ).get() );
	}
	
	template < class Accessor >
	struct PixMap_Property : readwrite_property
	{
		static const std::size_t fixed_size = Accessor::fixed_size;
		
		static const bool can_set = Accessor::is_mutable;
		
		typedef typename Accessor::result_type result_type;
		
		static void get( plus::var_string& result, const FSTree* that, bool binary )
		{
			PixMapHandle pix = get_pixmap( that );
			
			const result_type data = Accessor::Get( pix );
			
			Accessor::deconstruct::apply( result, data, binary );
		}
		
		static void set( const FSTree* that, const char* begin, const char* end, bool binary )
		{
			GWorld_Parameters& params = gGWorldMap[ that ];
			
			const result_type data = Accessor::reconstruct::apply( begin, end, binary );
			
			Accessor::Set( params, data );
			
			InvalidateWindowForView( that );
		}
	};
	
	#define PROPERTY( prop )  &new_property, &property_params_factory< PixMap_Property< prop > >::value
	
	static const premapped::mapping local_mappings[] =
	{
		{ "rowBytes", PROPERTY( PixMap_rowBytes ) },
		
		{ "bounds", PROPERTY( PixMap_bounds ) },
		{ "depth",  PROPERTY( PixMap_depth  ) },
		
		{ "pixels", &gworld_pixels_factory },
		
		{ NULL, NULL }
	};
	
	FSTreePtr New_FSTree_new_gworld( const FSTreePtr&     parent,
	                                 const plus::string&  name,
	                                 const void* )
	{
		return new FSTree_new_View( parent,
		                            name,
		                            &CreateView,
		                            local_mappings,
		                            &DestroyDelegate );
	}
	
}

