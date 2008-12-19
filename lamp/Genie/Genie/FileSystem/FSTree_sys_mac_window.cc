/*	========================
 *	FSTree_sys_mac_window.cc
 *	========================
 */

#include "Genie/FileSystem/FSTree_sys_mac_window.hh"

// Standard C++
#include <algorithm>

// Standard C/C++
#include <cstring>

// Nucleus
#include "Nucleus/Convert.h"
#include "Nucleus/Saved.h"

// ClassicToolbox
#include "ClassicToolbox/MacWindows.h"

// Pedestal
#include "Pedestal/Window.hh"

// Genie
#include "Genie/FileSystem/FSTree_Property.hh"


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
	
	
	bool sys_mac_window_Details::KeyIsValid( const Key& key )
	{
		Sequence sequence = ItemSequence();
		
		return std::find( sequence.begin(), sequence.end(), key ) != sequence.end();
	}
	
	extern const Functional_Traits< void >::Mapping sys_mac_window_REF_Mappings[];
	
	FSTreePtr sys_mac_window_Details::GetChildNode( const FSTreePtr&    parent,
		                                            const std::string&  name,
		                                            const Key&          key )
	{
		return Premapped_Factory< sys_mac_window_REF_Mappings >( parent, name );
	}
	
	
	static inline bool is_integer( const char* s )
	{
		return std::isdigit( s[ s[0] == '-' ] );
	}
	
	static Point ReadPoint( const std::string& value )
	{
		const char* p = value.c_str();
		
		long x = std::strtol( p, (char**) &p, 10 );
		
		if ( p != value.c_str()  &&  *p != '\0' )
		{
			while ( *++p )
			{
				if ( is_integer( p ) )
				{
					long y = std::strtol( p, NULL, 10 );
					
					Point result = { y, x };
					
					return result;
				}
			}
		}
		
		throw p7::errno_t( EINVAL );
	}
	
	static std::string WritePoint( Point point, const char* separator )
	{
		std::string result = NN::Convert< std::string >( point.h );
		
		result += separator;
		
		result += NN::Convert< std::string >( point.v );
		
		return result;
	}
	
	struct Access_WindowTitle
	{
		typedef N::Str255 Result;
		
		static Result Get( N::WindowRef window )
		{
			return N::GetWTitle( window );
		}
		
		static void Set( N::WindowRef window, const char* begin, const char* end )
		{
			N::SetWTitle( window, N::Str255( begin, end - begin ) );
		}
	};
	
	struct Access_WindowPosition
	{
		typedef std::string Result;
		
		static Result Get( N::WindowRef window )
		{
			Point position = Ped::GetWindowPosition( window );
			
			return WritePoint( position, "," );
		}
		
		static void Set( N::WindowRef window, const char* begin, const char* end )
		{
			Ped::SetWindowPosition( window, ReadPoint( begin ) );
		}
	};
	
	struct Access_WindowSize
	{
		typedef std::string Result;
		
		static Result Get( N::WindowRef window )
		{
			Point size = Ped::GetWindowSize( window );
			
			return WritePoint( size, "x" );
		}
		
		static void Set( N::WindowRef window, const char* begin, const char* end )
		{
			Ped::SetWindowSize( window, ReadPoint( begin ) );
		}
	};
	
	struct Access_WindowVisible
	{
		typedef std::string Result;
		
		static Result Get( N::WindowRef window )
		{
			const bool visible = N::IsWindowVisible( window );
			
			return visible ? "1" : "0";
		}
		
		static void Set( N::WindowRef window, const char* begin, const char* end )
		{
			const bool visible = begin[ 0 ] != '0';
			
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
	
	struct Access_WindowZOrder
	{
		typedef unsigned Result;
		
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
	
	inline unsigned char nibble_from_ascii( char c )
	{
		return c & 0x10 ?  c         - '0'
		                : (c | 0x20) - 'a' + 10;
	}
	
	inline UInt16 ReadIntensity_1n( const char* p )
	{
		const unsigned char nibble = nibble_from_ascii( p[ 0 ] );
		
		UInt16 result = nibble << 12
		              | nibble <<  8
		              | nibble <<  4
		              | nibble <<  0;
		
		return result;
	}
	
	inline UInt16 ReadIntensity_2n( const char* p )
	{
		const unsigned char high = nibble_from_ascii( p[ 0 ] );
		const unsigned char low  = nibble_from_ascii( p[ 1 ] );
		
		UInt16 result = high << 12
		              | low  <<  8
		              | high <<  4
		              | low  <<  0;
		
		return result;
	}
	
	inline UInt16 ReadIntensity_3n( const char* p )
	{
		const unsigned char high = nibble_from_ascii( p[ 0 ] );
		const unsigned char med  = nibble_from_ascii( p[ 1 ] );
		const unsigned char low  = nibble_from_ascii( p[ 2 ] );
		
		UInt16 result = high << 12
		              | med  <<  8
		              | low  <<  4
		              | high <<  0;
		
		return result;
	}
	
	static UInt16 ReadIntensity_4n( const char* p )
	{
		UInt16 result = nibble_from_ascii( p[ 0 ] ) << 12
		              | nibble_from_ascii( p[ 1 ] ) <<  8
		              | nibble_from_ascii( p[ 2 ] ) <<  4
		              | nibble_from_ascii( p[ 3 ] ) <<  0;
		
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
	
	template < RGBColor (*GetColor)(N::CGrafPtr), void (*SetColor)(const RGBColor&) >
	struct Access_WindowColor
	{
		typedef std::string Result;
		
		static Result Get( N::WindowRef window )
		{
			RGBColor color = GetColor( N::GetWindowPort( window ) );
			
			return WriteColor( color );
		}
		
		static void Set( N::WindowRef window, const char* begin, const char* end )
		{
			RGBColor color = ReadColor( begin, end );
			
			NN::Saved< N::Port_Value > savePort;
			
			N::SetPortWindowPort( window );
			
			SetColor( color );
			
			N::InvalRect( N::GetPortBounds( N::GetWindowPort( window ) ) );
		}
	};
	
	struct Access_WindowTextFont
	{
		typedef short Result;
		
		static short Get( N::WindowRef window )
		{
			return N::GetPortTextFont( N::GetWindowPort( window ) );
		}
		
		static void Set( N::WindowRef window, const char* begin, const char* end )
		{
			short fontID = std::atoi( begin );
			
			NN::Saved< N::Port_Value > savePort;
			
			N::SetPortWindowPort( window );
			
			::TextFont( fontID );
			
			N::InvalRect( N::GetPortBounds( N::GetWindowPort( window ) ) );
		}
	};
	
	struct Access_WindowTextSize
	{
		typedef short Result;
		
		static short Get( N::WindowRef window )
		{
			return N::GetPortTextSize( N::GetWindowPort( window ) );
		}
		
		static void Set( N::WindowRef window, const char* begin, const char* end )
		{
			short size = std::atoi( begin );
			
			NN::Saved< N::Port_Value > savePort;
			
			N::SetPortWindowPort( window );
			
			::TextSize( size );
			
			N::InvalRect( N::GetPortBounds( N::GetWindowPort( window ) ) );
		}
	};
	
	template < class Accessor >
	struct sys_mac_window_REF_Property
	{
		typedef N::WindowRef Key;
		
		static std::string Read( Key key )
		{
			if ( !sys_mac_window_Details::KeyIsValid( key ) )
			{
				p7::throw_errno( EIO );
			}
			
			return NN::Convert< std::string >( Accessor::Get( key ) );
		}
		
		static void Write( Key key, const char* begin, const char* end )
		{
			if ( !sys_mac_window_Details::KeyIsValid( key ) )
			{
				p7::throw_errno( EIO );
			}
			
			Accessor::Set( key, begin, end );
		}
	};
	
	
	static inline UInt32 Read_8_nibbles( const char* p )
	{
		UInt32 result = nibble_from_ascii( p[ 0 ] ) << 28
		              | nibble_from_ascii( p[ 1 ] ) << 24
		              | nibble_from_ascii( p[ 2 ] ) << 20
		              | nibble_from_ascii( p[ 3 ] ) << 16
		              | nibble_from_ascii( p[ 4 ] ) << 12
		              | nibble_from_ascii( p[ 5 ] ) <<  8
		              | nibble_from_ascii( p[ 6 ] ) <<  4
		              | nibble_from_ascii( p[ 7 ] ) <<  0;
		
		return result;
	}
	
	
	static inline void* PtrFromName( const std::string& name )
	{
		if ( name.length() != sizeof (void*) * 2 )
		{
			return NULL;
		}
		
		return (void*) Read_8_nibbles( name.data() );
	}
	
	static N::WindowRef GetKey( const FSTree* that )
	{
		return (N::WindowRef) PtrFromName( that->ParentRef()->Name() );
	}
	
	template < class Accessor >
	static FSTreePtr Const_Property_Factory( const FSTreePtr&    parent,
	                                         const std::string&  name )
	{
		typedef sys_mac_window_REF_Property< Accessor > Property;
		
		return FSTreePtr( new FSTree_Property( parent,
		                                       name,
		                                       &GetKey,
		                                       &Property::Read ) );
	}
	
	template < class Accessor >
	static FSTreePtr Property_Factory( const FSTreePtr&    parent,
	                                   const std::string&  name )
	{
		typedef sys_mac_window_REF_Property< Accessor > Property;
		
		return FSTreePtr( new FSTree_Property( parent,
		                                       name,
		                                       &GetKey,
		                                       &Property::Read,
		                                       &Property::Write ) );
	}
	
	const Functional_Traits< void >::Mapping sys_mac_window_REF_Mappings[] =
	{
		{ "title", &Property_Factory< Access_WindowTitle    > },
		{ "pos",   &Property_Factory< Access_WindowPosition > },
		{ "size",  &Property_Factory< Access_WindowSize     > },
		{ "vis",   &Property_Factory< Access_WindowVisible  > },
		
		{ "text-font",  &Property_Factory< Access_WindowTextFont > },
		{ "text-size",  &Property_Factory< Access_WindowTextSize > },
		
		{ "back-color", &Property_Factory< Access_WindowColor< N::GetPortBackColor, N::RGBBackColor > > },
		{ "fore-color", &Property_Factory< Access_WindowColor< N::GetPortForeColor, N::RGBForeColor > > },
		
		{ "z", &Const_Property_Factory< Access_WindowZOrder > },
		
		{ NULL, NULL }
	};
	
}

