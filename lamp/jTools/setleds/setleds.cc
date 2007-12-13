/*	==========
 *	setleds.cc
 *	==========
 */

// Standard C/C++
#include <cstdio>

// Iota
#include "iota/strings.hh"

// Arcana
#if !TARGET_API_MAC_CARBON
#include "ADBKeyboardLEDs.hh"
#endif

// POSeven
#include "POSeven/FileDescriptor.hh"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"


namespace N = Nitrogen;
namespace p7 = poseven;
namespace O = Orion;


#if !TARGET_API_MAC_CARBON

#include "Nucleus/IndexedContainer.h"

namespace Nitrogen
{
	
	struct ADBDevice_Container_Specifics
	{
		typedef UInt16             size_type;
		typedef SInt16             difference_type;
		typedef GetIndADB_Result   value_type;
		typedef const value_type&  const_reference;
		typedef const value_type*  const_pointer;
		
		class Nothing {};
		
		typedef Nothing ConstIteratorState;
		typedef Nothing ConstContainerState;
		
		
		static size_type Size( ConstContainerState )
		{
			return CountADBs();
		}
		
		static const_reference GetReference( ConstIteratorState state, size_type position )
		{
			return GetIndADB( position + 1 );
		}
		
		static const_pointer GetPointer( ConstIteratorState state, size_type position )
		{
			return &GetReference( state, position );
		}
	};
	
	class ADBDevice_Container : public Nucleus::ConstIndexedContainer< ADBDevice_Container_Specifics >
	{
		public:
			ADBDevice_Container() : ConstIndexedContainer( ADBDevice_Container_Specifics::Nothing() )  {}
	};
	
}


const UInt8 kLEDValueMask = 7;

static int gChangedBits     = 0;
static int gChangedBitsMask = 0;


inline bool IsKeyboard( const ADBDataBlock& adbData )
{
	const SInt8 kADBKeyboardDevice = 2;
	
	return adbData.devType == kADBKeyboardDevice;
}


static void PrintLEDs( UInt8 leds )
{
	const char* status[] = { "off", "on " };
	
	bool num    = leds & 1;
	bool caps   = leds & 2;
	bool scroll = leds & 4;
	
	std::string message;
	
	/*
	message += "# Device at ADB address ";
	
	message += address + '0';
	message += "\n";
	*/
	
	message += "Current default flags:  NumLock on    CapsLock off   ScrollLock off\n";
	message += "Current flags:          NumLock on    CapsLock off   ScrollLock off\n";
	message += "Current leds:           NumLock ";
	
	message += status[ num    ];
	message += "   " "CapsLock ";
	message += status[ caps   ];
	message += "   " "ScrollLock ";
	message += status[ scroll ];
	message += "\n";
	
	p7::write( p7::stdout_fileno, message.data(), message.size() );
}

static void DoLEDs( N::GetIndADB_Result adbDevice, int leds )
{
	if ( IsKeyboard( adbDevice ) )
	{
		UInt8 leds = GetLEDs( adbDevice );
		
		if ( gChangedBitsMask == 0 )
		{
			PrintLEDs( leds );
		}
		else
		{
			leds &= ~gChangedBitsMask;
			leds |= gChangedBits;
			
			SetLEDs( adbDevice, leds );
		}
	}
}

static const char* gOptionNames[] = { "num", "caps", "scroll", NULL };

#endif


int O::Main( int argc, char const *const argv[] )
{
#if TARGET_API_MAC_CARBON
	
	p7::write( p7::stderr_fileno, STR_LEN( "setleds: ADB unsupported in Carbon\n" ) );
	
	return EXIT_FAILURE;
	
#else
	
	int count = N::CountADBs();
	
	for ( iota::argp_t argp = argv + 1;  *argp != NULL;  ++argp )
	{
		const char* arg = *argp;
		
		bool on = false;
		
		if ( arg[0] == '+' )
		{
			on = true;
		}
		else if ( arg[0] != '-' )
		{
			std::fprintf( stderr, "setleds: unrecognized argument '%s'\n", arg );
			
			return 2;
		}
		
		for ( const char** pp = gOptionNames;  ;  ++pp )
		{
			if ( std::strcmp( *pp, arg + 1 ) == 0 )
			{
				int bit = pp - gOptionNames;
				
				gChangedBits     |= on << bit;
				gChangedBitsMask |= 1  << bit;
				
				break;
			}
			
			if ( *pp == NULL )
			{
				std::fprintf( stderr, "setleds: unrecognized led '%s'\n", arg );
				
				return 2;
			}
		}
	}
	
	bool setting = argc >= 2;
	
	int leds = setting ? std::atoi( argv[1] ) & kLEDValueMask
	                   : -1;
	
	N::ADBDevice_Container adbs;
	
	std::for_each( adbs.begin(),
	               adbs.end(),
	               std::bind2nd( std::ptr_fun( DoLEDs ),
	                             leds ) );
	
	return 0;
	
#endif
}

