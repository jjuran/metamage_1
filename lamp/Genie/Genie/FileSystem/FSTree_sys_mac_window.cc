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
#include "Genie/FileSystem/FSTree_QueryFile.hh"
#include "Genie/FileSystem/FSTree_PseudoFile.hh"


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
	
	extern const Functional_Traits< WindowRef_KeyName_Traits::Key >::Mapping sys_mac_window_REF_Mappings[];
	
	FSTreePtr sys_mac_window_Details::GetChildNode( const FSTreePtr&    parent,
		                                            const std::string&  name,
		                                            const Key&          key )
	{
		return Premapped_Factory< Key, sys_mac_window_REF_Mappings >( parent, name, key );
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
		
		Result Get( N::WindowRef window ) const
		{
			return N::GetWTitle( window );
		}
		
		void Set( N::WindowRef window, const std::string& value )
		{
			N::SetWTitle( window, N::Str255( value ) );
		}
	};
	
	struct Access_WindowPosition
	{
		typedef std::string Result;
		
		Result Get( N::WindowRef window ) const
		{
			Point position = Ped::GetWindowPosition( window );
			
			return WritePoint( position, "," );
		}
		
		void Set( N::WindowRef window, const std::string& value )
		{
			Ped::SetWindowPosition( window, ReadPoint( value ) );
		}
	};
	
	struct Access_WindowSize
	{
		typedef std::string Result;
		
		Result Get( N::WindowRef window ) const
		{
			Point size = Ped::GetWindowSize( window );
			
			return WritePoint( size, "x" );
		}
		
		void Set( N::WindowRef window, const std::string& value )
		{
			Ped::SetWindowSize( window, ReadPoint( value ) );
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
	
	static RGBColor ReadColor( const std::string& value )
	{
		const char* p = value.c_str();
		
		size_t length = value.length();
		
		if ( value[0] == '#' )
		{
			++p;
			--length;
		}
		
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
	struct Access_WindowBackColor
	{
		typedef std::string Result;
		
		Result Get( N::WindowRef window ) const
		{
			RGBColor color = GetColor( N::GetWindowPort( window ) );
			
			return WriteColor( color );
		}
		
		void Set( N::WindowRef window, const std::string& value )
		{
			RGBColor color = ReadColor( value );
			
			NN::Saved< N::Port_Value > savePort;
			
			N::SetPortWindowPort( window );
			
			SetColor( color );
			
			N::InvalRect( N::GetPortBounds( N::GetWindowPort( window ) ) );
		}
	};
	
	template < class Accessor >
	class sys_mac_window_REF_Property
	{
		private:
			typedef N::WindowRef Key;
			
			Key itsKey;
		
		public:
			sys_mac_window_REF_Property( const Key& key ) : itsKey( key )
			{
			}
			
			std::string Get() const
			{
				if ( !sys_mac_window_Details::KeyIsValid( itsKey ) )
				{
					p7::throw_errno( EIO );
				}
				
				std::string output = NN::Convert< std::string >( Accessor().Get( itsKey ) ) + "\n";
				
				return output;
			}
			
			void Set( const std::string& value )
			{
				if ( !sys_mac_window_Details::KeyIsValid( itsKey ) )
				{
					p7::throw_errno( EIO );
				}
				
				Accessor().Set( itsKey, value );
			}
	};
	
	template < class Accessor >
	static FSTreePtr Property_Factory( const FSTreePtr&                parent,
	                                   const std::string&              name,
	                                   WindowRef_KeyName_Traits::Key   key )
	{
		typedef sys_mac_window_REF_Property< Accessor > Property;
		
		typedef FSTree_PseudoFile< Property > QueryFile;
		
		return MakeFSTree( new QueryFile( parent, name, Property( key ) ) );
	}
	
	const Functional_Traits< WindowRef_KeyName_Traits::Key >::Mapping sys_mac_window_REF_Mappings[] =
	{
		{ "title", &Property_Factory< Access_WindowTitle    > },
		{ "pos",   &Property_Factory< Access_WindowPosition > },
		{ "size",  &Property_Factory< Access_WindowSize     > },
		
		{ "back-color", &Property_Factory< Access_WindowBackColor< N::GetPortBackColor, N::RGBBackColor > > },
		{ "fore-color", &Property_Factory< Access_WindowBackColor< N::GetPortForeColor, N::RGBForeColor > > },
		
		{ NULL, NULL }
	};
	
}

