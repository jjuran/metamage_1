/*
	Genie/FS/sys/app/window/list/REF.hh
	-----------------------------------
*/

#include "Genie/FS/sys/app/window/list/REF.hh"

#ifdef __APPLE__
#include <AvailabilityMacros.h>
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAWTEXT_H
#include "missing/QuickdrawText.h"
#endif
#endif

// Standard C/C++
#include <cstdio>
#include <cstring>

// mac-config
#include "mac_config/color-quickdraw.hh"

// mac-sys-utils
#include "mac_sys/has/ColorQuickDraw.hh"
#include "mac_sys/windowlist_contains.hh"

// mac-qd-utils
#include "mac_qd/scoped_port.hh"

// mac-ui-utils
#include "mac_ui/windows.hh"

// mac-app-utils
#include "mac_app/Window_menu.hh"

// gear
#include "gear/hexadecimal.hh"

// plus
#include "plus/deconstruct.hh"
#include "plus/hexadecimal.hh"
#include "plus/mac_utf8.hh"
#include "plus/reconstruct.hh"
#include "plus/serialize.hh"
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/ATSTypes.hh"
#include "Nitrogen/Quickdraw.hh"

// Nitrogen
#include "Nitrogen/MacWindows.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/property.hh"
#include "vfs/node/types/property_file.hh"
#include "vfs/node/types/trigger.hh"

// Pedestal
#include "Pedestal/Window.hh"
#include "Pedestal/WindowStorage.hh"

// relix-kernel
#include "relix/config/color.hh"

// Genie
#include "Genie/FS/serialize_Str255.hh"
#include "Genie/FS/serialize_qd.hh"


static inline
bool has_color_quickdraw()
{
	return CONFIG_COLOR_QUICKDRAW_GRANTED  ||  mac::sys::has_ColorQuickDraw();
}

namespace Nitrogen
{
	
	inline FontID GetPortTextFont( CGrafPtr port )
	{
		return FontID( ::GetPortTextFont( port ) );
	}
	
	static
	float GetWindowAlpha( WindowRef window )
	{
		float alpha;
		
		Mac::ThrowOSStatus( ::GetWindowAlpha( window, &alpha ) );
		
		return alpha;
	}
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	struct Access_WindowPosition : serialize_Point
	{
		static Point Get( WindowRef window )
		{
			return mac::ui::get_window_position( window );
		}
		
		static void Set( WindowRef window, Point position )
		{
			Ped::SetWindowPosition( window, position );
		}
	};
	
	struct Access_WindowSize : serialize_Point
	{
		static Point Get( WindowRef window )
		{
			return mac::ui::get_window_size( window );
		}
		
		static void Set( WindowRef window, Point size )
		{
			Ped::SetWindowSize( window, size );
		}
	};
	
	struct Access_WindowVisible : plus::serialize_bool
	{
		static bool Get( WindowRef window )
		{
			return N::IsWindowVisible( window );
		}
		
		static void Set( WindowRef window, bool visible )
		{
			if ( visible )
			{
				ShowWindow( window );
			}
			else
			{
				HideWindow( window );
			}
		}
	};
	
	struct Access_WindowZOrder : plus::serialize_unsigned< unsigned >
	{
		static unsigned Get( WindowRef window )
		{
			unsigned z = 0;
			
			for ( WindowRef it = GetWindowList();  it != window;  ++z, it = GetNextWindow( it ) )
			{
				if ( it == NULL )
				{
					p7::throw_errno( ENOENT );
				}
			}
			
			return z;
		}
	};
	
	static inline UInt16 ReadIntensity_1n( const char* p )
	{
		const unsigned char nibble = gear::decoded_hex_digit( p[ 0 ] );
		
		UInt16 result = nibble << 12
		              | nibble <<  8
		              | nibble <<  4
		              | nibble <<  0;
		
		return result;
	}
	
	static inline UInt16 ReadIntensity_2n( const char* p )
	{
		const unsigned char high = gear::decoded_hex_digit( p[ 0 ] );
		const unsigned char low  = gear::decoded_hex_digit( p[ 1 ] );
		
		UInt16 result = high << 12
		              | low  <<  8
		              | high <<  4
		              | low  <<  0;
		
		return result;
	}
	
