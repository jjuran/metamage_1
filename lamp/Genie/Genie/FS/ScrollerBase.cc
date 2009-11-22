/*	===============
 *	ScrollerBase.cc
 *	===============
 */

#include "Genie/FS/ScrollerBase.hh"

// Standard C++
#include <map>


namespace Genie
{
	
	namespace Ped = Pedestal;
	
	
	typedef std::map< const FSTree*, ScrollerParameters > ScrollerParametersMap;
	
	static ScrollerParametersMap gScrollerParametersMap;
	
	
	ScrollerParameters* ScrollerParameters::Find( const FSTree* key )
	{
		ScrollerParametersMap::iterator it = gScrollerParametersMap.find( key );
		
		if ( it != gScrollerParametersMap.end() )
		{
			return &it->second;
		}
		
		return NULL;
	}
	
	ScrollerParameters& ScrollerParameters::Get( const FSTree* key )
	{
		return gScrollerParametersMap[ key ];
	}
	
	void ScrollerParameters::Erase( const FSTree* key )
	{
		gScrollerParametersMap.erase( key );
	}
	
	int GetScrollerClientWidth( const FSTree* scroller )
	{
		if ( ScrollerParameters* params = ScrollerParameters::Find( scroller ) )
		{
			return params->itsClientWidth;
		}
		
		return 0;
	}
	
	int GetScrollerClientHeight( const FSTree* scroller )
	{
		if ( ScrollerParameters* params = ScrollerParameters::Find( scroller ) )
		{
			return params->itsClientHeight;
		}
		
		return 0;
	}
	
	int GetScrollerHOffset( const FSTree* scroller )
	{
		if ( ScrollerParameters* params = ScrollerParameters::Find( scroller ) )
		{
			return params->itsHOffset;
		}
		
		return 0;
	}
	
	int GetScrollerVOffset( const FSTree* scroller )
	{
		if ( ScrollerParameters* params = ScrollerParameters::Find( scroller ) )
		{
			return params->itsVOffset;
		}
		
		return 0;
	}
	
	void SetScrollerHOffset( const FSTree* scroller, int h )
	{
		if ( ScrollerParameters* params = ScrollerParameters::Find( scroller ) )
		{
			short dh = params->itsHOffset - h;
			
			params->itsHOffset = h;
			
			if ( params->itsView )
			{
				Ped::Scroller& scroller = *params->itsView;
				
				scroller.Scroll( dh, 0 );
			}
		}
	}
	
	void SetScrollerVOffset( const FSTree* scroller, int v )
	{
		if ( ScrollerParameters* params = ScrollerParameters::Find( scroller ) )
		{
			short dv = params->itsVOffset - v;
			
			params->itsVOffset = v;
			
			if ( params->itsView )
			{
				Ped::Scroller& scroller = *params->itsView;
				
				scroller.Scroll( 0, dv );
			}
		}
	}
	
	short GetScrollerLastViewWidth( const FSTree* scroller )
	{
		if ( ScrollerParameters* params = ScrollerParameters::Find( scroller ) )
		{
			const Rect& bounds = params->itsLastViewBounds;
			
			return bounds.right - bounds.left;
		}
		
		return 0;
	}
	
	short GetScrollerLastViewHeight( const FSTree* scroller )
	{
		if ( ScrollerParameters* params = ScrollerParameters::Find( scroller ) )
		{
			const Rect& bounds = params->itsLastViewBounds;
			
			return bounds.bottom - bounds.top;
		}
		
		return 0;
	}
	
	Rect GetScrollerLastViewBounds( const FSTree* scroller )
	{
		Rect result = { 0 };
		
		if ( ScrollerParameters* params = ScrollerParameters::Find( scroller ) )
		{
			result = params->itsLastViewBounds;
		}
		
		return result;
	}
	
	
	bool IsScrolledToBottom( const ScrollerParameters& params )
	{
		const Rect& bounds = params.itsLastViewBounds;
		
		// Not pinned at zero; a negative value works fine below
		short max_voffset = params.itsClientHeight - (bounds.bottom - bounds.top);
		
		return params.itsVOffset >= max_voffset;
	}
	
	
	short ScrollerProxy::ViewWidth() const
	{
		return GetScrollerLastViewWidth( itsKey );
	}
	
	short ScrollerProxy::ViewHeight() const
	{
		return GetScrollerLastViewHeight( itsKey );
	}
	
	int ScrollerProxy::GetHOffset() const
	{
		return GetScrollerHOffset( itsKey );
	}
	
	int ScrollerProxy::GetVOffset() const
	{
		return GetScrollerVOffset( itsKey );
	}
	
	void ScrollerProxy::SetHOffset( int h )
	{
		SetScrollerHOffset( itsKey, h );
	}
	
	void ScrollerProxy::SetVOffset( int v )
	{
		SetScrollerVOffset( itsKey, v );
	}
	
	
	void ScrollerBase::Install( const Rect& bounds )
	{
		gScrollerParametersMap[ itsKey ].itsView = this;
		
		Ped::Superview::Install( bounds );
		
		SetBounds( bounds );
	}
	
	void ScrollerBase::Uninstall()
	{
		gScrollerParametersMap[ itsKey ].itsView = NULL;
		
		Ped::Superview::Uninstall();
	}
	
	bool ScrollerBase::KeyDown( const EventRecord& event )
	{
		return Ped::Scroller_KeyDown( *this, event )  ||  Ped::Superview::KeyDown( event );
	}
	
	void ScrollerBase::Draw( const Rect& bounds, bool erasing )
	{
		gScrollerParametersMap[ itsKey ].itsLastViewBounds = bounds;
		
		Ped::Superview::Draw( bounds, erasing );
	}
	
}

