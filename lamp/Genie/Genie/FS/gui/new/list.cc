/*
	gui/new/list.cc
	---------------
*/

#include "Genie/FS/gui/new/list.hh"

// POSIX
#include <sys/stat.h>

// Standard C++
#include <vector>

// POSIX
#include <fcntl.h>

// plus
#include "plus/serialize.hh"
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/stream_method_set.hh"
#include "vfs/filehandle/primitives/get_file.hh"
#include "vfs/filehandle/types/property_reader.hh"

// Pedestal
#include "Pedestal/ListView.hh"

// Genie
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/Views.hh"
#include "Genie/FS/data_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/Utilities/simple_map.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	struct ListParameters
	{
		std::vector< plus::string >  itsStrings;
		bool                         itIntersectsGrowBox;
		bool                         bounds_changed;
		bool                         data_changed;
		
		ListParameters() : itIntersectsGrowBox(),
		                   bounds_changed(),
		                   data_changed()
		{
		}
	};
	
	typedef simple_map< const FSTree*, ListParameters > ListParameterMap;
	
	static ListParameterMap gListParameterMap;
	
	
	class ListView : public Ped::ListView
	{
		private:
			typedef const FSTree* Key;
			
			Key itsKey;
		
		public:
			ListView( Key key ) : itsKey( key )
			{
			}
			
			bool IntersectsGrowBox() const;
			
			void Draw( const Rect& bounds, bool erasing );
	};
	
	bool ListView::IntersectsGrowBox() const
	{
		return gListParameterMap[ itsKey ].itIntersectsGrowBox;
	}
	
	void ListView::Draw( const Rect& bounds, bool erasing )
	{
		ListParameters& params = gListParameterMap[ itsKey ];
		
		if ( params.bounds_changed )
		{
			params.bounds_changed = false;
			
			SetBounds( bounds );
		}
		
		if ( params.data_changed )
		{
			params.data_changed = false;
			
			DeleteCells();
			
			const std::vector< plus::string >& strings = params.itsStrings;
			
			typedef std::vector< plus::string >::const_iterator Iter;
			
			for ( Iter it = strings.begin();  it != strings.end();  ++it )
			{
				const plus::string& s = *it;
				
				AppendCell( s.data(), s.size() );
			}
		}
		
		Ped::ListView::Draw( bounds, erasing );
	}
	
	
	static boost::intrusive_ptr< Ped::View > CreateView( const FSTree* delegate )
	{
		return new ListView( delegate );
	}
	
	
	static void DestroyDelegate( const FSTree* delegate )
	{
		gListParameterMap.erase( delegate );
	}
	
	
	class List_data_Handle : public vfs::filehandle
	{
		public:
			List_data_Handle( const vfs::node& file, int flags );
			
			ssize_t SysWrite( const char* buffer, std::size_t byteCount );
	};
	
	
	static ssize_t listdata_write( vfs::filehandle* that, const char* buffer, size_t n )
	{
		return static_cast< List_data_Handle& >( *that ).SysWrite( buffer, n );
	}
	
	static const vfs::stream_method_set listdata_stream_methods =
	{
		NULL,
		NULL,
		&listdata_write,
	};
	
	static const vfs::filehandle_method_set listdata_methods =
	{
		NULL,
		NULL,
		&listdata_stream_methods,
	};
	
	
	List_data_Handle::List_data_Handle( const vfs::node& file, int flags )
	:
		vfs::filehandle( &file, flags, &listdata_methods )
	{
	}
	
	ssize_t List_data_Handle::SysWrite( const char* buffer, std::size_t byteCount )
	{
		if ( byteCount != 0  &&  buffer[ byteCount - 1 ] != '\n' )
		{
			p7::throw_errno( EINVAL );
		}
		
		const FSTree* view = get_file( *this )->owner();
		
		ListParameters& params = gListParameterMap[ view ];
		
		std::vector< plus::string >& strings = params.itsStrings;
		
		if ( get_flags() & O_TRUNC )
		{
			strings.clear();
		}
		
		const char* end = buffer + byteCount;
		
		const char* p = buffer;
		
		while ( p < end )
		{
			const char* q = std::find( p, end, '\n' );
			
			strings.push_back( plus::string( p, q ) );
			
			p = q + 1;
			
			params.data_changed = true;
		}
		
		// FIXME:  Ignores the file mark
		
		InvalidateWindowForView( view );
		
		return byteCount;
	}
	
	
	static std::size_t measure_strings( const std::vector< plus::string >& strings )
	{
		std::size_t result = 0;
		
		typedef std::vector< plus::string >::const_iterator Iter;
		
		for ( Iter it = strings.begin();  it != strings.end();  ++it )
		{
			result += it->size() + 1;
		}
		
		return result;
	}
	
	static off_t list_data_geteof( const FSTree* that )
	{
		return measure_strings( gListParameterMap[ that->owner() ].itsStrings );
	}
	
	static void list_data_seteof( const FSTree* that, off_t length )
	{
		if ( length != 0 )
		{
			p7::throw_errno( EINVAL );
		}
		
		ListParameters& params = gListParameterMap[ that->owner() ];
		
		params.itsStrings.clear();
		
		params.data_changed = true;
	}
	
	static plus::string join_strings( const std::vector< plus::string >& strings )
	{
		plus::var_string result;
		
		typedef std::vector< plus::string >::const_iterator Iter;
		
		for ( Iter it = strings.begin();  it != strings.end();  ++it )
		{
			result += *it;
			result += '\n';
		}
		
		return result;
	}
	
	static vfs::filehandle_ptr list_data_open( const FSTree* that, int flags, mode_t mode )
	{
		if ( flags == O_RDONLY )
		{
			plus::string data = join_strings( gListParameterMap[ that->owner() ].itsStrings );
			
			return vfs::new_property_reader( *that, flags, data );
		}
		else if (    (flags & ~O_CREAT) - O_WRONLY == O_TRUNC
		          || (flags & ~O_CREAT) - O_WRONLY == O_APPEND )
		{
			// return below to silence Metrowerks warning
		}
		else
		{
			throw p7::errno_t( EINVAL );
		}
		
		return new List_data_Handle( *that, flags );
	}
	
	static const data_method_set list_data_data_methods =
	{
		&list_data_open,
		&list_data_geteof,
		&list_data_seteof
	};
	
	static const node_method_set list_data_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&list_data_data_methods
	};
	
	static FSTreePtr list_data_factory( const FSTree*        parent,
	                                    const plus::string&  name,
	                                    const void*          args )
	{
		return new FSTree( parent, name, S_IFREG | 0600, &list_data_methods );
	}
	
	
	namespace
	{
		
		bool& Overlap( const FSTree* view )
		{
			return gListParameterMap[ view ].itIntersectsGrowBox;
		}
		
	}
	
	template < class Serialize, typename Serialize::result_type& (*Access)( const FSTree* ) >
	struct List_Property : View_Property< Serialize, Access >
	{
		static void set( const FSTree* that, const char* begin, const char* end, bool binary )
		{
			View_Property< Serialize, Access >::set( that, begin, end, binary );
			
			gListParameterMap[ that ].bounds_changed = true;
		}
	};
	
	
	#define PROPERTY( prop )  &new_property, &property_params_factory< prop >::value
	
	typedef List_Property< plus::serialize_bool, Overlap >  Overlap_Property;
	
	static const vfs::fixed_mapping local_mappings[] =
	{
		{ "data", &list_data_factory },
		
		{ "overlap", PROPERTY( Overlap_Property ) },
		
		{ NULL, NULL }
	};
	
	FSTreePtr New_FSTree_new_list( const FSTree*        parent,
	                               const plus::string&  name,
	                               const void*          args )
	{
		return New_new_view( parent,
		                     name,
		                     &CreateView,
		                     local_mappings,
		                     &DestroyDelegate );
	}
	
}

