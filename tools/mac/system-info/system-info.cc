/*
	system-info.cc
	--------------
*/

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif

// Standard C
#include <stdio.h>
#include <string.h>

// more-libc
#include "more/string.h"

// gear
#include "gear/inscribe_decimal.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"
#include "mac_sys/get_machine_name.hh"
#include "mac_sys/unit_table.hh"


#ifndef TARGET_CPU_X86_64
#define TARGET_CPU_X86_64  0
#endif


#define PROGRAM  "system-info"

#define STR_LEN( s )  "" s, (sizeof s - 1)

// Pascal string and entire length, including length byte
#define PSTR_LEN( s )  "\p" s, (sizeof s)


using mac::sys::gestalt;
using mac::sys::gestalt_bit_set;
using mac::sys::gestalt_defined;


#define M68K    "Motorola 68K"
#define PPC     "PowerPC"
#define X86     "x86"
#define X86_64  "x86_64"
#define WHAT    "???"

#define MOD_TYPE  "Execution module type:  "
#define COMPILED  "Compiled architecture:  "

#define MAE  "Macintosh Application Environment (MAE)"


struct SonyVars_record
{
	uint32_t zeros[ 3 ];
	uint32_t reserved;
	uint32_t magic;
	uint32_t address;
};

#if TARGET_CPU_68K

long    MemTop  : 0x0108;
uint8_t CPUFlag : 0x012F;

SonyVars_record* SonyVars : 0x0134;

short SysVersion : 0x015A;

char** AppPacks[ 8 ] : 0x0AB8;

#else

static long MemTop;
static uint8_t CPUFlag;
static short SysVersion;

static char*** AppPacks;

#endif

static inline
bool in_supervisor_mode()
{
#ifdef __MC68K__
	
	uint16_t status;
	
	asm
	{
		MOVE.W   SR,status
	}
	
	return status & (1 << 13);
	
#endif
	
	return false;
}

#ifdef __MC68K__

static
asm uint32_t core_signature()
{
	MOVE     #0,CCR
	MOVEQ.L  #0,D0
	MOVEQ.L  #7,D1
	
loop:
	CHK.W    D1,D1
	MOVE     SR,D2
	OR.B     D2,D0
	ROR.L    #4,D0
	DBRA.S   D1,loop
	
	RTS
}

static inline
bool in_v68k()
{
	return core_signature() == 'v68k';
}

#else

static inline
bool in_v68k()
{
	return false;
}

#endif

static
void compiled()
{
	const char* type = TARGET_RT_MAC_MACHO ? MOD_TYPE "Mach-O"
	                 : TARGET_RT_MAC_CFM   ? MOD_TYPE "CFM"
	                 :                       MOD_TYPE "Code resource";
	
	puts( type );
	
	const char* arch = TARGET_CPU_68K    ? COMPILED M68K
	                 : TARGET_CPU_PPC    ? COMPILED PPC
	                 : TARGET_CPU_X86    ? COMPILED X86
	                 : TARGET_CPU_X86_64 ? COMPILED X86_64
	                 :                     COMPILED WHAT;
	
	puts( arch );
}

static const char* clock_speed_unit_prefixes[] =
{
	"",  // Hz
	"k",
	"M",
	"G",
};

static
void print_clock_speed( const char* part, uint32_t speed, int i )
{
	uint32_t x1000 = speed;
	
	while ( speed >= 1000 )
	{
		x1000 = speed;
		speed /= 1000;
		
		++i;
	}
	
	const char* X = clock_speed_unit_prefixes[ i ];
	
	if ( speed < 100 )
	{
		const unsigned digit = x1000 / 100 % 10;
		
		printf( "%s clock speed:        %u.%u %sHz\n", part, speed, digit, X );
	}
	else
	{
		printf( "%s clock speed:        %u %sHz\n", part, speed, X );
	}
}

static const uint32_t sysa = gestalt( 'sysa' );