	static inline UInt16 ReadIntensity_3n( const char* p )
	{
		const unsigned char high = gear::decoded_hex_digit( p[ 0 ] );
		const unsigned char med  = gear::decoded_hex_digit( p[ 1 ] );
		const unsigned char low  = gear::decoded_hex_digit( p[ 2 ] );
		
		UInt16 result = high << 12
		              | med  <<  8
		              | low  <<  4
		              | high <<  0;
		
		return result;
	}
	
	static UInt16 ReadIntensity_4n( const char* p )
	{
		const UInt16 result = gear::decoded_hex_digit( p[ 0 ] ) << 12
		                    | gear::decoded_hex_digit( p[ 1 ] ) <<  8
		                    | gear::decoded_hex_digit( p[ 2 ] ) <<  4
		                    | gear::decoded_hex_digit( p[ 3 ] ) <<  0;
		
		return result;
	}
	
	static RGBColor ReadColor( const char* begin, const char* end )
	{
		const char* p = begin;
		
		if ( *p == '#' )
		{
			++p;
		}
		
		size_t length = end - p;
		
		unsigned detail = length / 3;
		
		if ( length % 3 != 0 )
		{
			p7::throw_errno( EINVAL );
		}
		
		RGBColor color;
		
		switch ( detail )
		{
			case 1:
				color.red   = ReadIntensity_1n( p );
				color.green = ReadIntensity_1n( p += detail );
				color.blue  = ReadIntensity_1n( p += detail );
				break;
			
			case 2:
				color.red   = ReadIntensity_2n( p );
				color.green = ReadIntensity_2n( p += detail );
				color.blue  = ReadIntensity_2n( p += detail );
				break;
			
			case 3:
				color.red   = ReadIntensity_3n( p );
				color.green = ReadIntensity_3n( p += detail );
				color.blue  = ReadIntensity_3n( p += detail );
				break;
			
			case 4:
				color.red   = ReadIntensity_4n( p );
				color.green = ReadIntensity_4n( p += detail );
				color.blue  = ReadIntensity_4n( p += detail );
				break;
			
			default:
				p7::throw_errno( EINVAL );
		}
		
		return color;
	}
	
	static plus::string WriteColor( const RGBColor& color )
	{
		char encoded[] = "#rrrrggggbbbb";
		
		std::sprintf( encoded + 1, "%.4x%.4x%.4x", color.red,
		                                           color.green,
		                                           color.blue );
		
		return encoded;
	}
	
	struct stringify_RGBColor
	{
		static void apply( plus::var_string& out, const RGBColor& color )
		{
			out = WriteColor( color );
		}
	};
	
	struct vivify_RGBColor
	{
		static RGBColor apply( const char* begin, const char* end )
		{
			return ReadColor( begin, end );
		}
	};
	
	struct serialize_RGBColor : plus::serialize_POD< RGBColor >
	{
		typedef stringify_RGBColor  stringify;
		typedef vivify_RGBColor     vivify;
		
		typedef plus::deconstruct< freeze, stringify >       deconstruct;
		typedef plus::reconstruct< RGBColor, thaw, vivify >  reconstruct;
	};
	
	struct fore_color_traits
	{
		static void get( RGBColor& color, CGrafPtr port )
		{
			GetPortForeColor( port, &color );
		}
		
		static void set( const RGBColor& color )
		{
			RGBForeColor( &color );
		}
	};
	
	struct back_color_traits
	{
		static void get( RGBColor& color, CGrafPtr port )
		{
			GetPortBackColor( port, &color );
		}
		
		static void set( const RGBColor& color )
		{
			RGBBackColor( &color );
		}
	};
	
	template < class Traits >
	struct Access_WindowColor : serialize_RGBColor
	{
		static RGBColor Get( WindowRef window )
		{
			if ( ! (CONFIG_COLOR_QUICKDRAW  &&  has_color_quickdraw()) )
			{
				p7::throw_errno( ENOENT );
			}
			
			RGBColor color;
			
			Traits::get( color, GetWindowPort( window ) );
			
			return color;
		}
		
		static void Set( WindowRef window, const RGBColor& color )
		{
			if ( ! (CONFIG_COLOR_QUICKDRAW  &&  has_color_quickdraw()) )
			{
				p7::throw_errno( ENOENT );
			}
			
			mac::qd::scoped_port thePort( window );
			
			Traits::set( color );
			
			Ped::invalidate_window( window );
		}
	};
	
	struct Access_WindowTextFont : plus::serialize_unsigned< short >
	{
		static short Get( WindowRef window )
		{
			return N::GetPortTextFont( GetWindowPort( window ) );
		}
		
