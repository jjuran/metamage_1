/*
	gui/new/scrollframe.cc
	----------------------
*/

#include "Genie/FS/gui/new/scrollframe.hh"

// POSIX
#include <sys/stat.h>

// plus
#include "plus/serialize.hh"
#include "plus/simple_map.hh"

// Pedestal
#include "Pedestal/EmptyView.hh"
#include "Pedestal/Scrollbar.hh"
#include "Pedestal/Scroller_beta.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/functions/resolve_pathname.hh"
#include "vfs/methods/item_method_set.hh"
#include "vfs/methods/link_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/node/types/fixed_dir.hh"
#include "vfs/node/types/property_file.hh"
#include "vfs/primitives/lookup.hh"

// relix-kernel
#include "relix/api/root.hh"

// Genie
#include "Genie/FS/ScrollerBase.hh"
#include "Genie/FS/subview.hh"
#include "Genie/FS/TrackScrollbar.hh"
#include "Genie/FS/Views.hh"


namespace Genie
{
	
	namespace Ped = Pedestal;
	
	
	template < class T >
	static inline T max( T a, T b )
	{
		return b < a ? a : b;
	}
	
	
	struct ScrollFrameParameters
	{
		bool  itHasVertical;
		bool  itHasHorizontal;
		
		boost::intrusive_ptr< Ped::View >  itsSubview;
		
		plus::string  itsTargetPath;
		
		ScrollerProxy  itsTargetProxy;
		
		ScrollFrameParameters() : itHasVertical(),
		                          itHasHorizontal(),
		                          itsSubview( Ped::EmptyView() )
		{
		}
	};
	
	typedef plus::simple_map< const vfs::node*, ScrollFrameParameters > ScrollFrameParametersMap;
	
	static ScrollFrameParametersMap gScrollFrameParametersMap;
	
	
	
	class Scrollbar : public Ped::Scrollbar
	{
		private:
			Scrollbar_UserData itsUserData;
		
		public:
			void Update( const Rect& bounds, const vfs::node* viewKey );
	};
	
	static inline
	Scrollbar_UserData* get_scrollbar_userdata( ControlRef control )
	{
		return (Scrollbar_UserData*) GetControlReference( control );
	}
	
	static Ped::ScrollerAPI* RecoverScrollerFromControl( ControlRef control )
	{
		Scrollbar_UserData* userData = get_scrollbar_userdata( control );
		
		ASSERT( userData      != NULL );
		ASSERT( userData->key != NULL );
		
		const vfs::node* scrollFrame = userData->key;
		
		if ( ScrollFrameParameters* it = gScrollFrameParametersMap.find( scrollFrame ) )
		{
			Ped::ScrollerAPI* proxy = &it->itsTargetProxy;
			
			return proxy;
		}
		
		return NULL;
	}
	
	static void DebriefAfterTrack( ControlRef control )
	{
		Scrollbar_UserData* userData = get_scrollbar_userdata( control );
		
		ASSERT( userData      != NULL );
		ASSERT( userData->key != NULL );
		
		const bool vertical = userData->vertical;
		
		const vfs::node* scrollFrame = userData->key;
		
		if ( ScrollFrameParameters* it = gScrollFrameParametersMap.find( scrollFrame ) )
		{
			if ( Ped::ScrollerAPI* proxy = &it->itsTargetProxy )
			{
				int clientLength = vertical ? proxy->ClientHeight() : proxy->ClientWidth();
				int viewLength   = vertical ? proxy->ViewHeight  () : proxy->ViewWidth  ();
				int offset       = vertical ? proxy->GetVOffset  () : proxy->GetHOffset ();
				
				short max_offset = max( clientLength - viewLength, offset );
				
				SetControlMaximum( control, max_offset );
			}
		}
	}
	
	void Scrollbar::Update( const Rect& bounds, const vfs::node* viewKey )
	{
		if ( Get() == NULL )
		{
			itsUserData.trackHook = &Ped::TrackScrollbar;
			itsUserData.fetchHook = &RecoverScrollerFromControl;
			itsUserData.afterHook = &DebriefAfterTrack;
			itsUserData.vertical  = bounds.bottom - bounds.top > bounds.right - bounds.left;
			itsUserData.key       = viewKey;
			
			Create( bounds, &itsUserData );
		}
		else
		{
			UpdateBounds( bounds );
		}
	}
	
	
	class ScrollFrame : public Ped::ScrollFrame
	{
		private:
			typedef const vfs::node* Key;
			