static
void host_env()
{
	const uint32_t sysv = gestalt( 'sysv' );
	
	const char* arch_name = sysa <=  1 ? M68K
	                      : sysa ==  2 ? PPC
	                      : sysa == 10 ? X86
	                      :              WHAT;
	
	char machine_name[ 256 ] = { 0 };
	
	const unsigned char* mnam = mac::sys::get_machine_name();
	
	if ( mnam != NULL )
	{
		mempcpy( machine_name, mnam + 1, mnam[ 0 ] );
	}
	
	uint8_t sys1;
	uint8_t sys2;
	uint8_t sys3;
	
	if ( sysv >= 0x1049 )
	{
		sys1 = gestalt( 'sys1' );
		sys2 = gestalt( 'sys2' );
		sys3 = gestalt( 'sys3' );
	}
	else
	{
		const uint32_t vers = (TARGET_CPU_68K  &&  ! sysv) ? SysVersion : sysv;
		
		if ( sysv >= 0x1000 )
		{
			sys1 = 10;
		}
		else
		{
			sys1 = (vers >> 8) & 0xF;
		}
		
		sys2 = (vers >> 4) & 0xF;
		sys3 = (vers >> 0) & 0xF;
	}
	
	char a[ 4 ] = { 0 };
	char b[ 4 ] = { 0 };
	char c[ 4 ] = { 0 };
	
	gear::inscribe_unsigned_decimal_r( sys1, a );
	gear::inscribe_unsigned_decimal_r( sys2, b );
	gear::inscribe_unsigned_decimal_r( sys3, c );
	
	const char* os_name = sysv < 0x0760 ? "Macintosh System"
	                    : sysv < 0x1000 ? "Mac OS"
	                    : sysv < 0x1080 ? "Mac OS X"
	                    : sys2 <     12 ? "OS X"
	                    :                 "macOS";
	
	printf( "Host CPU architecture:  %s\n", arch_name );
	
	if ( TARGET_CPU_68K  &&  sysa <= 1 )
	{
		printf( "Host CPU model number:  680%c0\n", CPUFlag + '0' );
	}
	
	if ( (TARGET_CPU_68K  &&  sysa == 2)  ||  TARGET_CPU_PPC )
	{
		const uint32_t cput = gestalt( 'cput' );
		
		const char* type = cput == 0x0101 ? "601"
		                 : cput == 0x0103 ? "603"
		                 : cput == 0x0104 ? "604"
		                 : cput == 0x0106 ? "603e"
		                 : cput == 0x0107 ? "603ev"
		                 : cput == 0x0108 ? "G3"
		                 : cput == 0x0109 ? "604e"
		                 : cput == 0x010A ? "604ev"
		                 : cput == 0x010C ? "740/750 (G3)"
		                 : cput == 0x0110 ? "7450 (G4)"
		                 : cput == 0x0111 ? "7455 (G4)"
		                 : cput == 0x0112 ? "7447 (G4)"
		                 : cput == 0x0113 ? "7447A (G4)"  // not in Gestalt.h
		                 : cput == 0x0120 ? "750FX (G3)"
		                 : cput == 0x0139 ? "970 (G5)"
		                 : cput == 0x013C ? "970FX (G5)"
		                 : cput == 0x0144 ? "970MP (G5)"
		                 :                  "???";
		
		printf( "Host CPU model number:  PowerPC %s\n", type );
	}
	
	if ( mnam != NULL )
	{
		printf( "Host CPU machine name:  %s\n", machine_name );
	}
	
	if ( const uint32_t pclk = gestalt( 'pclk' ) )
	{
		const uint32_t bclk = gestalt( 'bclk' );
		const uint32_t mclk = gestalt( 'mclk' );
		const uint32_t bclm = gestalt( 'bclm' );
		
		puts( "" );
		print_clock_speed( "CPU", mclk ? mclk : pclk, mclk ? 2 : 0 );
		print_clock_speed( "Bus", bclm ? bclm : bclk, bclm ? 2 : 0 );
	}
	
	if ( sys1 != 0 )
	{
		const char* o = ".";
		
		if ( sys3 == 0 )
		{
			++o;
			
			c[ 0 ] = '\0';
		}
		
		puts( "" );
		printf( "Host operating system:  %s %s.%s%s%s\n", os_name, a, b, o, c );
	}
	
	const uint32_t os = gestalt( 'os  ' );
	
	if ( TARGET_RT_MAC_MACHO )
	{
		// Do nothing.  Process Manager is implied and goes without saying.
	}
	else if ( os & (1 << 19) )
	{
		puts( "Application switching:  Process Manager (native)" );
	}
	else if ( os & (1 << 3) )
	{
		puts( "Application switching:  Process Manager" );
	}
	else if ( os & (1 << 1) )
	{
		puts( "Application switching:  MultiFinder" );
	}
	else
	{
		puts( "Application switching:  none" );
	}
	
	if ( ! TARGET_CPU_68K )
	{
		// Do nothing.  Thread Manager is implied and goes without saying.
	}
	else if ( gestalt( 'thds' ) )
	{
		puts( "Cooperative threading:  Thread Manager" );
	}
	else
	{
		puts( "Cooperative threading:  Metamage threads" );
	}
	
	const uint32_t gestaltOpenTptTCPPresentMask = 0x00000010;
	
	if ( gestalt( 'otan' ) & gestaltOpenTptTCPPresentMask )
	{
		const uint16_t version = gestalt( 'otvr' ) >> 16;
		
		const int major = (version >> 8 & 0xF) + 10 * (version >> 12);
		const int minor =  version >> 4 & 0xF;
		const int patch =  version >> 0 & 0xF;
		
		const bool mach = TARGET_RT_MAC_MACHO;
		
		const char* text = mach ? "Open Transport TCP/IP:  "
		                        : "TCP/IP protocol stack:  Open Transport ";
		
		char dot[] = ".0";
		
		if ( patch != 0 )
		{
			dot[ 1 ] += patch;
		}
		else
		{
			dot[ 0 ] = '\0';
		}
		
		printf( "%s%d.%d%s\n", text, major, minor, dot );
	}
	else if ( gestalt_defined( 'mtcp' ) )
	{
		puts( "TCP/IP protocol stack:  MacTCP (not yet supported)" );
	}
	
	const uint32_t mmu = gestalt( 'mmu ' );
	
	if ( TARGET_CPU_68K )
	{
		const uint32_t fpu = gestalt( 'fpu ' );
		
		const char* fpu_name = fpu == 0 ? (AppPacks[ 4 ] ? "SANE only" : "none")
		                     : fpu == 1 ? "68881"
		                     : fpu == 2 ? "68882"
		                     : fpu == 3 ? "68040 built-in FPU"
		                     :            "unknown FPU";
		
		printf( "Floating point praxis:  %s\n", fpu_name );
	}
	
	if ( TARGET_CPU_68K  &&  sysa <= 1  &&  CPUFlag >= 2 )
	{
		const char* mmu_name = mmu == 0 ? "none"  // (seen in Basilisk II)
		                     : mmu == 1 ? "none (non-paging AMU)"
		                     : mmu == 2 ? "68851"
		                     : mmu == 3 ? "68030 built-in MMU"
		                     : mmu == 4 ? "68040 built-in MMU"
		                     :            "unknown MMU";
		
		printf( "Paged memory hardware:  %s\n", mmu_name );
	}
	
	if ( TARGET_CPU_68K  &&  mmu )
	{
		const int gestalt32BitAddressing = 0;
		
		const bool _32bits = gestalt_bit_set( 'addr', gestalt32BitAddressing );
		
		printf( "680x0 addressing mode:  %s-bit\n", _32bits ? "32" : "24" );
	}
	
	int32_t ram;
	
	if ( TARGET_API_MAC_CARBON  &&  (ram = gestalt( 'ramm' )) )
	{
		const char* units = "MiB";
		
		if ( (ram & 0x3ff) == 0 )
		{
			ram /= 1024u;
			units = "GiB";
		}
		
		printf( "Physical memory total:  %ld %s\n", (long) ram, units );
	}
	else if ( (ram = gestalt( 'ram ' ))  ||  (ram = MemTop) )
	{
		ram /= 1024u;
		
		const char* units = "K";
		
		if ( (ram & 0x3ff) == 0 )
		{
			ram /= 1024u;
			units = " MiB";
		}
		
		printf( "Physical memory total:  %ld%s\n", (long) ram, units );
	}
	
	if ( TARGET_CPU_68K )
	{
		const char* level = in_supervisor_mode() ? "Supervisor" : "User";
		
		printf( "680x0 privilege level:  %s\n", level );
	}
	
	if ( ! TARGET_RT_MAC_MACHO  &&  mmu > 1  &&  sysv < 0x1000 )
	{
		const char* status = gestalt( 'vm  ' ) ? "On" : "Off";
		
		printf( "Virtual memory status:  %s\n", status );
	}
	
	if ( !! TARGET_RT_MAC_CFM  &&  TARGET_API_MAC_CARBON )
	{
		const uint32_t cbon = gestalt( 'cbon' );
		
		const char a = 0x30 + ((cbon >> 8) & 0xF);
		const char b = 0x30 + ((cbon >> 4) & 0xF);
		const char c = 0x30 + ((cbon >> 0) & 0xF);
		
		char dot[] = ".0";
		
		if ( c != '0' )
		{
			dot[ 1 ] = c;
		}
		else
		{
			dot[ 0 ] = '\0';
		}
		
		printf( "CarbonLib version:      %c.%c%s\n", a, b, dot );
	}
}

