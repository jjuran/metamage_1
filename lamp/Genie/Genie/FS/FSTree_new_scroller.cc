/*	======================
 *	FSTree_new_scroller.cc
 *	======================
 */

#include "Genie/FS/FSTree_new_scroller.hh"

// Standard C++
#include <map>

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
#include "Genie/FS/FSTree_sys_window_REF.hh"
#include "Genie/FS/ScrollerBase.hh"
#include "Genie/FS/Views.hh"


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
	
	typedef std::map< const FSTree*, BasicScrollerParameters > BasicScrollerParametersMap;
	
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
	
	
	template < class Property >
	static FSTreePtr Property_Factory( const FSTreePtr&     parent,
	                                   const plus::string&  name )
	{
		return New_FSTree_Property( parent,
		                            name,
		                            &Property::Get,
		                            &Property::Set );
	}
	
	static const FSTree_Premapped::Mapping local_mappings[] =
	{
		{ "width",  &Property_Factory< View_Property< Integer_Scribe< int >, ScrollerParameters::Width  > > },
		{ "height", &Property_Factory< View_Property< Integer_Scribe< int >, ScrollerParameters::Height > > },
		
		{ "x", &Property_Factory< View_Property< Integer_Scribe< int >, ScrollerParameters::HOffset > > },
		{ "y", &Property_Factory< View_Property< Integer_Scribe< int >, ScrollerParameters::VOffset > > },
		
		{ "v", &Basic_Factory< FSTree_X_view< GetView > >, true },
		
		{ NULL, NULL }
	};
	
	FSTreePtr New_FSTree_new_scroller( const FSTreePtr& parent, const plus::string& name )
	{
		return seize_ptr( new FSTree_new_View( parent,
		                                       name,
		                                       &CreateView,
		                                       local_mappings,
		                                       &DestroyDelegate ) );
	}
	
}

