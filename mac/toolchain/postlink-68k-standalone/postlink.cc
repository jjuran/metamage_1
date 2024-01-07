/*
	postlink.cc
	-----------
	
	Post-linker for classic Mac OS stand-alone 68K code resources
	
	Copyright 2023-2024, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	====================================================================
	NOTE:  The comments below contain a disassembly of executable code
	whose copyright was held by Metrowerks (until they were purchased).
	The /comments/ are copyright Joshua Juran, but the code to which
	they correspond is not, and that code is not covered by the AGPL.
	The disassembly of the code is presented here for the scholarly
	purpose of documenting the behavior of the post-linker, whose own
	purpose is that of interoperability.  Fair use applies.
	
	However, the new startup code is original code copyright Joshua Juran.
	====================================================================
	
	Overview
	--------
	
	This is a post-linker run by `ld` in the MacRelix-hosted toolchain
	targeting classic Mac OS via Metrowerks development tools.  It fixes
	a compatibility issue in the startup code for stand-alone 68K code
	resources which is automatically provided by the Metrowerks linker.
	
	The issue is that the startup code unconditionally calls StripAddress(),
	which is present on a Mac SE (or later) or in System 6.0 (or later),
	but not otherwise.  Therefore, a system extension (formerly, "INIT")
	built with the Metrowerks tools could work in System 6 on a Mac Plus
	or in System 4.2 on a Mac SE, but would crash during system startup
	in System 4.2 on a Mac Plus, for example.
	
	In the case of application plugins, it's possible for the application
	to check for the existence of StripAddress() and implement if needed,
	before calling any of its plugins.  Indeed, MacRelix takes this exact
	approach for its user-space programs.  For general use, however, it's
	inadequate.
	
	Nor is it acceptable to have StripAddress() implemented by an INIT at
	startup.  First, there's a bootstrapping problem -- the INIT would
	have to be produced using a different toolchain or specially crafted
	to avoid the very issue it means to address.  Second, were that INIT
	to be removed from the System Folder (or even renamed such that other
	INITs now loaded before it), affected code resources would mysteriously
	begin crashing.
	
	It's possible to specify a custom resource header, though only at the
	cost of basic C runtime facilities that are usually taken for granted,
	like global variable initialization.  This is impractical at best.
	
	A solution that achieves the desired backward compatibility without
	sacrificing either language support or use of the Metrowerks dev tools
	in the first place is a post-linking phase that patches the faulty
	runtime after the fact.  Fortunately, both the logic written in C and
	the Metrowerks tools used to build it left enough redundancy for us to
	squeeze in a system version check.  Our modified startup code[1] only
	calls StripAddress() in System 6[2] and later, where it's guaranteed
	to exist.  In earlier systems, it defaults to Lo3Bytes ($00FFFFFF) as
	an address mask.
	
	While it's possible to boot System 4.2 on a Mac II, which has a 68020
	processor and 32-bit addresses, the maximum usable RAM is 8 MiB, so
	code will never be loaded above $00800000, and which mechanism is used
	to mask the address is moot.
	
	[1] The first version of this tool only minimally patched the Metrowerks
	runtime to fix the problem; the current version replaces it completely.
	
	[2] More specifically, the modified startup code calls StripAddress()
	in System 6.0.4 and later.  Connectix Optima is documented as enabling
	32-bit mode on certain machines running 6.0.4 or later, but no earlier.
	It's also been reported that System 6.0.3 can be trivially patched to
	run on a machine with a 64K ROM (for which System 6 will /not/ install
	a handler for StripAddress()), since unlike 6.0.4, it boots to the
	Finder without calling StripAddress().  In any case, it's preferable
	to avoid the trap dispatch overhead, so in the absence of 32-bit mode
	support, we'll use Lo3Bytes directly in System 6.0.3 and earlier.
	
	Special thanks to Keith Kaisershot for extensively researching which
	versions of system software support StripAddress() and 32-bit mode.
	
	Theory
	------
	
	The startup code's logic flow is made less clear by the seemingly
	arbitrary order in which the routines appear.  Here's the call tree:
	
	__Startup__
		SetCurrentA4 -> A4 (data_start)
		__SetupMainRsrc__(code_start : D0)
			__GetMainXRef__ -> D0 (xref_start)
			__relocate_compr(xref_start, code_start, reloc_delta)
	main (tail call)
	
	Here's the call tree with branches and A-trap calls included:
	
	__Startup__
		SetCurrentA4
			_StripAddress
		_StripAddress
		__SetupMainRsrc__
			if relocating (always true on the first call):
				if relocating for the first time:
					_GetToolBoxTrapAddress
					_GetOSTrapAddress
				__GetMainXRef__
					_StripAddress
				__relocate_compr
					(loop)
				if _HWPriv implemented:
					_HWPriv
	main (tail call)
	
	Notice the three distinct calls to StripAddress().  This would be
	appropriate if StripAddress() performed arbitrary address translation.
	On the other hand, if StripAddress() is exactly as straightforward as
	it only needs to be to address the 24-bit / 32-bit duality, then some
	simplification is possible.
	
	Postulate #1:  In 32-bit mode, StripAddress() does nothing at all:
	               StripAddress(p) -> p
	
	Postulate #2:  In 24-bit mode, StripAddress() clears the high byte of D0:
	               StripAddress(p) -> p & 0x00FFFFFF
	
	Postulate #3:  In 24-bit mode, a maximum of 8 MiB of RAM is available.
	
	Now consider this proposition:
	
		Proposition #1:  For all p and k > 0 where p + k < 2^24,
		                 StripAddress(p + k) == StripAddress(p) + k
	
	In 32-bit mode, this is trivially true, because StripAddress() is the
	identity function (postulate #1).  In 24-bit mode, the constraint
	chosen ensures that the high byte of p + k is always zero, so again,
	StripAddress() (this time by clearing the already clear high byte) is
	an identity function (postulate #2).  Finally, because RAM extends no
	further than 8 MiB in 24-bit mode (postulate #3), the maximum possible
	RAM address is 2^23 - 1, satisfying the constraint.
	
	Conclusion #1:  We don't need to call StripAddress() for each address.
	                It's sufficient to call p = StripAddress(p) for the
	                base address p, and then compute q = p + k for values
	                corresponding to addresses in RAM.
	
	Conclusion #2:  We don't (necessarily) need to call StripAddress() at
	                all -- as long as we can prove that we're in either of
	                24-bit mode or 32-bit mode, we know what StripAddress()
	                would do in that case, and we can do it directly.
	
	Conclusion #3:  Even if we do call StripAddress(), it never has to be
	                called on a real address.  It's sufficient to call it
	                once to produce an address mask, like so:
	                
	                	mask = StripAddress(0xFFFFFFFF)
	                
	                which is then used for all actual address stripping.
	
	Strategy
	--------
	
	Previously, the goal was to insert new code that determines whether
	or not to call StripAddress(), or assume 24-bit mode and mask with
	Lo3Bytes, making space for it by consolidating redundant code.
	
	The new goal is replacing the Metrowerks runtime in its entirety.
	
	We extract the data offset, xref offset, and main() offset from
	their known locations in the code.  The entire startup code becomes
	a single function.  Other than the relocation data processing loop,
	execution flow proceeds only forward.
	
	First, we save registers onto the stack.  In addition to the usual
	volatile registers and A4, we also save A2 and A3 now so we can use
	them later, at no further cost in code size (since we're using MOVEM).
	
	The next code (which used to be the body of get_address_mask()) yields
	either 0x00FFFFFF or 0xFFFFFFFF in D0 as the address mask.  The mask
	is used to produce a stripped address of the start of the code, which
	in turn is used as the base for computing the start-of-data address.
	
	The first longword of data is the previous code start address.  If it
	matches the current start of code, then no relocation is necessary.
	If it's zero, then this code is running for the first time and some
	initialization is required.  Since we're going to be modifying code
	that's already executing, we'll need to flush the code cache (if any)
	when we're done.  The HWPriv trap can be used to do that, though only
	if it's implemented.  We check whether it is, and store the result for
	later.  If this isn't the first run, we reuse the result from earlier.
	
	The relocation data processor implements the same algorithm as in
	Metrowerks' runtime, but it's written from scratch.  It decodes a
	compressed series of offsets into the code, each of which points to
	an address requiring relocation (by adding the relocation delta).
	
	After relocation is performed, the code cache (if there is one) must
	be flushed.  We consult the result of our previous trap check and call
	HWPriv() if it's present.  Then we place the start-of-code address in
	the previous code start field.  (It won't be seen until (and unless)
	the code resource is called again, by which point it will be the
	previous code start).  Finally, we restore the saved registers and
	tail-call into main(), thus completing startup.
	
	Old startup code:
	
		__Startup__:
		...
		00000c:  MOVEM.L  D0-D2/A0-A1/A4,-(A7)    // save registers
		000010:  JSR      *+244    // $000104     // SetCurrentA4
		000014:  LEA      *-20,A0    // $000000
		000018:  MOVE.L   A0,D0                   // start of code
		00001a:  _StripAddress
		00001c:  JSR      *+146    // $0000ae     // __SetupMainRsrc__
		000020:  MOVEM.L  (A7)+,D0-D2/A0-A1/A4    // restore registers
		000024:  JMP      *+694    // $0002da     // main
		
		__GetMainXRef__:
		000028:  LEA      *-40,A0    // $000000
		00002c:  ADDA.L   #0x00000yyy,A0          // moved to __relocate_compr
		000032:  MOVE.L   A0,D0
		000034:  _StripAddress
		000036:  RTS
		
		__relocate_compr:
		...
		
		__SetupMainRsrc__:
		...
		
		SetCurrentA4:
		000104:  LEA      *-260,A0    // $000000
		000108:  ADDA.L   #0x00000xxx,A0          // moved to __Startup__
		00010e:  MOVE.L   A0,D0
		000110:  _StripAddress
		000112:  EXG      D0,A4                   // stripped start of data
		000114:  RTS
		
	New startup code:
	
		__Startup__:
			
			MOVEM.L  D0-D2/A0-A4,-(A7)       // save registers
			LEA      *-4,A0   // $000000
			
			// get_address_mask (inlined)
			MOVE.L   Lo3Bytes,D0
			CMPI.W   #0x0604,SysVersion      // at least System 6.0.4?
			BLT.S    mask_set
			
			MOVEQ    #-1,D0
			_StripAddress
			
		mask_set:
			
			MOVE.L   A0,D2
			AND.L    D0,D2                   // strip the address
			MOVEA.L  D2,A2                   // stripped start of code
			MOVEA.L  D2,A4
			ADDA.L   #0x00000xxx,A4          // stripped start of data
			
			// __SetupMainRsrc__ (inlined)
			MOVE.L   (A4),D0                 // previous start of code
			SUB.L    D0,D2                   // relocation delta
			BEQ.S    reloc_done
			
			MOVE.L   A2,(A4)                 // new previous start of code
			
			MOVEA.L  A2,A0                   // stripped start of code
			ADDA.L   #0x00000yyy,A0          // stripped start of xref
			
			// __relocate_compr (inlined)
			SUBQ.W   #4,A7
			MOVEA.L  A7,A3                   // scratch space
			MOVE.W   (A0)+,D1                // xref length
			SUBQ.W   #1,D1
			SUBA.L   A1,A1
			
		loop:
			
			MOVE.B   (A0)+,D0
			BGE.S    bit_7_clear
			
			// bit 7 set:  7-bit word delta
			ADD.B    D0,D0
			EXT.W    D0
			BRA.S    add_delta
			
		bit_7_clear:
			
			MOVE.B   D0,(A3)+
			MOVE.B   (A0)+,(A3)+
			ANDI.W   #0x40,D0
			BNE.S    bit_6_set
			
			// bits 6 and 7 clear:  30-bit word offset
			MOVE.B   (A0)+,(A3)+
			MOVE.B   (A0)+,(A3)+
			MOVE.L   -(A3),D0
			LSL.L    #2,D0
			ASR.L    #1,D0
			MOVEA.L  D0,A1
			BRA.S    relocate
			
		bit_6_set:
			
			// bit 7 clear, bit 6 set:  14-bit word delta
			MOVE.W   -(A3),D0
			LSL.W    #2,D0
			ASR.W    #1,D0
			
		add_delta:
			
			ADDA.W   D0,A1
			
		relocate:
			
			ADD.L    D2,(A2,A1.L)
			DBLE     D1,loop
			
			ADDQ.W   #4,A7
			
			LEA      __InitCode__,A0
			MOVE.L   #0x00000zzz,D0
			MOVEA.L  A0,A1
			_BlockMove
			
		reloc_done:
			
			MOVEM.L  (A7)+,D0-D2/A0-A4       // restore registers
			JMP      ...                     // jump to main()
	
*/

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif
#ifndef __TEXTUTILS__
#include <TextUtils.h>
#endif