			Key itsKey;
			
			Scrollbar  itsVertical;
			Scrollbar  itsHorizontal;
			
			Rect itsLastBounds;
		
		public:
			ScrollFrame( Key key ) : itsKey( key )
			{
			}
			
			const Rect& Bounds() const  { return itsLastBounds; }
			
			void SetBounds( const Rect& bounds );
			
			Scrollbar& GetHorizontal()  { return itsHorizontal; }
			Scrollbar& GetVertical  ()  { return itsVertical;   }
			
			void UpdateScrollbars();
			
			void Draw( const Rect& bounds, bool erasing );
			
			Ped::View& Subview();
	};
	
	
	void ScrollFrame::SetBounds( const Rect& bounds )
	{
		itsLastBounds = bounds;
		
		Rect aperture = bounds;
		
		const short kScrollbarThickness = 16;
		
		const short kOverlap = 1;
		
		const short kFootprint = kScrollbarThickness - kOverlap;
		
		const ScrollFrameParameters& params = gScrollFrameParametersMap[ itsKey ];
		
		if ( params.itHasHorizontal )
		{
			aperture.bottom -= kFootprint;
			
			Rect scrollbarBounds = itsLastBounds;
			
			scrollbarBounds.top = aperture.bottom;
			
			scrollbarBounds.left   -= kOverlap;
			scrollbarBounds.bottom += kOverlap;
			scrollbarBounds.right  += kOverlap - kFootprint;
			
			itsHorizontal.Update( scrollbarBounds, itsKey );
		}
		else
		{
			itsHorizontal.Clear();
		}
		
		if ( params.itHasVertical )
		{
			aperture.right -= kFootprint;
			
			Rect scrollbarBounds = itsLastBounds;
			
			scrollbarBounds.left = aperture.right;
			
			scrollbarBounds.top    -= kOverlap;
			scrollbarBounds.right  += kOverlap;
			scrollbarBounds.bottom += kOverlap - kFootprint;
			
			itsVertical.Update( scrollbarBounds, itsKey );
		}
		else
		{
			itsVertical.Clear();
		}
		
		Subview().SetBounds( aperture );
		
		UpdateScrollbars();
	}
	
	void ScrollFrame::UpdateScrollbars()
	{
		const ScrollFrameParameters& params = gScrollFrameParametersMap[ itsKey ];
		
		const vfs::node* target = params.itsTargetProxy.Get();
		
		const Rect aperture = ApertureFromBounds( itsLastBounds );
		
		itsHorizontal.Adjust( GetScrollerClientWidth( target ),
		                      GetScrollerHOffset    ( target ),
		                      aperture.right - aperture.left );
		
		itsVertical.Adjust( GetScrollerClientHeight( target ),
		                    GetScrollerVOffset     ( target ),
		                    aperture.bottom - aperture.top );
	}
	
	
	void ScrollFrame::Draw( const Rect& bounds, bool erasing )
	{
		const ScrollFrameParameters& params = gScrollFrameParametersMap[ itsKey ];
		
		const bool rebind =    params.itHasHorizontal != !!itsHorizontal.Get()
		                    || params.itHasVertical   != !!itsVertical  .Get();
		
		if ( rebind )
		{
			SetBounds( bounds );
		}
		
		Subview().Draw( ApertureFromBounds( bounds ), erasing );
		
		UpdateScrollbars();
	}
	
	Ped::View& ScrollFrame::Subview()
	{
		boost::intrusive_ptr< Ped::View >& subview = gScrollFrameParametersMap[ itsKey ].itsSubview;
		
		if ( subview.get() == NULL )
		{
			subview = Ped::EmptyView();
		}
		
		return *subview;
	}
	
	static boost::intrusive_ptr< Ped::View > CreateView( const vfs::node* delegate )
	{
		return new ScrollFrame( delegate );
	}
	
	
	static void DestroyDelegate( const vfs::node* delegate )
	{
		gScrollFrameParametersMap.erase( delegate );
	}
	
	
	static bool scrollframe_target_exists( const vfs::node* view )
	{
		return gScrollFrameParametersMap[ view ].itsTargetProxy.Get() != NULL;
	}
	
