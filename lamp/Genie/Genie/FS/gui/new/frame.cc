/*
	gui/new/frame.cc
	----------------
*/

#include "Genie/FS/gui/new/frame.hh"

// POSIX
#include <fcntl.h>

// Iota
#include "iota/strings.hh"

// gear
#include "gear/inscribe_decimal.hh"
#include "gear/parse_decimal.hh"

// plus
#include "plus/serialize.hh"
#include "plus/simple_map.hh"

// Nitrogen
#include "Nitrogen/Quickdraw.hh"

// Pedestal
#include "Pedestal/EmptyView.hh"
#include "Pedestal/Frame.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/node/types/fixed_dir.hh"
#include "vfs/node/types/property_file.hh"

// Genie
#include "Genie/FS/subview.hh"
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
	
	struct stringify_Value
	{
		static void apply( plus::var_string& out, const Value& value );
	};
	
	void stringify_Value::apply( plus::var_string& out, const Value& value )
	{
		const bool is_auto = value.flags == 0  &&  value.number == 0;
		
		out = is_auto ? "auto" : gear::inscribe_decimal( value.number );
	}
	
	struct vivify_Value
	{
		static Value apply( const char* begin, const char* end );
	};
	
	Value vivify_Value::apply( const char* begin, const char* end )
	{
		if ( std::memcmp( begin, STR_LEN( "auto" ) + 1 ) == 0 )
		{
			return Value();
		}
		
		return Value( 1, gear::parse_decimal( begin ) );
	}
	
	struct serialize_Value : plus::serialize_POD< Value >
	{
		typedef stringify_Value  stringify;
		typedef vivify_Value     vivify;
		
		typedef plus::deconstruct< freeze, stringify >    deconstruct;
		typedef plus::reconstruct< Value, thaw, vivify >  reconstruct;
	};
	
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
		
		boost::intrusive_ptr< Ped::View >  itsSubview;
		
		FrameParameters() : padding( 0 ),
		                    outline_width(),
		                    outline_offset(),
		                    outline_curvature(),
		                    bounds_changed(),
		                    itsSubview( Ped::EmptyView() )
		{
		}
	};
	
	typedef plus::simple_map< const vfs::node*, FrameParameters > FrameParametersMap;
	
	static FrameParametersMap gFrameParametersMap;
	
	
	class Frame : public Ped::Frame
	{
		private:
			typedef const vfs::node* Key;
			
			Key itsKey;
		
		public:
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
		if ( FrameParameters* it = gFrameParametersMap.find( itsKey ) )
		{
			return it->padding;
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
		boost::intrusive_ptr< Ped::View >& subview = gFrameParametersMap[ itsKey ].itsSubview;
		
		if ( subview.get() == NULL )
		{
			subview = Ped::EmptyView();
		}
		
		return *subview;
	}
	
	static boost::intrusive_ptr< Ped::View > CreateView( const vfs::node* delegate )
	{
		return new Frame( delegate );
	}
	
	
	static void DestroyDelegate( const vfs::node* delegate )
	{
		gFrameParametersMap.erase( delegate );
	}
	
	
	
	namespace
	{
		
		Value& Width( const vfs::node* view )
		{
			return gFrameParametersMap[ view ].width;
		}
		
		Value& Height( const vfs::node* view )
		{
			return gFrameParametersMap[ view ].height;
		}
		
		Value& Margin_Top( const vfs::node* view )
		{
			return gFrameParametersMap[ view ].margin_top;
		}
		
		Value& Margin_Right( const vfs::node* view )
		{
			return gFrameParametersMap[ view ].margin_right;
		}
		
		Value& Margin_Bottom( const vfs::node* view )
		{
			return gFrameParametersMap[ view ].margin_bottom;
		}
		
		Value& Margin_Left( const vfs::node* view )
		{
			return gFrameParametersMap[ view ].margin_left;
		}
		
		int& Padding( const vfs::node* view )
		{
			return gFrameParametersMap[ view ].padding;
		}
		
		int& Outline_Width( const vfs::node* view )
		{
			return gFrameParametersMap[ view ].outline_width;
		}
		
		int& Outline_Offset( const vfs::node* view )
		{
			return gFrameParametersMap[ view ].outline_offset;
		}
		
		int& Outline_Curvature( const vfs::node* view )
		{
			return gFrameParametersMap[ view ].outline_curvature;
		}
		
	}
	
	static
	Ped::View* get_view( const vfs::node* key, const plus::string& name )
	{
		return gFrameParametersMap[ key ].itsSubview.get();
	}
	
	static
	void set_view( const vfs::node* key, const plus::string&, Ped::View* view )
	{
		gFrameParametersMap[ key ].itsSubview = view;
	}
	
	const View_Accessors access =
	{
		&get_view,
		&set_view,
	};
	
	
	static
	Value& value_field( const vfs::node* view, const plus::string& name )
	{
		FrameParameters& params = gFrameParametersMap[ view ];
		
		const unsigned n = name.size();
		
		Value& value = n == STRLEN( "width"          ) ? params.width
		             : n == STRLEN( "height"         ) ? params.height
		             : n == STRLEN( ".margin-top"    ) ? params.margin_top
		             : n == STRLEN( ".margin-right"  ) ? params.margin_right
		             : n == STRLEN( ".margin-bottom" ) ? params.margin_bottom
		             : n == STRLEN( ".margin-left"   ) ? params.margin_left
		             :                                   *(Value*) 0;
		
		return value;
	}
	
	static
	int& int_field( const vfs::node* view, const plus::string& name )
	{
		FrameParameters& params = gFrameParametersMap[ view ];
		
		const unsigned n = name.size();
		
		int& value = n == STRLEN( "padding"            ) ? params.padding
		           : n == STRLEN( ".outline-width"     ) ? params.outline_width
		           : n == STRLEN( ".outline-offset"    ) ? params.outline_offset
		           : n == STRLEN( ".outline-curvature" ) ? params.outline_curvature
		           :                                       *(int*) 0;
		
		return value;
	}
	
	static
	void value_get( plus::var_string& result, const vfs::node* view, bool binary, const plus::string& name )
	{
		const Value& value = value_field( view, name );
		
		serialize_Value::deconstruct::apply( result, value, binary );
	}
	
	static
	void value_set( const vfs::node* view, const char* begin, const char* end, bool binary, const plus::string& name )
	{
		Value& value = value_field( view, name );
		
		value = serialize_Value::reconstruct::apply( begin, end, binary );
		
		InvalidateWindowForView( view );
		
		gFrameParametersMap[ view ].bounds_changed = true;
	}
	
	static const vfs::property_params frame_value_params =
	{
		serialize_Value::fixed_size,
		(vfs::property_get_hook) &value_get,
		(vfs::property_set_hook) &value_set,
	};
	
	typedef plus::serialize_int< int > serialize_int;
	
	static
	void int_get( plus::var_string& result, const vfs::node* view, bool binary, const plus::string& name )
	{
		int value = int_field( view, name );
		
		serialize_int::deconstruct::apply( result, value, binary );
	}
	
	static
	void int_set( const vfs::node* view, const char* begin, const char* end, bool binary, const plus::string& name )
	{
		int& value = int_field( view, name );
		
		value = serialize_int::reconstruct::apply( begin, end, binary );
		
		InvalidateWindowForView( view );
		
		gFrameParametersMap[ view ].bounds_changed = true;
	}
	
	static const vfs::property_params frame_int_params =
	{
		serialize_int::fixed_size,
		(vfs::property_get_hook) &int_get,
		(vfs::property_set_hook) &int_set,
	};
	
	#define PROPERTY_VALUE( access )  &vfs::new_property, &frame_value_params
	#define PROPERTY_INT(   access )  &vfs::new_property, &frame_int_params
	
	static const vfs::fixed_mapping local_mappings[] =
	{
		{ "width",  PROPERTY_VALUE( Width  ) },
		{ "height", PROPERTY_VALUE( Height ) },
		
		{ ".margin-top",    PROPERTY_VALUE( Margin_Top    ) },
		{ ".margin-right",  PROPERTY_VALUE( Margin_Right  ) },
		{ ".margin-bottom", PROPERTY_VALUE( Margin_Bottom ) },
		{ ".margin-left",   PROPERTY_VALUE( Margin_Left   ) },
		
		{ "padding", PROPERTY_INT( Padding ) },
		
		{ ".outline-width",     PROPERTY_INT( Outline_Width     ) },
		{ ".outline-offset",    PROPERTY_INT( Outline_Offset    ) },
		{ ".outline-curvature", PROPERTY_INT( Outline_Curvature ) },
		
		{ "view", &subview_factory, &access },
		{ "v",    &new_view_dir,            },
		
		{ NULL, NULL }
	};
	
	vfs::node_ptr New_FSTree_new_frame( const vfs::node*     parent,
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
