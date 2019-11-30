/*
	Genie/FS/sys/cpu.cc
	-------------------
*/

#include "Genie/FS/sys/cpu.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __GESTALT__
#include <Gestalt.h>
#endif

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// gear
#include "gear/inscribe_decimal.hh"

// Debug
#include "debug/assert.hh"

// plus
#include "plus/var_string.hh"

// vfs
#include "vfs/property.hh"
#include "vfs/node/types/property_file.hh"


#if UNIVERSAL_INTERFACES_VERSION <= 0x0342

enum
{
	gestaltCPUApollo = 0x0111,
	gestaltCPU750FX  = 0x0120,
};

#endif

#ifndef MAC_OS_X_VERSION_10_3

enum
{
	gestaltCPUG47447 = 0x0112,
	gestaltCPU970    = 0x0139,
	gestaltCPU970FX  = 0x013c,
	gestaltCPU970MP  = 0x0144,
};

#endif

namespace Genie
{
	
	static long GetProcCode()
	{
		return mac::sys::gestalt( 'proc' ) - 1;
	}
	
	static const char* Get68KCPUName( long code )
	{
		return gear::inscribe_decimal( 68000 + 10 * code );
	}
	
	struct GetCPUProc
	{
		typedef const char* Result;
		
		static Result Get()
		{
			return Get68KCPUName( GetProcCode() );
		}
	};
	
	static long GetCPUCode( unsigned long selector )
	{
		if ( !TARGET_CPU_68K )
		{
			return mac::sys::gestalt( selector );
		}
		
		long code = mac::sys::gestalt( selector, -1 );
		
		if ( code == gestaltCPU68040 + 1 )
		{
			// Work around bug in System 7.1 on AV Quadras
			--code;
		}
		else if ( code < 0 )
		{
			code = GetProcCode();
		}
		
		return code;
	}
	
	static const char* GetCPUName( long code )
	{
		if ( TARGET_CPU_68K  &&  code < 0x0100 )
		{
			return Get68KCPUName( code );
		}
		
		const char* name = NULL;
		
		switch ( code )
		{
			case gestaltCPU601:     name = "601";    break;
			case gestaltCPU603:     name = "603";    break;
			case gestaltCPU604:     name = "604";    break;
			case gestaltCPU603e:    name = "603e";   break;
			case gestaltCPU603ev:   name = "603ev";  break;
			case gestaltCPU750:     name = "G3";     break;
			case gestaltCPU604e:    name = "604e";   break;
			case gestaltCPU604ev:   name = "604ev";  break;
			case gestaltCPUG4:      name = "G4";     break;
			case gestaltCPUG47450:  name = "7450";   break;
			case gestaltCPUApollo:  name = "7455";   break;
			case gestaltCPUG47447:  name = "7447";   break;
			case gestaltCPU750FX:   name = "750FX";  break;
			case gestaltCPU970:     name = "G5";     break;
			
		#if defined( __MACOS__ )  ||  defined( MAC_OS_X_VERSION_10_4 )
			
			case gestaltCPU970FX:   name = "970FX";  break;
			
		#endif
			
		#ifdef __MACOS__
			
			case gestaltCPU970MP:   name = "970MP";  break;
			
		#endif
			
			default:
				return gear::inscribe_decimal( code );
		}
		
		ASSERT( name != NULL );
		
		return name;
	}
	
	struct GetCPUFamily
	{
		typedef const char* Result;
		
		static Result Get()
		{
			return GetCPUName( GetCPUCode( 'cpuf' ) );
		}
	};
	
	struct GetCPUType
	{
		typedef const char* Result;
		
		static Result Get()
		{
			return GetCPUName( GetCPUCode( 'cput' ) );
		}
	};
	
	struct GetPrivilegeMode
	{
		typedef const char* Result;
		
		static Result Get()
		{
			UInt16 status = 0;
			
		#if TARGET_CPU_68K
			
			asm
			{
				MOVE.W		SR,status
			}
			
		#endif
			
			const char *const text = status & (1 << 13) ? "supervisor" : "user";
			
			return text;
		}
	};
	
	template < class Accessor >
	struct sys_cpu_Property : vfs::readonly_property
	{
		static void get( plus::var_string& result, const vfs::node* that, bool binary )
		{
			result = Accessor::Get();
		}
	};
	
	#define PROPERTY( prop )  &vfs::new_property, &vfs::property_params_factory< prop >::value
	
	const vfs::fixed_mapping sys_cpu_Mappings[] =
	{
		{ "family", PROPERTY( sys_cpu_Property< GetCPUFamily > ) },
		{ "type",   PROPERTY( sys_cpu_Property< GetCPUType   > ) },
		
	#if TARGET_CPU_68K
		
		{ "mode", PROPERTY( sys_cpu_Property< GetPrivilegeMode > ) },
		
	#endif
		
		{ NULL, NULL }
		
	};
	
}
