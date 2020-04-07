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
#include "plus/simple_map.hh"
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
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/node/types/property_file.hh"

// Pedestal
#include "Pedestal/ListView.hh"

// Genie
#include "Genie/FS/Views.hh"


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
	
	typedef plus::simple_map< const vfs::node*, ListParameters > ListParameterMap;
	
	static ListParameterMap gListParameterMap;
	
	
	class ListView : public Ped::ListView
	{
		private:
			typedef const vfs::node* Key;
			
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
	
	
	static boost::intrusive_ptr< Ped::View > CreateView( const vfs::node* delegate )
	{
		return new ListView( delegate );
	}
	
	
	static void DestroyDelegate( const vfs::node* delegate )
	{
		gListParameterMap.erase( delegate );
	}
	
	
	static
	ssize_t listdata_write( vfs::filehandle* that, const char* buffer, size_t n );
	
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
	
	
	static
	ssize_t listdata_write( vfs::filehandle* that, const char* buffer, size_t n )
	{
		if ( n != 0  &&  buffer[ n - 1 ] != '\n' )
		{
			p7::throw_errno( EINVAL );
		}
		
		const vfs::node* view = get_file( *that )->owner();
		
		ListParameters& params = gListParameterMap[ view ];
		
		std::vector< plus::string >& strings = params.itsStrings;
		
		if ( that->get_flags() & O_TRUNC )
		{
			strings.clear();
		}
		
		const char* end = buffer + n;
		
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
		
		return n;
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
	
	static off_t list_data_geteof( const vfs::node* that )
	{
		return measure_strings( gListParameterMap[ that->owner() ].itsStrings );
	}
	
	static void list_data_seteof( const vfs::node* that, off_t length )
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
	
	static vfs::filehandle_ptr list_data_open( const vfs::node* that, int flags, mode_t mode )
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
		
		return new vfs::filehandle( that, flags, &listdata_methods );
	}
	
	static const vfs::data_method_set list_data_data_methods =
	{
		&list_data_open,
		&list_data_geteof,
		&list_data_seteof
	};
	
	static const vfs::node_method_set list_data_methods =
	{
		NULL,
		&list_data_data_methods
	};
	
	static vfs::node_ptr list_data_factory( const vfs::node*     parent,
	                                        const plus::string&  name,
	                                        const void*          args )
	{
		return new vfs::node( parent, name, S_IFREG | 0600, &list_data_methods );
	}
	
	
	typedef plus::serialize_bool serialize;
	
	static
	void get( plus::var_string& result, const vfs::node* view, bool binary )
	{
		bool intersects = gListParameterMap[ view ].itIntersectsGrowBox;
		
		serialize::deconstruct::apply( result, intersects, binary );
	}
	
	static
	void set( const vfs::node* view, const char* begin, const char* end, bool binary )
	{
		ListParameters& params = gListParameterMap[ view ];
		
		bool& intersects = params.itIntersectsGrowBox;
		
		intersects = serialize::reconstruct::apply( begin, end, binary );
		
		InvalidateWindowForView( view );
		
		params.bounds_changed = true;
	}
	
	static const vfs::property_params list_overlap_params =
	{
		serialize::fixed_size,
		&get,
		&set,
	};
	
	#define PROPERTY( prop )  &vfs::new_property, &list_overlap_params
	
	static const vfs::fixed_mapping local_mappings[] =
	{
		{ "data", &list_data_factory },
		
		{ "overlap", PROPERTY( Overlap_Property ) },
		
		{ NULL, NULL }
	};
	
	vfs::node_ptr New_FSTree_new_list( const vfs::node*     parent,
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