// POSIX
#include <unistd.h>

// Standard C
#include <stdlib.h>

// more-libc
#include <more/string.h>

// mac-sys-utils
#include "mac_sys/res_error.hh"

// mac-relix-utils
#include "mac_relix/FSSpec_from_path.hh"

// command
#include "command/get_option.hh"


#pragma exceptions off


#define PREFIX  "postlink-68k-standalone: "

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define ERROR( e, msg )  (wrote( 2, STR_LEN( PREFIX msg "\n" ) ) ? e : 13)


using namespace command::constants;

enum
{
	Option_dry_run = 'n',
};

static command::option options[] =
{
	{ "", Option_dry_run },
	
	{ NULL }
};

static bool dry_run = false;

static inline
bool wrote( int fd, const void* buffer, size_t n )
{
	return write( fd, buffer, n ) == n;
}

static
char* const* get_options( char* const* argv )
{
	++argv;  // skip arg 0
	
	short opt;
	
	while ( (opt = command::get_option( &argv, options )) )
	{
		switch ( opt )
		{
			case Option_dry_run:
				dry_run = true;
				break;
			
			default:
				exit( ERROR( 50, "invalid option" ) );
		}
	}
	
	return argv;
}

const UInt16 _StripAddress  = 0xA055;

const UInt16 BRA_S_12     = 0x600a;  // BRA.S    *+12
const UInt16 JMP_PCrel    = 0x4efa;  // JMP      *...
const UInt16 MOVE_L_A0_D0 = 0x2008;  // MOVE.L   A0,D0

