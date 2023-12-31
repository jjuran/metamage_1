/*
	postlink.cc
	-----------
	
	Post-linker for classic Mac OS stand-alone 68K code resources
	
	Copyright 2023, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	====================================================================
	NOTE:  The comments below contain a disassembly of executable code
	whose copyright was held by Metrowerks (until they were purchased).
	The /comments/ are copyright Joshua Juran, but the code to which
	they correspond is not, and that code is not covered by the AGPL.
	The disassembly of the code is presented here for the scholarly
	purpose of documenting the behavior of the post-linker, whose own
	purpose is that of interoperability.  Fair use applies.
	
	However, get_address_mask() is original code copyright Joshua Juran.
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
	squeeze in a system version check.  Our modified startup code only
	calls StripAddress() in System 6[1] and later, where it's guaranteed
	to exist.  In earlier systems, it defaults to Lo3Bytes ($00FFFFFF) as
	an address mask.
	
	While it's possible to boot System 4.2 on a Mac II, which has a 68020
	processor and 32-bit addresses, the maximum usable RAM is 8 MiB, so
	code will never be loaded above $00800000, and which mechanism is used
	to mask the address is moot.
	
	[1] More specifically, the modified startup code calls StripAddress()
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
	
	The goal is to insert new code that determines whether or not to call
	StripAddress(), or assume 24-bit mode and mask with Lo3Bytes.  We'll
	make some space by consolidating redundant code.
	
	SetCurrentA4() and __GetMainXRef__() each contain an offset that we
	need, but are otherwise replaced completely.  SetCurrentA4() becomes
	get_address_mask(), which returns either 0x00FFFFFF or 0xFFFFFFFF in
	D0.  __Startup__() places the stripped code start address in D0.  Then
	A4 is set to the stripped data start address, using the offset taken
	from SetCurrentA4().  The additional code in __Startup__() overwrites
	the beginning of __GetMainXRef__(); the rest is filled with NOPs.
	
	Next, __Startup__() calls __SetupMainRsrc__(), passing the stripped
	code start address in D0.  __SetupMainRsrc__() checks to see whether
	the code address is unchanged (in which case no relocation is needed),
	whether this is a subsequent relocation, and if not, whether _HWPriv
	is implemented (which is cached for next time).
	
	The Metrowerks code for __SetupMainRsrc__() calls __GetMainXRef__() to
	get the stripped xref start address, and then calls __relocate_compr()
	with three arguments pushed in this order: the relocation delta, the
	stripped code start address, and the stripped xref start address.  Our
	patched code replaces the __GetMainXRef__() with a register move that
	places the stripped code start address directly into A1 (where it will
	be used) and a NOP.  The two instructions in __relocate_compr() that
	unpack the xref start and code start parameters are replaced with code
	to calculate the xref start in A0 (using the offset originally from
	__GetMainXRef__()).  (A1 is set to the code start by the caller.)
	
	After __SetupMainRsrc__() returns, __Startup__() restores registers
	and tail-calls main(), thus completing startup.
	
	Legend
	------
	
			instruction unchanged
		!	instruction slightly changed (e.g. different register operand)
		^	instruction moved up (possibly with minor changes)
		v	instruction moved down within its function
		V	instruction moved down to a different function
		x	instruction deleted
		*	instruction inserted
	
	Old startup code:
	
		__Startup__:
		...
		00000c:  MOVEM.L  D0-D2/A0-A1/A4,-(A7)    // save registers
		000010:  JSR      *+244    // $000104     // SetCurrentA4
		000014:  LEA      *-20,A0    // $000000
!		000018:  MOVE.L   A0,D0                   // start of code
x		00001a:  _StripAddress
v		00001c:  JSR      *+146    // $0000ae     // __SetupMainRsrc__
v		000020:  MOVEM.L  (A7)+,D0-D2/A0-A1/A4    // restore registers
v		000024:  JMP      *+694    // $0002da     // main
		
		__GetMainXRef__:
x		000028:  LEA      *-40,A0    // $000000
V		00002c:  ADDA.L   #0x00000yyy,A0          // moved to __relocate_compr
x		000032:  MOVE.L   A0,D0
x		000034:  _StripAddress
x		000036:  RTS
		
		__relocate_compr:
		000038:  MOVEM.L  D5-D6,-(A7)
		00003c:  SUBQ.W   #4,A7
x		00003e:  MOVEA.L  (16,A7),A0              // stripped start of xref
x		000042:  MOVEA.L  (20,A7),A1              // stripped start of code
		000046:  MOVE.L   (24,A7),D6              // relocation delta
		...
		0000ac:  RTS
		
		__SetupMainRsrc__:
		0000ae:  MOVEM.L  D3/A2,-(A7)
		0000b2:  MOVEA.L  D0,A2
		...
		0000de:  MOVE.L   D3,-(A7)                // relocation delta
		0000e0:  MOVE.L   A2,-(A7)                // stripped start of code
x		0000e2:  JSR      *-186    // $000028     // __GetMainXRef__
		0000e6:  MOVE.L   D0,-(A7)                // stripped start of xref
		0000e8:  JSR      *-176    // $000038     // __relocate_compr
		...
		000102:  RTS
		
		SetCurrentA4:
x		000104:  LEA      *-260,A0    // $000000
^		000108:  ADDA.L   #0x00000xxx,A0          // moved to __Startup__
x		00010e:  MOVE.L   A0,D0
v		000110:  _StripAddress
x		000112:  EXG      D0,A4                   // stripped start of data
		000114:  RTS
		
	New startup code:
	
		__Startup__:
		...
		00000c:  MOVEM.L  D0-D2/A0-A1/A4,-(A7)    // save registers
		000010:  JSR      *+244    // $000104     // get_address_mask
		000014:  LEA      *-20,A0    // $000000
!		000018:  MOVE.L   A0,D1                   // start of code
*		00001a:  AND.L    D1,D0                   // stripped start of code
*		00001c:  MOVEA.L  D0,A4
^		00001e:  ADDA.L   #0x00000xxx,A4          // stripped start of data
v		000024:  JSR      *+138    // $0000ae     // __SetupMainRsrc__
v		000028:  MOVEM.L  (A7)+,D0-D2/A0-A1/A4    // restore registers
v		00002c:  JMP      *+???    // $000zzz     // main (addr varies)
		
*		000030:  NOP
*		000032:  NOP
*		000034:  NOP
*		000036:  NOP
		
		__relocate_compr:
		000038:  MOVEM.L  D5-D6,-(A7)
		00003c:  SUBQ.W   #4,A7
*		00003e:  MOVEA.L  A2,A0                   // stripped start of code
V		000040:  ADDA.L   #0x00000yyy,A0          // stripped start of xref
		000046:  MOVE.L   (24,A7),D6              // relocation delta
		...
		0000ac:  RTS
	
		__SetupMainRsrc__:
		0000ae:  MOVEM.L  D3/A2,-(A7)
		0000b2:  MOVEA.L  D0,A2
		...
		0000de:  MOVE.L   D3,-(A7)                // relocation delta
		0000e0:  MOVE.L   A2,-(A7)                // stripped start of code
*		0000e2:  MOVEA.L  A2,A1
*		0000e4:  NOP
		0000e6:  MOVE.L   D0,-(A7)                // stripped start of xref
		0000e8:  JSR      *-176    // $000038     // __relocate_compr
		...
		000102:  RTS
		
		get_address_mask:
*		000104:  MOVE.L   Lo3Bytes,D0
*		000108:  CMPI.W   #minimum_sysv,SysVersion
*		00010e:  BLT.S    *+6                     // mask_set
*		000110:  MOVEQ.L  #-1,D0
v		000112:  _StripAddress
	mask_set:
		000114:  RTS
		
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

const UInt16 minimum_sysv = 0x0604;  // System 6.0.4

const UInt16 SysVersion = 0x015a;
const UInt16 Lo3Bytes =   0x031a;

const UInt16 _StripAddress = 0xA055;

const UInt16 AND_L_D1_D0  = 0xc081;  // AND.L    D1,D0
const UInt16 BLT_S_6      = 0x6d04;  // BLT.S    *+6
const UInt16 BRA_S_12     = 0x600a;  // BRA.S    *+12
const UInt16 CMPI_W_abs_W = 0x0c78;  // CMPI.W   #imm,0x____
const UInt16 JMP_PCrel    = 0x4efa;  // JMP      *...
const UInt16 MOVE_L_A0_D0 = 0x2008;  // MOVE.L   A0,D0
const UInt16 MOVE_L_A0_D1 = 0x2208;  // MOVE.L   A0,D1
const UInt16 MOVE_L_A2_A0 = 0x204a;  // MOVEA.L  A2,A0
const UInt16 MOVE_L_A2_A1 = 0x224a;  // MOVEA.L  A2,A1
const UInt16 MOVE_L_D0_A4 = 0x2840;  // MOVEA.L  D0,A4
const UInt16 MOVEQ_FF_D0  = 0x70FF;  // MOVEQ    #-1,D0
const UInt16 NOP          = 0x4e71;  // NOP
const UInt16 RTS          = 0x4e75;  // RTS

const UInt16 ADDA_L_abs_L_A0 = 0xd1fc;  // ADDA.L   #0x________,A0
const UInt16 ADDA_L_abs_L_A4 = 0xd9fc;  // ADDA.L   #0x________,A4
const UInt16 MOVE_L_abs_W_D0 = 0x2038;  // MOVE.L   0x____,D0

const UInt32 JMP_6                     = JMP_PCrel    << 16 | 0x0004;

const UInt32 MOVE_L_A0_D0_StripAddress = MOVE_L_A0_D0 << 16 | _StripAddress;
const UInt32 MOVE_L_A2_A1_NOP          = MOVE_L_A2_A1 << 16 | NOP;

const UInt32 NOP_NOP = 0x4e714e71;

static const UInt16 get_address_mask[] =
{
	MOVE_L_abs_W_D0,
	Lo3Bytes,         // MOVE.L   Lo3Bytes,D0
	
	CMPI_W_abs_W,
	minimum_sysv,
	SysVersion,       // CMPI.W   #minimum_sysv,SysVersion
	
	BLT_S_6,          // BLT.S    *+6
	MOVEQ_FF_D0,      // MOVEQ.L  #-1,D0
	_StripAddress,    // _StripAddress
	RTS,              // RTS
};

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
	
	UInt32* pL = NULL;
	UInt32* qL = NULL;
	
	UInt16* qW = NULL;
	
	/*
		__SetupMainRsrc__
		
		  * replace the call to __GetMainXRef__() with a register move
	*/
	
	qL = (UInt32*) (p + 0x00e2);
	
	*qL = MOVE_L_A2_A1_NOP;
	
	/*
		__relocate_compr
		
		  * replace arg loads to A0 and A1 with A0 = A2 + xref offset
	*/
	
	qW = (UInt16*) (p + 0x003e);
	
	*qW++ = MOVE_L_A2_A0;     // MOVEA.L  A2,A0
	
	*qW++ = ADDA_L_abs_L_A0;  // ADDA.L   #0x________,A0
	
	pL = (UInt32*) (p + 0x002e);
	qL = (UInt32*) qW;
	
	*qL = *pL;                // copy ADDA immediate operand (xref offset)
	
	/*
		__Startup__, __GetMainXRef__
		
		  * NOP out end of __GetMainXRef__()
		  * move JSR, MOVEM, and JMP at end of __Startup__() down 8 bytes
		  * copy ADDA instruction from SetCurrentA4()
		  * insert new MOVE and AND instructions
	*/
	
	qL = (UInt32*) (p + 0x0038);
	
	*--qL = NOP_NOP;
	*--qL = NOP_NOP;
	
	pL = qL - 2;
	
	*--qL = *--pL - 8;   // move JMP to main down
	*--qL = *--pL;       // move MOVEM restore down
	*--qL = *--pL - 8;   // move JSR to __SetupMainRsrc__ down
	
	pL = (UInt32*) (p + 0x010a);
	
	*--qL = *pL;         // copy ADDA immediate operand (data offset)
	
	qW = (UInt16*) qL;
	
	*--qW = ADDA_L_abs_L_A4;  // ADDA.L   #0x________,A4
	
	*--qW = MOVE_L_D0_A4;  // MOVEA.L  D0,A4
	*--qW = AND_L_D1_D0;   // AND.L    D1,D0
	*--qW = MOVE_L_A0_D1;  // MOVE.L   A0,D1
	
	/*
		SetCurrentA4 -> get_address_mask
		
		  * overwrite with new code
	*/
	
	mempcpy( p + 0x0104, get_address_mask, sizeof get_address_mask );
	
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
