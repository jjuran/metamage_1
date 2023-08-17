/*
	gui/new/scroller.cc
	-------------------
*/

#include "Genie/FS/gui/new/scroller.hh"

// Nostalgia
#include "Nostalgia/MacWindows.hh"

// plus
#include "plus/simple_map.hh"

// nucleus
#include "nucleus/saved.hh"

// Nitrogen
#include "Nitrogen/Quickdraw.hh"

// Pedestal
#include "Pedestal/EmptyView.hh"
#include "Pedestal/Scroller_beta.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/node/types/fixed_dir.hh"
#include "vfs/node/types/property_file.hh"

// Genie
#include "Genie/FS/ScrollerBase.hh"
#include "Genie/FS/subview.hh"
#include "Genie/FS/Views.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace Ped = Pedestal;
	
	
	struct BasicScrollerParameters
	{
		boost::intrusive_ptr< Ped::View >  itsSubview;
		
		BasicScrollerParameters() : itsSubview( Ped::EmptyView() )
		{
		}
	};
	
	typedef plus::simple_map< const vfs::node*, BasicScrollerParameters > BasicScrollerParametersMap;
	
	static BasicScrollerParametersMap  gBasicScrollerParametersMap;
	
	
	class BasicScroller : public ScrollerBase
	{
		private:
			Rect itsSavedBounds;
		
		public:
			BasicScroller( Key key ) : ScrollerBase( key )
			{
			}
			
			View& Subview();
			
			Rect ApertureFromBounds( const Rect& bounds );
			
			void Draw( const Rect& bounds, bool erasing );
			
			void Scroll( int dh, int dv );
	};
	
	Ped::View& BasicScroller::Subview()
	{
		boost::intrusive_ptr< Ped::View >& subview = gBasicScrollerParametersMap[ GetKey() ].itsSubview;
		
		if ( subview.get() == NULL )
		{
			subview = Ped::EmptyView();
		}
		
		return *subview;
	}
	
	Rect BasicScroller::ApertureFromBounds( const Rect& bounds )
	{
		Rect area = { 0 };
		
		area.right  = ClientWidth();
		area.bottom = ClientHeight();
		
		const short dx = bounds.left - GetHOffset();
		const short dy = bounds.top  - GetVOffset();
		
		::OffsetRect( &area, dx, dy );
		
		return area;
	}
	
	static void Draw_Clipped( BasicScroller&  that,
	                          const Rect&     bounds,
	                          bool            erasing,
	                          RgnHandle       clip )
	{
		n::saved< N::Clip > savedClip;
		
		N::SetClip( N::SectRgn( N::RectRgn( bounds ), clip ) );
		
		that.Superview::Draw( bounds, erasing );
	}
	
	void BasicScroller::Draw( const Rect& bounds, bool erasing )
	{
		ScrollerParameters::ViewBounds( GetKey() ) = bounds;
		
		itsSavedBounds = bounds;
		
		Draw_Clipped( *this, bounds, erasing, N::GetClip() );
	}
	
	void BasicScroller::Scroll( int dh, int dv )
	{
		const Rect& bounds = itsSavedBounds;
		
		Draw_Clipped( *this, bounds, true, N::ScrollRect( bounds, dh, dv ) );
		
		ValidRect( &bounds );
	}
	
	
	static boost::intrusive_ptr< Ped::View > CreateView( const vfs::node* delegate )
	{
		return new BasicScroller( delegate );
	}
	
	
	static void DestroyDelegate( const vfs::node* delegate )
	{
		ScrollerParameters::Erase( delegate );
		
		gBasicScrollerParametersMap.erase( delegate );
	}
	
	
	static
	Ped::View* get_view( const vfs::node* key, const plus::string& name )
	{
		return gBasicScrollerParametersMap[ key ].itsSubview.get();
	}
	
	static
	void set_view( const vfs::node* key, const plus::string&, Ped::View* view )
	{
		gBasicScrollerParametersMap[ key ].itsSubview = view;
	}
	
	const View_Accessors access =
	{
		&get_view,
		&set_view,
	};
	
	
	#define PROPERTY( prop )  &vfs::new_property, &scroller_setting_params
	
	static const vfs::fixed_mapping local_mappings[] =
	{
		{ "width",  PROPERTY( Width_Property  ) },
		{ "height", PROPERTY( Height_Property ) },
		
		{ "x", PROPERTY( HOffset_Property ) },
		{ "y", PROPERTY( VOffset_Property ) },
		
		{ "view", &subview_factory, &access },
		{ "v",    &new_view_dir,            },
		
		{ NULL, NULL }
	};
	
	vfs::node_ptr New_FSTree_new_scroller( const vfs::node*     parent,
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