		static void Set( WindowRef window, short fontID )
		{
			mac::qd::scoped_port thePort( window );
			
			::TextFont( fontID );
			
			Ped::invalidate_window( window );
		}
	};
	
	struct Access_WindowTextSize : plus::serialize_unsigned< short >
	{
		static short Get( WindowRef window )
		{
			return GetPortTextSize( GetWindowPort( window ) );
		}
		
		static void Set( WindowRef window, short size )
		{
			mac::qd::scoped_port thePort( window );
			
			::TextSize( size );
			
			Ped::invalidate_window( window );
		}
	};
	
	struct Access_WindowAlpha : plus::serialize_unsigned< short >
	{
		static short Get( WindowRef window )
		{
			return short( N::GetWindowAlpha( window ) * 10000 );
		}
		
		static void Set( WindowRef window, short alpha )
		{
			if ( uint16_t( alpha ) > 10000 )
			{
				alpha = 10000;
			}
			
			SetWindowAlpha( window, alpha / 10000.0 );
		}
	};
	
	static void select_trigger( const vfs::node* that )
	{
		vfs::trigger_extra& extra = *(vfs::trigger_extra*) that->extra();
		
		const WindowRef window = (WindowRef) extra.data;
		
		SelectWindow( window );
	}
	
	
	static WindowRef GetKeyFromParent( const vfs::node* parent )
	{
		return (WindowRef) plus::decode_32_bit_hex( parent->name() );
	}
	
	static
	WindowRef get_node_window( const vfs::node* that )
	{
		WindowRef key = GetKeyFromParent( that );
		
		if ( ! mac::sys::windowlist_contains( key ) )
		{
			p7::throw_errno( EIO );
		}
		
		return key;
	}
	
	#define DEFINE_GETTER( p )  \
	static void p##_get( plus::var_string& result, const vfs::node* that, bool binary )  \
	{  \
		typedef p Accessor;                                       \
		WindowRef key = get_node_window( that );                  \
		const Accessor::result_type data = Accessor::Get( key );  \
		Accessor::deconstruct::apply( result, data, binary );     \
	}
	
	#define DEFINE_SETTER( p )  \
	static void p##_set( const vfs::node* that, const char* begin, const char* end, bool binary )  \
	{  \
		WindowRef key = get_node_window( that );                     \
		p::Set( key, p::reconstruct::apply( begin, end, binary ) );  \
	}
	
	static
	void title_get( plus::var_string& result, const vfs::node* that, bool binary, const plus::string& name )
	{
		WindowRef window = get_node_window( that );
		
		result = N::GetWTitle( window );
		
		/*
			title
			.mac-title
			.~title
			.~mac-title
			
			The ".~" prefix sets the binary flag, so if `binary` is true, we
			need to skip two bytes.  The next byte might be either '.' or 'm'
			for MacRoman titles, but will always be 't' for UTF-8.
		*/
		
		if ( name[ binary * 2 ] == 't' )
		{
			result = plus::utf8_from_mac( result );
		}
	}
	
	static
	void set_window_title( WindowRef window, const char* begin, const char* end )
	{
		N::Str255 title( begin, end - begin );
		
		SetWTitle( window, title );
		
		mac::app::Window_menu_remove( window );
		mac::app::Window_menu_insert( window );
	}
	
	static inline
	void set_window_title( WindowRef window, const plus::string& name )
	{
		return set_window_title( window, name.begin(), name.end() );
	}
	
	static
	void title_set( const vfs::node* that, const char* begin, const char* end, bool binary, const plus::string& name )
	{
		WindowRef window = get_node_window( that );
		
		if ( name[ binary * 2 ] != 't' )
		{
			set_window_title( window, begin, end );
		}
		else
		{
			plus::string mac_text = plus::mac_from_utf8( begin, end - begin );
			
			set_window_title( window, mac_text );
		}
	}
	
	
	static vfs::node_ptr window_trigger_factory( const vfs::node*     parent,
	                                             const plus::string&  name,
	                                             const void*          args )
	{
		const WindowRef window = GetKeyFromParent( parent );
		
		const vfs::trigger_extra extra = { &select_trigger, (intptr_t) window };
		
		return vfs::trigger_factory( parent, name, &extra );
	}
	
	
	#define PROPERTY( prop )  &vfs::new_property, &prop##_params
	
	#define PROPERTY_CONST_ACCESS( access )  PROPERTY( access )
	#define PROPERTY_ACCESS( access )        PROPERTY( access )
	
