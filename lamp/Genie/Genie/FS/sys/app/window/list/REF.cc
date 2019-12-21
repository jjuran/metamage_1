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

// mac-sys-utils
#include "mac_sys/windowlist_contains.hh"

// mac-ui-utils
#include "mac_ui/windows.hh"

// gear
#include "gear/hexadecimal.hh"

// plus
#include "plus/deconstruct.hh"
#include "plus/hexadecimal.hh"
#include "plus/reconstruct.hh"
#include "plus/serialize.hh"
#include "plus/var_string.hh"

// nucleus
#include "nucleus/saved.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/ATSTypes.hh"
#include "Nitrogen/Quickdraw.hh"

// ClassicToolbox
#include "ClassicToolbox/MacWindows.hh"

// MacFeatures
#include "MacFeatures/ColorQuickdraw.hh"

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
#include "Genie/FS/utf8_text_property.hh"


namespace Nitrogen
{
	
	inline void TextFont( FontID font )
	{
		::TextFont( font );
	}
	
	using ::TextSize;
	
	inline FontID GetPortTextFont( CGrafPtr port )
	{
		return FontID( ::GetPortTextFont( port ) );
	}
	
	using ::GetPortTextSize;
	
	inline void SetPortTextFont( CGrafPtr port, FontID font )
	{
		::SetPortTextFont( port, font );
	}
	
	using ::SetPortTextSize;
	
	static inline
	void SetWindowAlpha( WindowRef window, float alpha )
	{
		::SetWindowAlpha( window, alpha );
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
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	struct Access_WindowTitle : serialize_Str255_contents
	{
		static N::Str255 Get( WindowRef window )
		{
			return N::GetWTitle( window );
		}
		
		static void Set( WindowRef window, ConstStr255Param title )
		{
			N::SetWTitle( window, title );
		}
	};
	
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
				N::ShowWindow( window );
			}
			else
			{
				N::HideWindow( window );
			}
		}
	};
	
	struct Access_WindowZOrder : plus::serialize_unsigned< unsigned >
	{
		static unsigned Get( WindowRef window )
		{
			unsigned z = 0;
			
			for ( WindowRef it = N::GetWindowList();  it != window;  ++z, it = N::GetNextWindow( it ) )
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
	
	template < RGBColor (*GetColor)(CGrafPtr), void (*SetColor)(const RGBColor&) >
	struct Access_WindowColor : serialize_RGBColor
	{
		static RGBColor Get( WindowRef window )
		{
			if ( !MacFeatures::Has_ColorQuickdraw() )
			{
				p7::throw_errno( ENOENT );
			}
			
			return GetColor( N::GetWindowPort( window ) );
		}
		
		static void Set( WindowRef window, const RGBColor& color )
		{
			if ( !MacFeatures::Has_ColorQuickdraw() )
			{
				p7::throw_errno( ENOENT );
			}
			
			n::saved< N::Port > savePort;
			
			N::SetPortWindowPort( window );
			
			SetColor( color );
			
			Ped::invalidate_window( window );
		}
	};
	
	struct Access_WindowTextFont : plus::serialize_unsigned< short >
	{
		static short Get( WindowRef window )
		{
			return N::GetPortTextFont( N::GetWindowPort( window ) );
		}
		
		static void Set( WindowRef window, short fontID )
		{
			n::saved< N::Port > savePort;
			
			N::SetPortWindowPort( window );
			
			::TextFont( fontID );
			
			Ped::invalidate_window( window );
		}
	};
	
	struct Access_WindowTextSize : plus::serialize_unsigned< short >
	{
		static short Get( WindowRef window )
		{
			return N::GetPortTextSize( N::GetWindowPort( window ) );
		}
		
		static void Set( WindowRef window, short size )
		{
			n::saved< N::Port > savePort;
			
			N::SetPortWindowPort( window );
			
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
			
			N::SetWindowAlpha( window, alpha / 10000.0 );
		}
	};
	
	static void select_trigger( const vfs::node* that )
	{
		vfs::trigger_extra& extra = *(vfs::trigger_extra*) that->extra();
		
		const WindowRef window = (WindowRef) extra.data;
		
		N::SelectWindow( window );
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
	
	template < class Accessor >
	struct sys_app_window_list_REF_Const_Property : vfs::readonly_property
	{
		static const int fixed_size = Accessor::fixed_size;
		
		typedef WindowRef Key;
		
		static void get( plus::var_string& result, const vfs::node* that, bool binary )
		{
			Key key = get_node_window( that );
			
			typedef typename Accessor::result_type result_type;
			
			const result_type data = Accessor::Get( key );
			
			Accessor::deconstruct::apply( result, data, binary );
		}
	};
	
	template < class Accessor >
	struct sys_app_window_list_REF_Property : sys_app_window_list_REF_Const_Property< Accessor >
	{
		static const bool can_set = true;
		
		static const int fixed_size = Accessor::fixed_size;
		
		typedef WindowRef Key;
		
		static void set( const vfs::node* that, const char* begin, const char* end, bool binary )
		{
			Key key = get_node_window( that );
			
			Accessor::Set( key, Accessor::reconstruct::apply( begin, end, binary ) );
		}
	};
	
	
	static vfs::node_ptr window_trigger_factory( const vfs::node*     parent,
	                                             const plus::string&  name,
	                                             const void*          args )
	{
		const WindowRef window = GetKeyFromParent( parent );
		
		const vfs::trigger_extra extra = { &select_trigger, (intptr_t) window };
		
		return vfs::trigger_factory( parent, name, &extra );
	}
	
	
	#define PROPERTY( prop )  &vfs::new_property, &vfs::property_params_factory< prop >::value
	
	#define PROPERTY_CONST_ACCESS( access )  PROPERTY( sys_app_window_list_REF_Const_Property< access > )
	
	#define PROPERTY_ACCESS( access )  PROPERTY( sys_app_window_list_REF_Property< access > )
	
	typedef sys_app_window_list_REF_Property< Access_WindowTitle > window_title;
	
	typedef Access_WindowColor< N::GetPortBackColor, N::RGBBackColor > Access_WindowBackColor;
	typedef Access_WindowColor< N::GetPortForeColor, N::RGBForeColor > Access_WindowForeColor;
	
	const vfs::fixed_mapping sys_app_window_list_REF_Mappings[] =
	{
		{ ".mac-title", PROPERTY(                     window_title   ) },
		{      "title", PROPERTY( utf8_text_property< window_title > ) },
		
		{ ".~mac-title", PROPERTY(                     window_title   ) },
		{     ".~title", PROPERTY( utf8_text_property< window_title > ) },
		
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