	static void scrollframe_target_remove( const vfs::node* that )
	{
		const vfs::node* view = that->owner();
		
		ScrollFrameParameters& params = gScrollFrameParametersMap[ view ];
		
		params.itsTargetPath.reset();
		
		params.itsTargetProxy = ScrollerProxy( NULL );
		
		InvalidateWindowForView( view );
	}
	
	static void scrollframe_target_symlink( const vfs::node*     that,
	                                        const plus::string&  target_path )
	{
		const vfs::node* view = that->owner();
		
		const vfs::node_ptr target = resolve_pathname( *relix::root(), target_path, *view );
		
		vfs::node_ptr delegate = lookup( *target, plus::string::null );
		
		ScrollFrameParameters& params = gScrollFrameParametersMap[ view ];
		
		params.itsTargetPath  = target_path;
		params.itsTargetProxy = ScrollerProxy( delegate.get() );
		
		InvalidateWindowForView( view );
	}
	
	static plus::string scrollframe_target_readlink( const vfs::node* that )
	{
		const vfs::node* view = that->owner();
		
		return gScrollFrameParametersMap[ view ].itsTargetPath;
	}
	
	static const vfs::item_method_set scrollframe_target_item_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		&scrollframe_target_remove,
	};
	
	static const vfs::link_method_set scrollframe_target_link_methods =
	{
		&scrollframe_target_readlink,
		NULL,
		&scrollframe_target_symlink
	};
	
	static const vfs::node_method_set scrollframe_target_methods =
	{
		&scrollframe_target_item_methods,
		NULL,
		&scrollframe_target_link_methods
	};
	
	
	static
	Ped::View* get_view( const vfs::node* key, const plus::string& name )
	{
		return gScrollFrameParametersMap[ key ].itsSubview.get();
	}
	
	static
	void set_view( const vfs::node* key, const plus::string&, Ped::View* view )
	{
		gScrollFrameParametersMap[ key ].itsSubview = view;
	}
	
	const View_Accessors access =
	{
		&get_view,
		&set_view,
	};
	
	typedef plus::serialize_bool serialize;
	
	static
	void get( plus::var_string& result, const vfs::node* view, bool binary, const plus::string& name )
	{
		ScrollFrameParameters& params = gScrollFrameParametersMap[ view ];
		
		const bool has = name[ 0 ] == 'h' ? params.itHasHorizontal
		                                  : params.itHasVertical;
		
		serialize::deconstruct::apply( result, has, binary );
	}
	
	static
	void set( const vfs::node* view, const char* begin, const char* end, bool binary, const plus::string& name )
	{
		ScrollFrameParameters& params = gScrollFrameParametersMap[ view ];
		
		bool& has = name[ 0 ] == 'h' ? params.itHasHorizontal
		                             : params.itHasVertical;
		
		has = serialize::reconstruct::apply( begin, end, binary );
		
		InvalidateWindowForView( view );
	}
	
	static const vfs::property_params has_scrollbar_params =
	{
		serialize::fixed_size,
		(vfs::property_get_hook) &get,
		(vfs::property_set_hook) &set,
	};
	
	#define PROPERTY( prop )  &vfs::new_property, &has_scrollbar_params
	
	static vfs::node_ptr target_factory( const vfs::node*     parent,
	                                     const plus::string&  name,
	                                     const void*          args )
	{
		if ( const bool exists = scrollframe_target_exists( parent ) )
		{
			return new vfs::node( parent, name, S_IFLNK | 0777, &scrollframe_target_methods );
		}
		
		return new vfs::node( parent, name, 0, &scrollframe_target_methods );
	}
	
	static const vfs::fixed_mapping local_mappings[] =
	{
		{ "horizontal", PROPERTY( Horizontal_Property ) },
		{ "vertical",   PROPERTY( Vertical_Property   ) },
		
		{ "target", &target_factory },
		
		{ "view", &subview_factory, &access },
		{ "v",    &new_view_dir,            },
		
		{ NULL, NULL }
	};
	
	vfs::node_ptr New_FSTree_new_scrollframe( const vfs::node*     parent,
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
