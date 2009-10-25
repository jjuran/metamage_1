/*	===================
 *	FSTree_new_frame.cc
 *	===================
 */

#include "Genie/FS/FSTree_new_frame.hh"

// POSIX
#include <fcntl.h>

// Iota
#include "iota/strings.hh"

// Nitrogen
#include "Nitrogen/QuickDraw.h"

// Pedestal
#include "Pedestal/EmptyView.hh"
#include "Pedestal/Frame.hh"

// Genie
#include "Genie/FS/FSTree_Directory.hh"
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/FSTree_sys_window_REF.hh"
#include "Genie/FS/Scribes.hh"
#include "Genie/FS/Views.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace Ped = Pedestal;
	
	
	struct Value
	{
		UInt16 flags;
		SInt16 number;
		
		Value() : flags(), number()
		{
			// 0,0 -> undefined/auto
		}
		
		Value( UInt16 f, SInt16 n ) : flags( f ), number( n )
		{
		}
	};
	
	struct Value_Scribe
	{
		typedef Genie::Value Value;
		
		static std::string Encode( const Value& value );
		
		static Value Decode( const char* begin, const char* end );
	};
	
	std::string Value_Scribe::Encode( const Value& value )
	{
		if ( value.flags == 0  &&  value.number == 0 )
		{
			return "auto";
		}
		
		return Int_Scribe::Encode( value.number );
	}
	
	Value Value_Scribe::Decode( const char* begin, const char* end )
	{
		if ( std::memcmp( begin, STR_LEN( "auto" ) + 1 ) == 0 )
		{
			return Value();
		}
		
		return Value( 1, Int_Scribe::Decode( begin, end ) );
	}
	
	
	struct FrameParameters
	{
		Value  width;
		Value  height;
		
		Value  margin_top;
		Value  margin_right;
		Value  margin_bottom;
		Value  margin_left;
		
		int    padding;
		int    outline_width;
		int    outline_offset;
		int    outline_curvature;
		bool   bounds_changed;
		
		boost::shared_ptr< Ped::View >  itsSubview;
		
		FrameParameters() : padding( 0 ),
		                    outline_width(),
		                    outline_offset(),
		                    outline_curvature(),
		                    bounds_changed(),
		                    itsSubview( Ped::EmptyView::Get() )
		{
		}
	};
	
	typedef std::map< const FSTree*, FrameParameters > FrameParametersMap;
	
	static FrameParametersMap gFrameParametersMap;
	
	
	class Frame : public Ped::Frame
	{
		private:
			typedef const FSTree* Key;
			
			Key itsKey;
		
		public:
			typedef Key Initializer;
			
			Frame( Key key ) : itsKey( key )
			{
			}
			
			void Draw( const Rect& bounds, bool erasing );
			
			short Padding() const;
			
			short OutlineWidth() const;
			short OutlineOffset() const;
			short OutlineCurvature() const;
			
			Rect Margin( const Rect& bounds ) const;
			
			Ped::View& Subview();
	};
	
	void Frame::Draw( const Rect& bounds, bool erasing )
	{
		FrameParameters& params = gFrameParametersMap[ itsKey ];
		
		if ( params.bounds_changed )
		{
			params.bounds_changed = false;
			
			SetBounds( bounds );
		}
		
		Ped::Frame::Draw( bounds, erasing );
	}
	
	short Frame::Padding() const
	{
		FrameParametersMap::const_iterator it = gFrameParametersMap.find( itsKey );
		
		if ( it != gFrameParametersMap.end() )
		{
			return it->second.padding;
		}
		
		return 0;
	}
	
	short Frame::OutlineWidth() const
	{
		return gFrameParametersMap[ itsKey ].outline_width;
	}
	
	short Frame::OutlineOffset() const
	{
		return gFrameParametersMap[ itsKey ].outline_offset;
	}
	
	short Frame::OutlineCurvature() const
	{
		return gFrameParametersMap[ itsKey ].outline_curvature;
	}
	
	Rect Frame::Margin( const Rect& bounds ) const
	{
		const FrameParameters& params = gFrameParametersMap[ itsKey ];
		
		const Value& width  = params.width;
		const Value& height = params.height;
		
		const Value& margin_top    = params.margin_top;
		const Value& margin_right  = params.margin_right;
		const Value& margin_bottom = params.margin_bottom;
		const Value& margin_left   = params.margin_left;
		
		const short padding = params.padding;
		
		const short outer_width  = (bounds.right - bounds.left) - 2 * padding;
		const short outer_height = (bounds.bottom - bounds.top) - 2 * padding;
		
		Rect margin = N::SetRect( 0, 0, 0, 0 );
		
		if ( width.flags  &&  !margin_left.flags )
		{
			if ( !margin_right.flags )
			{
				const short h_margin = outer_width - width.number;
				
				const short left_margin  = h_margin / 2;
				const short right_margin = h_margin - left_margin;
				
				margin.left  = left_margin;
				margin.right = right_margin;
			}
			else
			{
				margin.right = margin_right.number;
				
				margin.left = outer_width - margin.right - width.number;
			}
		}
		else
		{
			margin.left  = margin_left.number;
			
			const bool constrained = margin_left.flags && width.flags;
			
			margin.right = constrained ? outer_width - margin.left - width.number
			                           : margin_right.number;
		}
		
		if ( height.flags  &&  !margin_top.flags )
		{
			if ( !margin_bottom.flags )
			{
				const short v_margin = outer_height - height.number;
				
				const short top_margin    = v_margin / 2;
				const short bottom_margin = v_margin - top_margin;
				
				margin.top    = top_margin;
				margin.bottom = bottom_margin;
			}
			else
			{
				margin.bottom = margin_bottom.number;
				
				margin.top = outer_height - margin.bottom - height.number;
			}
		}
		else
		{
			margin.top  = margin_top.number;
			
			const bool constrained = margin_top.flags && height.flags;
			
			margin.bottom = constrained ? outer_height - margin.top - height.number
			                            : margin_bottom.number;
		}
		
		return margin;
	}
	
	Ped::View& Frame::Subview()
	{
		boost::shared_ptr< Ped::View >& subview = gFrameParametersMap[ itsKey ].itsSubview;
		
		if ( subview.get() == NULL )
		{
			subview = Ped::EmptyView::Get();
		}
		
		return *subview;
	}
	
	static boost::shared_ptr< Ped::View > CreateView( const FSTree* delegate )
	{
		return boost::shared_ptr< Ped::View >( new Frame( delegate ) );
	}
	
	
	static void DestroyDelegate( const FSTree* delegate )
	{
		gFrameParametersMap.erase( delegate );
	}
	
	
	
	namespace
	{
		
		Value& Width( const FSTree* view )
		{
			return gFrameParametersMap[ view ].width;
		}
		
		Value& Height( const FSTree* view )
		{
			return gFrameParametersMap[ view ].height;
		}
		
		Value& Margin_Top( const FSTree* view )
		{
			return gFrameParametersMap[ view ].margin_top;
		}
		
		Value& Margin_Right( const FSTree* view )
		{
			return gFrameParametersMap[ view ].margin_right;
		}
		
		Value& Margin_Bottom( const FSTree* view )
		{
			return gFrameParametersMap[ view ].margin_bottom;
		}
		
		Value& Margin_Left( const FSTree* view )
		{
			return gFrameParametersMap[ view ].margin_left;
		}
		
		int& Padding( const FSTree* view )
		{
			return gFrameParametersMap[ view ].padding;
		}
		
		int& Outline_Width( const FSTree* view )
		{
			return gFrameParametersMap[ view ].outline_width;
		}
		
		int& Outline_Offset( const FSTree* view )
		{
			return gFrameParametersMap[ view ].outline_offset;
		}
		
		int& Outline_Curvature( const FSTree* view )
		{
			return gFrameParametersMap[ view ].outline_curvature;
		}
		
		boost::shared_ptr< Ped::View >& GetView( const FSTree* key )
		{
			return gFrameParametersMap[ key ].itsSubview;
		}
		
	}
	
	
	template < class Property >
	static FSTreePtr Property_Factory( const FSTreePtr&    parent,
	                                   const std::string&  name )
	{
		return New_FSTree_Property( parent,
		                            name,
		                            &Property::Get,
		                            &Property::Set );
	}
	
	template < class Scribe, typename Scribe::Value& (*Access)( const FSTree* ) >
	struct Frame_Property : View_Property< Scribe, Access >
	{
		static void Set( const FSTree* that, const char* begin, const char* end, bool binary )
		{
			View_Property< Scribe, Access >::Set( that, begin, end, binary );
			
			const FSTree* view = GetViewKey( that );
			
			gFrameParametersMap[ view ].bounds_changed = true;
		}
	};
	
	static const FSTree_Premapped::Mapping local_mappings[] =
	{
		{ "width",  &Property_Factory< Frame_Property< Value_Scribe, Width  > > },
		{ "height", &Property_Factory< Frame_Property< Value_Scribe, Height > > },
		
		{ ".margin-top",    &Property_Factory< Frame_Property< Value_Scribe, Margin_Top    > > },
		{ ".margin-right",  &Property_Factory< Frame_Property< Value_Scribe, Margin_Right  > > },
		{ ".margin-bottom", &Property_Factory< Frame_Property< Value_Scribe, Margin_Bottom > > },
		{ ".margin-left",   &Property_Factory< Frame_Property< Value_Scribe, Margin_Left   > > },
		
		{ "padding", &Property_Factory< Frame_Property< Int_Scribe, Padding > > },
		
		{ ".outline-width",     &Property_Factory< Frame_Property< Int_Scribe, Outline_Width     > > },
		{ ".outline-offset",    &Property_Factory< Frame_Property< Int_Scribe, Outline_Offset    > > },
		{ ".outline-curvature", &Property_Factory< Frame_Property< Int_Scribe, Outline_Curvature > > },
		
		{ "v", &Basic_Factory< FSTree_X_view< GetView > >, true },
		
		{ NULL, NULL }
	};
	
	FSTreePtr New_FSTree_new_frame( const FSTreePtr& parent, const std::string& name )
	{
		return FSTreePtr( new FSTree_new_View( parent,
		                                       name,
		                                       &CreateView,
		                                       local_mappings,
		                                       &DestroyDelegate ) );
	}
	
}

