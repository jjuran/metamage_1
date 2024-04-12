/*
	gui/new/list.cc
	---------------
*/

#include "Genie/FS/gui/new/list.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __LISTS__
#include <Lists.h>
#endif

// POSIX
#include <fcntl.h>
#include <sys/stat.h>

// Standard C
#include <string.h>

// Nostalgia
#include "Nostalgia/MacWindows.hh"

// mac-qd-utils
#include "mac_qd/get_visRgn.hh"
#include "mac_qd/globals/thePort.hh"
#include "mac_qd/globals/thePort_window.hh"

// plus
#include "plus/serialize.hh"
#include "plus/simple_map.hh"
#include "plus/var_string.hh"

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

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
#include "Pedestal/View.hh"

// Genie
#include "Genie/FS/Views.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	struct ListParameters
	{
		plus::string  its_data;
		bool          itIntersectsGrowBox;
		bool          bounds_changed;
		bool          data_changed;
		
		ListParameters() : itIntersectsGrowBox(),
		                   bounds_changed(),
		                   data_changed()
		{
		}
	};
	
	typedef plus::simple_map< const vfs::node*, ListParameters > ListParameterMap;
	
	static ListParameterMap gListParameterMap;
	
	
	class ListView : public Ped::View
	{
		private:
			typedef const vfs::node* Key;
			
			Key itsKey;
			
			ListHandle itsList;
			
			// non-copyable
			ListView           ( const ListView& );
			ListView& operator=( const ListView& );
		
		private:
			void Install( const Rect& bounds );
			
			void Uninstall();
		
		public:
			ListView( Key key ) : itsKey( key ), itsList()
			{
			}
			
			~ListView()  { LDispose( itsList ); }
			
			ListHandle Get() const  { return itsList; }
			
			bool IntersectsGrowBox() const;
			
			void SetBounds( const Rect& bounds );
			
			void AppendCell( const char* data, std::size_t length );
			
			void DeleteCells();
			
			bool MouseDown( const EventRecord& event );
			bool KeyDown  ( const EventRecord& event );
			
			void Activate( bool activating )  { ::LActivate( activating, itsList ); }
			
			void Draw( const Rect& bounds, bool erasing );
	};
	
	inline
	bool ListView::IntersectsGrowBox() const
	{
		return gListParameterMap[ itsKey ].itIntersectsGrowBox;
	}
	
	static Rect AdjustedListBounds( const Rect&  bounds,
	                                bool         scrollHoriz,
	                                bool         scrollVert )
	{
		Rect result = bounds;
		
		/*
			Mac OS places the scroll bars outside the bounds.
			We adjust the bounds inward so that the scroll bars
			draw within the original bounds.
		*/
		
		if ( scrollHoriz )
		{
			result.bottom -= 15;
		}
		
		if ( scrollVert )
		{
			result.right -= 15;
		}
		
		return result;
	}
	
	static void FixListScrollbarBounds( ListHandle list )
	{
		//bool hasGrow     = GetListFlags( list ) & 0x20;
		const bool hasGrow = true;
		
		const bool scrollVert  = ::GetListVerticalScrollBar  ( list );
		const bool scrollHoriz = ::GetListHorizontalScrollBar( list );
		
		// List Manager bug:  LNew() and LSize() ignore hasGrow, so we have to fudge it
		if ( hasGrow && ( scrollVert != scrollHoriz ) )
		{
			// This hack is only necessary with hasGrow and one scrollbar
			
			ControlRef scrollbar = scrollVert ? ::GetListVerticalScrollBar  ( list )
			                                  : ::GetListHorizontalScrollBar( list );
			
			Rect bounds;
			GetControlBounds( scrollbar, &bounds );
			
			InvalRect(& bounds );
			
			(scrollVert ? bounds.bottom : bounds.right) -= 15;
			
			SetControlBounds( scrollbar, &bounds );
			
			DrawOneControl( scrollbar );
		}
	}
	
	void ListView::Install( const Rect& bounds )
	{
		const bool drawIt      = true;
		const bool hasGrow     = IntersectsGrowBox();
		const bool scrollHoriz = false;
		const bool scrollVert  = true;
		
		const short proc = 0;
		
		const Rect dataBounds = { 0, 0, 0, 1 };  // one column, zero rows
		const Point cSize = {};
		
		Rect rView = AdjustedListBounds( bounds, scrollHoriz, scrollVert );
		
		ListHandle list = ::LNew( &rView,
		                          &dataBounds,
		                          cSize,
		                          proc,
		                          mac::qd::thePort_window(),
		                          drawIt,
		                          hasGrow,
		                          scrollHoriz,
		                          scrollVert );
		
		// FIXME:  Check for null handle
		
		itsList = list;
		
		if ( hasGrow )
		{
			FixListScrollbarBounds( itsList );
		}
	}
	
	void ListView::Uninstall()
	{
		if ( itsList )
		{
			LDispose( itsList );
		}
		
		itsList = NULL;
	}
	
	void ListView::SetBounds( const Rect& bounds )
	{
		const Rect r = AdjustedListBounds( bounds, false, true );
		
		SetListViewBounds( itsList, &r );
		
		LSize( r.right - r.left,
		       r.bottom - r.top,
		       itsList );
		
		itsList[0]->cellSize.h = r.right - r.left;
		
		if ( IntersectsGrowBox() )
		{
			FixListScrollbarBounds( itsList );
		}
	}
	
	inline
	void ListView::AppendCell( const char* data, std::size_t length )
	{
		Rect bounds;
		
		GetListDataBounds( itsList, &bounds );
		
		const short i_row = LAddRow( 1, bounds.bottom, itsList );
		
		const Point loc = { i_row, 0 };
		
		LSetCell( data, length, loc, itsList );
	}
	
	inline
	void ListView::DeleteCells()
	{
		LDelRow( 0, 0, itsList );
	}
	
	bool ListView::MouseDown( const EventRecord& event )
	{
		Point where = event.where;
		
		GlobalToLocal( &where );
		
		/*
			MWC68K mysteriously generates smaller code
			with the (int) cast than without it.
		*/
		
		LClick( where,
		        (int) event.modifiers,
		        itsList );
		
		return true;
	}
	
	bool ListView::KeyDown( const EventRecord& event )
	{
		// FIXME
		return false;
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
			
			const plus::string& data = params.its_data;
			
			const char* end = data.end();
			
			for ( const char* p = data.begin();  p != end; )
			{
				const char* q = strchr( p, '\n' );
				
				AppendCell( p, q - p );
				
				p = q + 1;
			}
		}
		
		using mac::qd::get_visRgn;
		using mac::qd::thePort;
		
		//Rect bounds = Bounds( itsList );
		//EraseRect( &bounds );
		
		LUpdate( get_visRgn( thePort() ), itsList );
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
		
		if ( that->get_flags() & O_TRUNC )
		{
			params.its_data.assign( buffer, n );
		}
		else
		{
			plus::var_string combined = params.its_data;
			
			combined.append( buffer, n );
			
			params.its_data = combined.move();
		}
		
		params.data_changed = true;
		
		// FIXME:  Ignores the file mark
		
		InvalidateWindowForView( view );
		
		return n;
	}
	
	
	static off_t list_data_geteof( const vfs::node* that )
	{
		return gListParameterMap[ that->owner() ].its_data.size();
	}
	
	static void list_data_seteof( const vfs::node* that, off_t length )
	{
		if ( length != 0 )
		{
			p7::throw_errno( EINVAL );
		}
		
		ListParameters& params = gListParameterMap[ that->owner() ];
		
		params.its_data.reset();
		
		params.data_changed = true;
	}
	
	static vfs::filehandle_ptr list_data_open( const vfs::node* that, int flags, mode_t mode )
	{
		if ( flags == O_RDONLY )
		{
			plus::string data = gListParameterMap[ that->owner() ].its_data;
			
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
