/*	=================
 *	FSTree_sys_cpu.cc
 *	=================
 */

#include "Genie/FS/FSTree_sys_cpu.hh"

// Iota
#include "iota/decimal.hh"

// Debug
#include "debug/assert.hh"

// Nitrogen
#include "Nitrogen/Gestalt.hh"

// Genie
#include "Genie/FS/FSTree_Property.hh"


#if UNIVERSAL_INTERFACES_VERSION <= 0x0342

enum
{
	gestaltCPUApollo = 0x0111,
	gestaltCPUG47447 = 0x0112,
	gestaltCPU750FX  = 0x0120,
	gestaltCPU970    = 0x0139,
	gestaltCPU970FX  = 0x013c,
	gestaltCPU970MP  = 0x0144,
};

#endif

namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	static long GetProcCode()
	{
		return N::Gestalt( N::Gestalt_Selector( 'proc' ) ) - 1;
	}
	
	static const char* Get68KCPUName( long code )
	{
		return iota::inscribe_decimal( 68000 + 10 * code );
	}
	
	struct GetCPUProc
	{
		typedef const char* Result;
		
		static Result Get()
		{
			return Get68KCPUName( GetProcCode() );
		}
	};
	
	static long GetCPUCode( N::Gestalt_Selector selector )
	{
		if ( !TARGET_CPU_68K )
		{
			return N::Gestalt( selector );
		}
		
		long code = 0;
		
		try
		{
			code = N::Gestalt( selector );
			
			if ( code == gestaltCPU68040 + 1 )
			{
				// Work around bug in System 7.1 on AV Quadras
				--code;
			}
		}
		catch ( const N::OSStatus& err )
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
			case gestaltCPU970FX:   name = "970FX";  break;
			case gestaltCPU970MP:   name = "970MP";  break;
			
			default:
				return iota::inscribe_decimal( code );
		}
		
		ASSERT( name != NULL );
		
		return name;
	}
	
	struct GetCPUFamily
	{
		typedef const char* Result;
		
		static Result Get()
		{
			return GetCPUName( GetCPUCode( N::Gestalt_Selector( 'cpuf' ) ) );
		}
	};
	
	struct GetCPUType
	{
		typedef const char* Result;
		
		static Result Get()
		{
			return GetCPUName( GetCPUCode( N::Gestalt_Selector( 'cput' ) ) );
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
	struct sys_cpu_Property
	{
		static std::string Read( const FSTree* that, bool binary )
		{
			return Accessor::Get();
		}
	};
	
	template < class Accessor >
	static FSTreePtr Property_Factory( const FSTreePtr&    parent,
	                                   const std::string&  name )
	{
		typedef sys_cpu_Property< Accessor > Property;
		
		return New_FSTree_Property( parent,
		                            name,
		                            &Property::Read );
	}
	
	const FSTree_Premapped::Mapping sys_cpu_Mappings[] =
	{
		{ "family", &Property_Factory< GetCPUFamily > },
		{ "type",   &Property_Factory< GetCPUType   > },
		
	#if TARGET_CPU_68K
		
		{ "mode", &Property_Factory< GetPrivilegeMode > },
		
	#endif
		
		{ NULL, NULL }
		
	};
	
}

