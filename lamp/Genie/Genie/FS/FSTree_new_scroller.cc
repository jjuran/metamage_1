/*	======================
 *	FSTree_new_scroller.cc
 *	======================
 */

#include "Genie/FS/FSTree_new_scroller.hh"

// plus
#include "plus/serialize.hh"

// nucleus
#include "nucleus/saved.hh"

// Nitrogen
#include "Nitrogen/Quickdraw.hh"

// ClassicToolbox
#include "ClassicToolbox/MacWindows.hh"

// Pedestal
#include "Pedestal/EmptyView.hh"
#include "Pedestal/Scroller_beta.hh"

// Genie
#include "Genie/FS/FSTree_Directory.hh"
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/ScrollerBase.hh"
#include "Genie/FS/sys/port/ADDR.hh"
#include "Genie/FS/Views.hh"
#include "Genie/Utilities/simple_map.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace Ped = Pedestal;
	
	
	struct BasicScrollerParameters
	{
		boost::intrusive_ptr< Ped::View >  itsSubview;
		
		BasicScrollerParameters() : itsSubview( Ped::EmptyView::Get() )
		{
		}
	};
	
	typedef simple_map< const FSTree*, BasicScrollerParameters > BasicScrollerParametersMap;
	
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
			subview = Ped::EmptyView::Get();
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
		
		N::ValidRect( bounds );
	}
	
	
	static boost::intrusive_ptr< Ped::View > CreateView( const FSTree* delegate )
	{
		return seize_ptr( new BasicScroller( delegate ) );
	}
	
	
	static void DestroyDelegate( const FSTree* delegate )
	{
		ScrollerParameters::Erase( delegate );
		
		gBasicScrollerParametersMap.erase( delegate );
	}
	
	
	namespace
	{
		
		boost::intrusive_ptr< Ped::View >& GetView( const FSTree* key )
		{
			return gBasicScrollerParametersMap[ key ].itsSubview;
		}
		
	}
	
	
	#define PROPERTY( prop )  &new_property, &property_params_factory< prop >::value
	
	typedef View_Property< plus::serialize_int< int >, ScrollerParameters::Width  >  Width_Property;
	typedef View_Property< plus::serialize_int< int >, ScrollerParameters::Height >  Height_Property;
	
	typedef View_Property< plus::serialize_int< int >, ScrollerParameters::HOffset >  HOffset_Property;
	typedef View_Property< plus::serialize_int< int >, ScrollerParameters::VOffset >  VOffset_Property;
	
	static const FSTree_Premapped::Mapping local_mappings[] =
	{
		{ "width",  PROPERTY( Width_Property  ) },
		{ "height", PROPERTY( Height_Property ) },
		
		{ "x", PROPERTY( HOffset_Property ) },
		{ "y", PROPERTY( VOffset_Property ) },
		
		{ "v", &Basic_Factory< FSTree_X_view< GetView > > },
		
		{ NULL, NULL }
	};
	
	FSTreePtr New_FSTree_new_scroller( const FSTreePtr&     parent,
	                                   const plus::string&  name,
	                                   const void*          args )
	{
		return seize_ptr( new FSTree_new_View( parent,
		                                       name,
		                                       &CreateView,
		                                       local_mappings,
		                                       &DestroyDelegate ) );
	}
	
}