const UInt32 JMP_6                     = JMP_PCrel    << 16 | 0x0004;

const UInt32 MOVE_L_A0_D0_StripAddress = MOVE_L_A0_D0 << 16 | _StripAddress;

static const UInt16 startup_code[] =
{
	0x48e7, 0xe0f8,          // 000000:  MOVEM.L  D0-D2/A0-A4,-(A7)
	0x41fa, 0xfffa,          // 000004:  LEA      *-4,A0     // $000000
	0x2038, 0x031a,          // 000008:  MOVE.L   Lo3Bytes,D0
	0x0c78, 0x0604, 0x015a,  // 00000c:  CMPI.W   #0x0604,SysVersion
	0x6d04,                  // 000012:  BLT.S    *+6    // $000018 mask_set
	
	0x70ff,                  // 000014:  MOVEQ    #-1,D0
	0xa055,                  // 000016:  _StripAddress
	
// mask_set:
	
	0x2408,                  // 000018:  MOVE.L   A0,D2
	0xc480,                  // 00001a:  AND.L    D0,D2
	0x2442,                  // 00001c:  MOVEA.L  D2,A2
	0x2842,                  // 00001e:  MOVEA.L  D2,A4
	0xd9fc, 0x0000, 0x0000,  // 000020:  ADDA.L   #0x00000000,A4
	0x2014,                  // 000026:  MOVE.L   (A4),D0
	0x9480,                  // 000028:  SUB.L    D0,D2
	0x6756,                  // 00002a:  BEQ.S    *+88   // $000082 reloc_done
	
	0x288a,                  // 00002c:  MOVEA.L  A2,(A4)
	0x204a,                  // 00002e:  MOVEA.L  A2,A0
	0xd1fc, 0x0000, 0x0000,  // 000030:  ADDA.L   #0x00000000,A0
	0x594f,                  // 000036:  SUBQ.W   #4,A7
	0x264f,                  // 000038:  MOVEA.L  A7,A3
	0x3218,                  // 00003a:  MOVE.W   (A0)+,D1
	0x5341,                  // 00003c:  SUBQ.W   #1,D1
	0x93c9,                  // 00003e:  SUBA.L   A1,A1
	
// loop:
	
	0x1018,                  // 000040:  MOVE.B   (A0)+,D0
	0x6c06,                  // 000042:  BGE.S    *+8    // $00004a bit_7_clear
	
	0xd000,                  // 000044:  ADD.B    D0,D0
	0x4880,                  // 000046:  EXT.W    D0
	0x601e,                  // 000048:  BRA.S    *+32   // $000068 add_delta
	
// bit_7_clear:
	
	0x16c0,                  // 00004a:  MOVE.B   D0,(A3)+
	0x16d8,                  // 00004c:  MOVE.B   (A0)+,(A3)+
	0x0240, 0x0040,          // 00004e:  ANDI.W   #0x40,D0
	0x660e,                  // 000052:  BNE.S    *+16   // $000062 bit_6_set
	
	0x16d8,                  // 000054:  MOVE.B   (A0)+,(A3)+
	0x16d8,                  // 000056:  MOVE.B   (A0)+,(A3)+
	0x2023,                  // 000058:  MOVE.L   -(A3),D0
	0xe588,                  // 00005a:  LSL.L    #2,D0
	0xe280,                  // 00005c:  ASR.L    #1,D0
	0x2240,                  // 00005e:  MOVEA.L  D0,A1
	0x6008,                  // 000060:  BRA.S    *+10   // $00006a relocate
	
// bit_6_set:
	
	0x3023,                  // 000062:  MOVE.W   -(A3),D0
	0xe548,                  // 000064:  LSL.W    #2,D0
	0xe240,                  // 000066:  ASR.W    #1,D0
	
// add_delta:
	
	0xd2c0,                  // 000068:  ADDA.W   D0,A1
	
// relocate:
	
	0xd5b2, 0x9800,          // 00006a:  ADD.L    D2,(A2,A1.L)
	0x5fc9, 0xffd0,          // 00006e:  DBLE     D1,*-46    // $000040 loop
	
	0x584f,                  // 000072:  ADDQ.W   #4,A7
	
	0x41fa, 0x0014,          // 000074:  LEA      *+22,A0    // $00008a
	0x203c, 0x0000, 0x0000,  // 000078:  MOVE.L   #0x00000zzz,D0
	0x2248,                  // 00007e:  MOVEA.L  A0,A1
	0xA02E,                  // 000080:  _BlockMove
	
// reloc_done:
	
	0x4cdf, 0x1f07,          // 000082:  MOVEM.L  (A7)+,D0-D2/A0-A4
	0x4efa, 0x0000,          // 000086:  JMP      ...
};