static
bool in_MinivMac()
{
#if TARGET_CPU_68K
	
	if ( SonyVars  &&  ((uint32_t) SonyVars & 1) == 0 )
	{
		const uint32_t magic = 0x841339E2;
		
		const uint32_t* p = SonyVars->zeros;
		
		uint32_t zero = 0;
		
		zero |= *p++;
		zero |= *p++;
		zero |= *p++;
		
		return zero == 0  &&  SonyVars->magic == magic;
	}
	
#endif
	
	return false;
}

static
bool in_BasiliskII()
{
	using mac::types::AuxDCE;
	
	if ( ! TARGET_CPU_68K )
	{
		return false;
	}
	
	const short n = mac::sys::get_unit_table_entry_count();
	
	AuxDCE*** const begin = mac::sys::get_unit_table_base();
	AuxDCE*** const end   = begin + n;
	
	for ( AuxDCE*** it = begin;  it < end;  ++it )
	{
		const unsigned char* name = mac::sys::get_driver_name( *it );
		
		int cmp = memcmp( name, PSTR_LEN( ".Display_Video_Apple_Basilisk" ) );
		
		if ( cmp == 0 )
		{
			return true;
		}
	}
	
	return false;
}

static
bool in_ShapeShifter()
{
	using mac::types::AuxDCE;
	
	if ( ! TARGET_CPU_68K )
	{
		return false;
	}
	
	const short n = mac::sys::get_unit_table_entry_count();
	
	AuxDCE*** const begin = mac::sys::get_unit_table_base();
	AuxDCE*** const end   = begin + n;
	
	for ( AuxDCE*** it = begin;  it < end;  ++it )
	{
		const unsigned char* name = mac::sys::get_driver_name( *it );
		
		/*
			Expecting one of:
			
				.Display_Video_Apple_Amiga1
				.Display_Video_Apple_Amiga2
		*/
		
		int cmp = memcmp( name, PSTR_LEN( ".Display_Video_Apple_Amiga?" ) - 1 );
		
		if ( cmp == 0 )
		{
			return true;
		}
	}
	
	return false;
}

