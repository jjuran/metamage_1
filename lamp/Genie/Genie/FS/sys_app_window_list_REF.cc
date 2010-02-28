/*	==========================
 *	sys_app_window_list_REF.cc
 *	==========================
 */

#include "Genie/FS/sys_app_window_list_REF.hh"

// Standard C/C++
#include <cstring>

// iota
#include "iota/hexidecimal.hh"

// plus
#include "plus/hexidecimal.hh"

// Nucleus
#include "Nucleus/Saved.h"

// poseven
#include "poseven/types/errno_t.hh"

// ClassicToolbox
#include "ClassicToolbox/MacWindows.hh"

// Pedestal
#include "Pedestal/Window.hh"

// Genie
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/Scribes.hh"
#include "Genie/FS/Trigger.hh"
#include "Genie/Utilities/WindowList_contains.hh"


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
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	template < class scribe >
	struct serialization_via_scribe
	{
		typedef scribe Scribe;
		
		typedef typename Scribe::Value value_type;
		
		static const size_t fixed_size = sizeof (value_type);
		
		static std::string freeze( const value_type& value, bool binary )
		{
			return Freeze< Scribe >( value, binary );
		}
		
		static value_type vivify( const char* begin, const char* end, bool binary )
		{
			return Vivify< Scribe >( begin, end, binary );
		}
	};
	
	struct serialization_for_Str255
	{
		static const size_t fixed_size = 0;
		
		static std::string freeze( ConstStr255Param param, bool binary )
		{
			return std::string( (const char*) &param[1], param[0] );
		}
		
		static N::Str255 vivify( const char* begin, const char* end, bool binary )
		{
			return N::Str255( begin, end - begin );
		}
	};
	
	
	struct Access_WindowTitle : serialization_for_Str255
	{
		static N::Str255 Get( N::WindowRef window )
		{
			return N::GetWTitle( window );
		}
		
		static void Set( N::WindowRef window, ConstStr255Param title )
		{
			N::SetWTitle( window, title );
		}
	};
	
	struct Access_WindowPosition : serialization_via_scribe< Point_Scribe< ',' > >
	{
		static Point Get( N::WindowRef window )
		{
			return Ped::GetWindowPosition( window );
		}
		
		static void Set( N::WindowRef window, Point position )
		{
			Ped::SetWindowPosition( window, position );
		}
	};
	
	struct Access_WindowSize : serialization_via_scribe< Point_Scribe< 'x' > >
	{
		static Point Get( N::WindowRef window )
		{
			return Ped::GetWindowSize( window );
		}
		
		static void Set( N::WindowRef window, Point size )
		{
			Ped::SetWindowSize( window, size );
		}
	};
	
	struct Access_WindowVisible : serialization_via_scribe< Boolean_Scribe >
	{
		static bool Get( N::WindowRef window )
		{
			return N::IsWindowVisible( window );
		}
		
		static void Set( N::WindowRef window, bool visible )
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
	
	struct Access_WindowZOrder : serialization_via_scribe< Integer_Scribe< unsigned > >
	{
		static unsigned Get( N::WindowRef window )
		{
			unsigned z = 0;
			
			for ( N::WindowRef it = N::GetWindowList();  it != window;  ++z, it = N::GetNextWindow( it ) )
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
		const unsigned char nibble = iota::decoded_hex_digit( p[ 0 ] );
		
		UInt16 result = nibble << 12
		              | nibble <<  8
		              | nibble <<  4
		              | nibble <<  0;
		
		return result;
	}
	
	static inline UInt16 ReadIntensity_2n( const char* p )
	{
		const unsigned char high = iota::decoded_hex_digit( p[ 0 ] );
		const unsigned char low  = iota::decoded_hex_digit( p[ 1 ] );
		
		UInt16 result = high << 12
		              | low  <<  8
		              | high <<  4
		              | low  <<  0;
		
		return result;
	}
	
	static inline UInt16 ReadIntensity_3n( const char* p )
	{
		const unsigned char high = iota::decoded_hex_digit( p[ 0 ] );
		const unsigned char med  = iota::decoded_hex_digit( p[ 1 ] );
		const unsigned char low  = iota::decoded_hex_digit( p[ 2 ] );
		
		UInt16 result = high << 12
		              | med  <<  8
		              | low  <<  4
		              | high <<  0;
		
		return result;
	}
	
	static UInt16 ReadIntensity_4n( const char* p )
	{
		const UInt16 result = iota::decoded_hex_digit( p[ 0 ] ) << 12
		                    | iota::decoded_hex_digit( p[ 1 ] ) <<  8
		                    | iota::decoded_hex_digit( p[ 2 ] ) <<  4
		                    | iota::decoded_hex_digit( p[ 3 ] ) <<  0;
		
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
	
	static std::string WriteColor( const RGBColor& color )
	{
		char encoded[] = "#rrrrggggbbbb";
		
		std::sprintf( encoded + 1, "%.4x%.4x%.4x", color.red,
		                                           color.green,
		                                           color.blue );
		
		return encoded;
	}
	
	struct RGBColor_Scribe
	{
		typedef RGBColor Value;
		
		static std::string Encode( const RGBColor& value )
		{
			return WriteColor( value );
		}
		
		static RGBColor Decode( const char* begin, const char* end )
		{
			return ReadColor( begin, end );
		}
	};
	
	template < RGBColor (*GetColor)(N::CGrafPtr), void (*SetColor)(const RGBColor&) >
	struct Access_WindowColor : serialization_via_scribe< RGBColor_Scribe >
	{
		static RGBColor Get( N::WindowRef window )
		{
			return GetColor( N::GetWindowPort( window ) );
		}
		
		static void Set( N::WindowRef window, const RGBColor& color )
		{
			NN::Saved< N::Port_Value > savePort;
			
			N::SetPortWindowPort( window );
			
			SetColor( color );
			
			N::InvalRect( N::GetPortBounds( N::GetWindowPort( window ) ) );
		}
	};
	
	struct Access_WindowTextFont : serialization_via_scribe< Integer_Scribe< short > >
	{
		static short Get( N::WindowRef window )
		{
			return N::GetPortTextFont( N::GetWindowPort( window ) );
		}
		
		static void Set( N::WindowRef window, short fontID )
		{
			NN::Saved< N::Port_Value > savePort;
			
			N::SetPortWindowPort( window );
			
			::TextFont( fontID );
			
			N::InvalRect( N::GetPortBounds( N::GetWindowPort( window ) ) );
		}
	};
	
	struct Access_WindowTextSize : serialization_via_scribe< Integer_Scribe< short > >
	{
		static short Get( N::WindowRef window )
		{
			return N::GetPortTextSize( N::GetWindowPort( window ) );
		}
		
		static void Set( N::WindowRef window, short size )
		{
			NN::Saved< N::Port_Value > savePort;
			
			N::SetPortWindowPort( window );
			
			::TextSize( size );
			
			N::InvalRect( N::GetPortBounds( N::GetWindowPort( window ) ) );
		}
	};
	
	class Select_Trigger
	{
		private:
			typedef N::WindowRef Key;
			
			Key itsKey;
		
		public:
			Select_Trigger( Key key ) : itsKey( key )
			{
			}
			
			void operator()() const
			{
				N::SelectWindow( itsKey );
			}
	};
	
	
	static N::WindowRef GetKeyFromParent( const FSTreePtr& parent )
	{
		return (N::WindowRef) plus::decode_32_bit_hex( parent->Name() );
	}
	
	static N::WindowRef GetKey( const FSTree* that )
	{
		return GetKeyFromParent( that->ParentRef() );
	}
	
	template < class Accessor >
	struct sys_app_window_list_REF_Property
	{
		typedef N::WindowRef Key;
		
		static std::string Read( const FSTree* that, bool binary )
		{
			Key key = GetKey( that );
			
			if ( !WindowList_contains( key ) )
			{
				p7::throw_errno( EIO );
			}
			
			return Accessor::freeze( Accessor::Get( key ), binary );
		}
		
		static void Write( const FSTree* that, const char* begin, const char* end, bool binary )
		{
			Key key = GetKey( that );
			
			if ( !WindowList_contains( key ) )
			{
				p7::throw_errno( EIO );
			}
			
			Accessor::Set( key, Accessor::vivify( begin, end, binary ) );
		}
	};
	
	
	template < class Trigger >
	static FSTreePtr Trigger_Factory( const FSTreePtr&    parent,
	                                  const std::string&  name )
	{
		N::WindowRef key = GetKeyFromParent( parent );
		
		return seize_ptr( new Trigger( parent, name, key ) );
	}
	
	template < class Accessor >
	static FSTreePtr Const_Property_Factory( const FSTreePtr&    parent,
	                                         const std::string&  name )
	{
		typedef sys_app_window_list_REF_Property< Accessor > Property;
		
		return New_FSTree_Property( parent,
		                            name,
		                            Accessor::fixed_size,
		                            &Property::Read );
	}
	
	template < class Accessor >
	static FSTreePtr Property_Factory( const FSTreePtr&    parent,
	                                   const std::string&  name )
	{
		typedef sys_app_window_list_REF_Property< Accessor > Property;
		
		return New_FSTree_Property( parent,
		                            name,
		                            Accessor::fixed_size,
		                            &Property::Read,
		                            &Property::Write );
	}
	
	const FSTree_Premapped::Mapping sys_app_window_list_REF_Mappings[] =
	{
		{ "title", &Property_Factory< Access_WindowTitle    > },
		{ "pos",   &Property_Factory< Access_WindowPosition > },
		{ "size",  &Property_Factory< Access_WindowSize     > },
		{ "vis",   &Property_Factory< Access_WindowVisible  > },
		
		{ "text-font",  &Property_Factory< Access_WindowTextFont > },
		{ "text-size",  &Property_Factory< Access_WindowTextSize > },
		
		{ "back-color", &Property_Factory< Access_WindowColor< N::GetPortBackColor, N::RGBBackColor > > },
		{ "fore-color", &Property_Factory< Access_WindowColor< N::GetPortForeColor, N::RGBForeColor > > },
		
		{ "select", &Trigger_Factory< Trigger< Select_Trigger > > },
		
		{ "z", &Const_Property_Factory< Access_WindowZOrder > },
		
		{ NULL, NULL }
	};
	
}