#define SPACE  \
          "      "  \
"           This "  \
"                "  \
" s  p  a  c  e  "  \
"                "  \
"  intentionally "  \
"                "  \
"         left   "  \
"blank.          "  \
"      "

static
bool fix_up_far_code( Handle code )
{
	const Size expected_minimum_size = 0x0116;
	
	Size size = GetHandleSize( code );
	
	char* p = *code;
	
	/*
		Bail out if (a) the code is too short to look at, (b) the runtime
		doesn't match what we're expecting, or (c) we already patched it.
	*/
	
	if ( size < expected_minimum_size )
	{
		return false;
	}
	
	if ( *(UInt16*) p != BRA_S_12 )
	{
		return false;
	}
	
	if ( *(UInt32*) (p + 0x0018) != MOVE_L_A0_D0_StripAddress )
	{
		return false;
	}
	
	const UInt32 data_offset = *(UInt32*) (p + 0x010a);
	const UInt32 xref_offset = *(UInt32*) (p + 0x002e);
	const UInt32 xref_is_odd = xref_offset & 1;
	
	const UInt16 main_offset = *(UInt16*) (p + 0x0026) + 0x0026;
	const UInt16 jump_offset = sizeof startup_code - 2;
	const UInt16 init_offset = 0x0116;
	
	UInt16* qW = NULL;
	
	qW = (UInt16*) mempcpy( p, startup_code, jump_offset );
	
	*qW++ = main_offset - jump_offset;
	
	mempcpy( qW, STR_LEN( SPACE ) );
	
	const UInt32 target_size = data_offset - init_offset;
	
	*(UInt32*) (p + 0x0022) = data_offset;
	*(UInt32*) (p + 0x0032) = xref_offset + xref_is_odd;
	*(UInt32*) (p + 0x007a) = target_size;
	
	/*
		We limit the xref to 64K - 1 relocations
		and force it to be word-aligned.
		
		If the xref starts on an odd byte offset, slide it down
		one byte position and advance the offset by one byte.
		
		If the xref starts on a word boundary, slide it down
		two byte positions.
	*/
	
	const UInt32 xref_slack = 2 - xref_is_odd;
	
	Munger( code, xref_offset, NULL, xref_slack, NULL, 0  );
	
	return true;
}