static
bool in_SheepShaver()
{
	using mac::types::AuxDCE;
	
	if ( TARGET_RT_MAC_MACHO )
	{
		return false;
	}
	
	const short n = mac::sys::get_unit_table_entry_count();
	
	AuxDCE*** const begin = mac::sys::get_unit_table_base();
	AuxDCE*** const end   = begin + n;
	
	for ( AuxDCE*** it = begin;  it < end;  ++it )
	{
		const unsigned char* name = mac::sys::get_driver_name( *it );
		
		// Yes, SheepShaver currently installs a driver with a dotless name.
		
		if ( memcmp( name, PSTR_LEN( "Display_Video_Apple_Sheep" ) ) == 0 )
		{
			return true;
		}
		
		if ( memcmp( name, PSTR_LEN( ".Display_Video_Apple_Sheep" ) ) == 0 )
		{
			return true;
		}
	}
	
	return false;
}

static
bool in_QEMU()
{
	using mac::types::AuxDCE;
	
	if ( TARGET_RT_MAC_MACHO )
	{
		return false;
	}
	
	const short n = mac::sys::get_unit_table_entry_count();
	
	AuxDCE*** const begin = mac::sys::get_unit_table_base();
	AuxDCE*** const end   = begin + n;
	
	for ( AuxDCE*** it = begin;  it < end;  ++it )
	{
		const unsigned char* name = mac::sys::get_driver_name( *it );
		
		int cmp = memcmp( name, PSTR_LEN( ".Display_Video_QemuVGA" ) );
		
		if ( cmp == 0 )
		{
			return true;
		}
	}
	
	return false;
}