	static const vfs::property_params window_title_params =
	{
		vfs::no_fixed_size,
		(vfs::property_get_hook) &title_get,
		(vfs::property_set_hook) &title_set,
	};
	
	typedef Access_WindowColor< back_color_traits > Access_WindowBackColor;
	typedef Access_WindowColor< fore_color_traits > Access_WindowForeColor;
	
	DEFINE_GETTER( Access_WindowPosition  );
	DEFINE_GETTER( Access_WindowSize      );
	DEFINE_GETTER( Access_WindowVisible   );
	DEFINE_GETTER( Access_WindowTextFont  );
	DEFINE_GETTER( Access_WindowTextSize  );
	DEFINE_GETTER( Access_WindowBackColor );
	DEFINE_GETTER( Access_WindowForeColor );
	DEFINE_GETTER( Access_WindowAlpha     );
	DEFINE_GETTER( Access_WindowZOrder    );
	
	DEFINE_SETTER( Access_WindowPosition  );
	DEFINE_SETTER( Access_WindowSize      );
	DEFINE_SETTER( Access_WindowVisible   );
	DEFINE_SETTER( Access_WindowTextFont  );
	DEFINE_SETTER( Access_WindowTextSize  );
	DEFINE_SETTER( Access_WindowBackColor );
	DEFINE_SETTER( Access_WindowForeColor );
	DEFINE_SETTER( Access_WindowAlpha     );
	
	#define DEFINE_PARAMS( p )  \
	static const vfs::property_params p##_params = {p::fixed_size, &p##_get, &p##_set}
	
	DEFINE_PARAMS( Access_WindowPosition  );
	DEFINE_PARAMS( Access_WindowSize      );
	DEFINE_PARAMS( Access_WindowVisible   );
	DEFINE_PARAMS( Access_WindowTextFont  );
	DEFINE_PARAMS( Access_WindowTextSize  );
	DEFINE_PARAMS( Access_WindowBackColor );
	DEFINE_PARAMS( Access_WindowForeColor );
	DEFINE_PARAMS( Access_WindowAlpha     );
	
	static const vfs::property_params Access_WindowZOrder_params =
	{
		Access_WindowZOrder::fixed_size,
		&Access_WindowZOrder_get,
	};
	
	const vfs::fixed_mapping sys_app_window_list_REF_Mappings[] =
	{
		{ ".mac-title",  PROPERTY( window_title ) },
		{      "title",  PROPERTY( window_title ) },
		{ ".~mac-title", PROPERTY( window_title ) },
		{     ".~title", PROPERTY( window_title ) },
	
		{ "pos",   PROPERTY_ACCESS( Access_WindowPosition ) },
		{ "size",  PROPERTY_ACCESS( Access_WindowSize     ) },
		{ "vis",   PROPERTY_ACCESS( Access_WindowVisible  ) },
		
		{ ".~pos",   PROPERTY_ACCESS( Access_WindowPosition ) },
		{ ".~size",  PROPERTY_ACCESS( Access_WindowSize     ) },
		{ ".~vis",   PROPERTY_ACCESS( Access_WindowVisible  ) },
		
		{ "text-font",  PROPERTY_ACCESS( Access_WindowTextFont ) },
		{ "text-size",  PROPERTY_ACCESS( Access_WindowTextSize ) },
		
		{ ".~text-font",  PROPERTY_ACCESS( Access_WindowTextFont ) },
		{ ".~text-size",  PROPERTY_ACCESS( Access_WindowTextSize ) },
		
	#if CONFIG_COLOR
		
		{ "back-color", PROPERTY_ACCESS( Access_WindowBackColor ) },
		{ "fore-color", PROPERTY_ACCESS( Access_WindowForeColor ) },
		
		{ ".~back-color", PROPERTY_ACCESS( Access_WindowBackColor ) },
		{ ".~fore-color", PROPERTY_ACCESS( Access_WindowForeColor ) },
		
	#endif
		
	#if TARGET_API_MAC_OSX
		
		{ "alpha",  PROPERTY_ACCESS( Access_WindowAlpha ) },
		
	#endif
		
		{ "select", &window_trigger_factory },
		
		{ "z", PROPERTY_CONST_ACCESS( Access_WindowZOrder ) },
		
		{ ".~z", PROPERTY_CONST_ACCESS( Access_WindowZOrder ) },
		
		{ NULL, NULL }
	};
	
}