static
bool fix_up_near_code( Handle code )
{
	const Size expected_minimum_size = 0x0014;
	
	Size size = GetHandleSize( code );
	
	char* p = *code;
	
	/*
		Bail out if (a) the code is too short to look at, (b) the runtime
		doesn't match what we're expecting, or (c) we already patched it.
	*/
	
	if ( size < expected_minimum_size )
	{
		return false;
	}
	
	if ( *(UInt16*) p != BRA_S_12 )
	{
		return false;
	}
	
	if ( *(UInt32*) (p + 0x000c) != JMP_6 )
	{
		return false;
	}
	
	Munger( code, 0, NULL, 0x0012, NULL, 0  );  // delete the first 18 bytes
	
	return true;
}

static inline
OSErr CompactResFile( short refnum )
{
	using mac::sys::res_error;
	
	short attrs;
	
	(attrs = GetResFileAttrs( refnum ), res_error())  ||
	(attrs |= mapCompact, false)                      ||
	(SetResFileAttrs( refnum, attrs ), res_error());
	
	return res_error();
}

int main( int argc, char** argv )
{
	using mac::relix::FSSpec_from_existing_path;
	using mac::sys::res_error;
	
	char *const *args = get_options( argv );
	
	const int argn = argc - (args - argv);
	
	if ( argn == 0 )
	{
		return ERROR( 50, "argument required" );
	}
	
	const char* target_path = args[ 0 ];
	
	FSSpec target_filespec;
	
	if ( FSSpec_from_existing_path( target_path, target_filespec ) )
	{
		return ERROR( 43, "file not found" );
	}
	
	short resfile = FSpOpenResFile( &target_filespec, fsRdWrPerm );
	
	if ( resfile < 0 )
	{
		OSErr err = res_error();
		
		return ERROR( err ? -err : 23, "can't open resource fork" );
	}
	
	bool patching_applicable = false;
	
	int exit_status = 0;
	
	const short n_types = Count1Types();
	
	for ( short i = 1;  i <= n_types;  ++i )
	{
		ResType type;
		
		Get1IndType( &type, i );
		
		const short n_rsrcs = Count1Resources( type );
		
		for ( short j = 1;  j <= n_rsrcs;  ++j )
		{
			if ( Handle h = Get1IndResource( type, j ) )
			{
				if ( fix_up_far_code( h )  ||  fix_up_near_code( h ) )
				{
					patching_applicable = true;
					
					if ( ! dry_run )
					{
						exit_status = (ChangedResource( h ), res_error())  ||
						              (CompactResFile( resfile ))          ||
						              (UpdateResFile( resfile ), res_error());
						
						if ( exit_status )
						{
							ERROR( 0, "error writing resource" );
							break;
						}
					}
				}
			}
			else
			{
				OSErr err = res_error();
				
				exit_status = ERROR( err ? -err : 36, "error loading resource" );
				
				break;
			}
		}
		
		if ( exit_status )
		{
			break;
		}
	}
	
	CloseResFile( resfile );
	
	if ( exit_status == 0  &&  ! patching_applicable )
	{
		exit_status = ERROR( 1, "no patchable resources" );
	}
	
	return exit_status;
}

/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