static
void virt_env()
{
	const char* blank = "\n";
	
	if ( gestalt( 'a/ux' ) )
	{
		printf( "%s" "Paravirtualization:     A/UX\n", blank );
		blank = "";
	}
	
	const bool bbox = gestalt( 'bbox' );
	
	if ( bbox )
	{
		printf( "%s" "Paravirtualization:     Blue Box\n", blank );
		blank = "";
	}
	
	if ( TARGET_CPU_68K  &&  sysa == 2 )
	{
		printf( "%s" "68K emulation:          Apple\n", blank );
		blank = "";
	}
	
	if ( TARGET_CPU_68K  &&  in_v68k() )
	{
		printf( "%s" "68K emulation:          v68k\n", blank );
		blank = "";
	}
	else if ( TARGET_CPU_68K  &&  gestalt_defined( 'cith' ) )
	{
		printf( "%s" "68K emulation:          " MAE "\n", blank );
		blank = "";
	}
	else if ( TARGET_CPU_68K  &&  in_MinivMac() )
	{
		printf( "%s" "68K emulation:          Mini vMac\n", blank );
		blank = "";
	}
	else if ( in_BasiliskII() )
	{
		printf( "%s" "68K emulation:          Basilisk II\n", blank );
		blank = "";
	}
	else if ( TARGET_CPU_68K  &&  in_ShapeShifter() )
	{
		printf( "%s" "68K emulation:          ShapeShifter\n", blank );
		blank = "";
	}
	else if ( in_SheepShaver() )
	{
		printf( "%s" "PPC emulation:          SheepShaver\n", blank );
		blank = "";
	}
	else if ( in_QEMU() )
	{
		printf( "%s" "PPC emulation:          QEMU\n", blank );
		blank = "";
	}
	
	if ( TARGET_CPU_PPC  &&  TARGET_API_MAC_CARBON  &&  ! bbox )
	{
		if ( gestalt( 'ppcf' ) == 0x0011 )
		{
			printf( "%s" "PPC emulation:          Rosetta\n", blank );
			blank = "";
		}
	}
}

int main( int argc, char** argv )
{
	compiled();
	host_env();
	virt_env();
	
	return 0;
}
