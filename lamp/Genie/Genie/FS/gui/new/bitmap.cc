/*
	gui/new/bitmap.cc
	-----------------
*/

#include "Genie/FS/gui/new/bitmap.hh"

// POSIX
#include <sys/mman.h>
#include <sys/stat.h>

// plus
#include "plus/serialize.hh"
#include "plus/simple_map.hh"

// nucleus
#include "nucleus/shared.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/MacMemory.hh"
#include "Nitrogen/Quickdraw.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/methods/bstore_method_set.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/general_method_set.hh"
#include "vfs/filehandle/primitives/get_file.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/node/types/property_file.hh"

// MacVFS
#include "MacVFS/mmap/Ptr_memory_mapping.hh"

// Pedestal
#include "Pedestal/View.hh"

// Genie
#include "Genie/FS/serialize_qd.hh"
#include "Genie/FS/Views.hh"


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
	
	
	struct BitMap_Parameters
	{
		n::shared< N::Ptr >  bits;
		
		BitMap               bitmap;
	};
	
	typedef plus::simple_map< const vfs::node*, BitMap_Parameters > BitMapMap;
	
	static BitMapMap gBitMapMap;
	
	
	static unsigned BitMap_n_bytes( const BitMap& bits )
	{
		const short n_rows = bits.bounds.bottom - bits.bounds.top;
		
		const short rowBytes = bits.rowBytes & 0x3FFF;
		
		return n_rows * rowBytes;
	}
	
	static off_t Bits_GetEOF( const vfs::node* key )
	{
		return BitMap_n_bytes( gBitMapMap[ key ].bitmap );
	}
	
	class Bits_IO : public vfs::filehandle
	{
		private:
			// non-copyable
			Bits_IO           ( const Bits_IO& );
			Bits_IO& operator=( const Bits_IO& );
		
		public:
			Bits_IO( const vfs::node* file, int flags );
			
			const vfs::node* ViewKey();
			
			ssize_t Positioned_Read( char* buffer, size_t n_bytes, off_t offset );
			
			ssize_t Positioned_Write( const char* buffer, size_t n_bytes, off_t offset );
			
			off_t GetEOF()  { return Bits_GetEOF( ViewKey() ); }
			
			vfs::memory_mapping_ptr Map( size_t length, int prot, int flags, off_t offset );
			
			//void Synchronize( bool metadata );
	};
	
	
	static ssize_t bits_pread( vfs::filehandle* file, char* buffer, size_t n, off_t offset )
	{
		return static_cast< Bits_IO& >( *file ).Positioned_Read( buffer, n, offset );
	}
	
	static off_t bits_geteof( vfs::filehandle* file )
	{
		return static_cast< Bits_IO& >( *file ).GetEOF();
	}
	
	static ssize_t bits_pwrite( vfs::filehandle* file, const char* buffer, size_t n, off_t offset )
	{
		return static_cast< Bits_IO& >( *file ).Positioned_Write( buffer, n, offset );
	}
	
	static vfs::memory_mapping_ptr bits_mmap( vfs::filehandle* that, size_t length, int prot, int flags, off_t offset )
	{
		return static_cast< Bits_IO& >( *that ).Map( length, prot, flags, offset );
	}
	
	static const vfs::bstore_method_set bits_bstore_methods =
	{
		&bits_pread,
		&bits_geteof,
		&bits_pwrite,
	};
	
	static const vfs::general_method_set bits_general_methods =
	{
		&bits_mmap,
	};
	
	static const vfs::filehandle_method_set bits_methods =
	{
		&bits_bstore_methods,
		NULL,
		NULL,
		&bits_general_methods,
	};
	
	
	Bits_IO::Bits_IO( const vfs::node* file, int flags )
	:
		vfs::filehandle( file, flags, &bits_methods )
	{
	}
	
	class bits_memory_mapping : public vfs::Ptr_memory_mapping
	{
		private:
			vfs::filehandle_ptr its_file;
		
		public:
			bits_memory_mapping( const nucleus::shared< Mac::Ptr >&  p,
			                     size_t                              length,
			                     int                                 flags,
			                     vfs::filehandle&                    file,
			                     off_t                               offset );
			
			~bits_memory_mapping();
			
			void msync( void* addr, size_t len, int flags ) const;
	};
	
	bits_memory_mapping::bits_memory_mapping( const nucleus::shared< Mac::Ptr >&  p,
	                                          size_t                              length,
	                                          int                                 flags,
	                                          vfs::filehandle&                    file,
	                                          off_t                               offset )
	:
		vfs::Ptr_memory_mapping( p, length, flags, offset ),
		its_file( &file )
	{
	}
	
	bits_memory_mapping::~bits_memory_mapping()
	{
		msync( get_address(), get_size(), MS_SYNC );
	}
	
	void bits_memory_mapping::msync( void* addr, size_t len, int flags ) const
	{
		Bits_IO& file = static_cast< Bits_IO& >( *its_file.get() );
		
		InvalidateWindowForView( file.ViewKey() );
	}
	
	
	const vfs::node* Bits_IO::ViewKey()
	{
		return get_file( *this )->owner();
	}
	
	ssize_t Bits_IO::Positioned_Read( char* buffer, size_t n_bytes, off_t offset )
	{
		const vfs::node* view = ViewKey();
		
		BitMap_Parameters& params = gBitMapMap[ view ];
		
		const size_t pix_size = BitMap_n_bytes( params.bitmap );
		
		if ( offset >= pix_size )
		{
			return 0;
		}
		
		n_bytes = min< size_t >( n_bytes, pix_size - offset );
		
		const char* baseAddr = params.bitmap.baseAddr;
		
		memcpy( buffer, &baseAddr[ offset ], n_bytes );
		
		return n_bytes;
	}
	
	ssize_t Bits_IO::Positioned_Write( const char* buffer, size_t n_bytes, off_t offset )
	{
		const vfs::node* view = ViewKey();
		
		BitMap_Parameters& params = gBitMapMap[ view ];
		
		if ( params.bitmap.baseAddr == NULL )
		{
			p7::throw_errno( EIO );
		}
		
		const size_t pix_size = BitMap_n_bytes( params.bitmap );
		
		if ( offset + n_bytes > pix_size )
		{
			if ( offset >= pix_size )
			{
				p7::throw_errno( EFAULT );
			}
			
			n_bytes = pix_size - offset;
		}
		
		char* baseAddr = params.bitmap.baseAddr;
		
		memcpy( &baseAddr[ offset ], buffer, n_bytes );
		
		InvalidateWindowForView( view );
		
		return n_bytes;
	}
	
	vfs::memory_mapping_ptr
	//
	Bits_IO::Map( size_t length, int prot, int flags, off_t offset )
	{
		const vfs::node* view = ViewKey();
		
		BitMap_Parameters& params = gBitMapMap[ view ];
		
		const size_t pix_size = BitMap_n_bytes( params.bitmap );
		
		if ( offset + length > pix_size )
		{
			p7::throw_errno( ENXIO );
		}
		
		return new bits_memory_mapping( params.bits, length, flags, *this, offset );
	}
	
	
	static bool has_bits( const vfs::node* view )
	{
		return gBitMapMap[ view ].bits.get() != NULL;
	}
	
	static off_t bitmap_bits_geteof( const vfs::node* that )
	{
		return Bits_GetEOF( that->owner() );
	}
	
	static vfs::filehandle_ptr bitmap_bits_open( const vfs::node* that, int flags, mode_t mode )
	{
		return new Bits_IO( that, flags );
	}
	
	static const vfs::data_method_set bitmap_bits_data_methods =
	{
		&bitmap_bits_open,
		&bitmap_bits_geteof
	};
	
	static const vfs::node_method_set bitmap_bits_methods =
	{
		NULL,
		&bitmap_bits_data_methods
	};
	
	
	class BitMapView : public Ped::View
	{
		private:
			typedef const vfs::node* Key;
			
			Key itsKey;
		
		public:
			BitMapView( Key key ) : itsKey( key )
			{
			}
			
			void Draw( const Rect& bounds, bool erasing );
	};
	
	static inline
	short compute_rowBytes_from_width( short width )
	{
		ASSERT( width > 0 );
		
		return (width + 15) >> 3 & ~1;
	}
	
	static inline short compute_rowBytes_from_bounds( const Rect& bounds )
	{
		const short width = bounds.right - bounds.left;
		
		return compute_rowBytes_from_width( width );
	}
	
	void BitMapView::Draw( const Rect& bounds, bool erasing )
	{
		const N::TransferMode mode = N::srcCopy;
		
		if ( erasing  &&  mode != N::srcCopy )
		{
			N::EraseRect( bounds );
		}
		
		BitMap_Parameters& params = gBitMapMap[ itsKey ];
		
		if ( !params.bitmap.rowBytes )
		{
			params.bitmap.bounds = bounds;
			
			params.bitmap.rowBytes = compute_rowBytes_from_bounds( bounds );
		}
		
		if ( params.bitmap.baseAddr == NULL )
		{
			size_t n_bytes = params.bitmap.rowBytes * (bounds.bottom - bounds.top);
			
			params.bits = N::NewPtrClear( n_bytes );
			
			params.bitmap.baseAddr = params.bits.get();
		}
		
		// Copy to dest
		N::CopyBits( &params.bitmap,
		             N::GetPortBitMapForCopyBits( N::GetQDGlobalsThePort() ),
		             params.bitmap.bounds,
		             bounds,
		             mode );
	}
	
	
	static boost::intrusive_ptr< Ped::View > CreateView( const vfs::node* delegate )
	{
		BitMap_Parameters& params = gBitMapMap[ delegate ];
		
		params.bitmap.baseAddr = 0;
		params.bitmap.rowBytes = 0;
		
		params.bitmap.bounds.top    = 0;
		params.bitmap.bounds.left   = 0;
		params.bitmap.bounds.bottom = 0;
		params.bitmap.bounds.right  = 0;
		
		return new BitMapView( delegate );
	}
	
	
	static void DestroyDelegate( const vfs::node* delegate )
	{
		gBitMapMap.erase( delegate );
	}
	
	
	struct BitMap_rowBytes : plus::serialize_unsigned< short >
	{
		static const bool is_mutable = false;
		
		static short Get( const BitMap& bits )
		{
			return bits.rowBytes & 0x3FFF;
		}
		
		static void Set( BitMap_Parameters& params, short depth );
	};
	
	struct BitMap_bounds : serialize_Rect
	{
		static const bool is_mutable = true;
		
		static Rect Get( const BitMap& bits )
		{
			return bits.bounds;
		}
		
		static void Set( BitMap_Parameters& params, const Rect& bounds );
	};
	
	void BitMap_bounds::Set( BitMap_Parameters& params, const Rect& bounds )
	{
		if ( ::EmptyRect( &bounds ) )
		{
			p7::throw_errno( EINVAL );
		}
		
		const short rowBytes = compute_rowBytes_from_bounds( bounds );
		
		const size_t new_size = rowBytes * (bounds.bottom - bounds.top);
		
		::Ptr const old_base = params.bitmap.baseAddr;
		
		const size_t old_size = old_base ? N::GetPtrSize( old_base ) : 0;
		
		params.bits.reset();
		
		params.bitmap.baseAddr = ::NewPtrClear( new_size );
		
		if ( params.bitmap.baseAddr == NULL  &&  old_base != NULL )
		{
			/*
				Try to reallocate old block.  This shouldn't fail, but
				theoretically it might throw memFullErr.
			*/
			params.bits = N::NewPtrClear( old_size );
			
			params.bitmap.baseAddr = params.bits.get();
			
			// Even if we succeed here, it's still a failure mode.
			p7::throw_errno( ENOMEM );
		}
		
		// Seize the newly allocated block
		params.bits = n::owned< N::Ptr >::seize( params.bitmap.baseAddr );
		
		params.bitmap.bounds   = bounds;
		params.bitmap.rowBytes = rowBytes;
	}
	
	struct BitMap_size : serialize_Point
	{
		static const bool is_mutable = true;
		
		static Point Get( const BitMap& bits )
		{
			const short height = bits.bounds.bottom - bits.bounds.top;
			const short width  = bits.bounds.right - bits.bounds.left;
			
			Point point = { height, width };
			return point;
		}
		
		static void Set( BitMap_Parameters& params, const Point& size )
		{
			const Rect bounds = { 0, 0, size.v, size.h };
			
			BitMap_bounds::Set( params, bounds );
		}
	};
	
	static const BitMap& get_bitmap( const vfs::node* that )
	{
		BitMap_Parameters* it = gBitMapMap.find( that );
		
		if ( it == NULL )
		{
			p7::throw_errno( ENOENT );
		}
		
		return it->bitmap;
	}
	
	template < class Accessor >
	struct BitMap_Property : vfs::readwrite_property
	{
		static const int fixed_size = Accessor::fixed_size;
		
		static const bool can_set = Accessor::is_mutable;
		
		typedef typename Accessor::result_type result_type;
		
		static void get( plus::var_string& result, const vfs::node* that, bool binary )
		{
			const BitMap& bits = get_bitmap( that );
			
			const result_type data = Accessor::Get( bits );
			
			Accessor::deconstruct::apply( result, data, binary );
		}
		
		static void set( const vfs::node* that, const char* begin, const char* end, bool binary )
		{
			BitMap_Parameters& params = gBitMapMap[ that ];
			
			const result_type data = Accessor::reconstruct::apply( begin, end, binary );
			
			Accessor::Set( params, data );
			
			InvalidateWindowForView( that );
		}
	};
	
	
	static vfs::node_ptr bitmap_bits_Factory( const vfs::node*     parent,
	                                          const plus::string&  name,
	                                          const void*          args )
	{
		const mode_t mode = has_bits( parent ) ? S_IFREG | 0600 : 0;
		
		return new vfs::node( parent, name, mode, &bitmap_bits_methods );
	}
	
	#define PROPERTY( prop )  &vfs::new_property, &vfs::property_params_factory< BitMap_Property< prop > >::value
	
	static const vfs::fixed_mapping local_mappings[] =
	{
		{ "rowBytes", PROPERTY( BitMap_rowBytes ) },
		
		{ "bounds", PROPERTY( BitMap_bounds ) },
		{ "size",   PROPERTY( BitMap_size   ) },
		
		{ "bits", &bitmap_bits_Factory },
		
		{ NULL, NULL }
	};
	
	vfs::node_ptr New_FSTree_new_bitmap( const vfs::node*     parent,
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
