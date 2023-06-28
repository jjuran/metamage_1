/*
	Copyright 2017-2018, Josh Juran.  All rights reserved.
	
	Assembly-optimized numerics functions for the Motorola 68K architecture.
	
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Affero General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Affero General Public License for more details.
	
	You should have received a copy of the GNU Affero General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

static asm
void long_multiply( uint64_t*  x : __A0,
                    uint32_t   a : __D0,
                    uint32_t   b : __D1 )
{
		/*
			A 68020 or later implements longword multiplication in a single
			instruction, which we'd like to use if available.  If codegen is
			specifically targeting 68020 (or later), then we can just use it.
			Otherwise, we'll test for it (though obliquely) -- just because
			our build is inclusive of the 68000 doesn't mean we don't want to
			have the advantage on a 68020+.
		*/
		
	#if ! __MC68020__
		
		/*
			This is an inclusive build, so we check.  A PC-relative jump has
			an offset relative to the word following the beginning of the JMP
			instruction (which is the /middle/ of the instruction, not the end
			of it).  So an offset of 2 jumps to the end of the JMP instruction
			(to the subsequent BRA) and an offset of 4 jumps over the BRA and
			directly to the 68020-only MULU.L.  The jump's effective address
			contains a scaling factor which is only recognized by a 68020+, so
			the scaled offset is 4 in that case but remains 2 on a 68000.  In
			the latter event, we branch over the 68020+ block and calculate
			via FOIL as we did previously.
		*/
		
		MOVEQ.L  #2,D2
		
	#if __MWERKS__ > 0x2200
		JMP      (PC, D2.W*2)
	#else
		DC.L     0x4efb2200  // JMP (PC, D2.W*2)
	#endif
		
		BRA.S    foil
		
	#endif
		
		machine  68020
		
		MULU.L   D0,D2:D1  // a * b -> D2 (high) and D1 (low)
		
		MOVE.L   D2,(A0)+
		MOVE.L   D1,(A0)
		
		RTS
	
	#if ! __MC68020__
		
	foil:
		
	/*
		Multiply 32-bit operands using a FOIL (first/outer/inner/last)
		strategy.  Instead of first and last, we have upper and lower
		words, which we'll distinguish in the comments by letter case:
		A, a, B, and b.  The result is just X, with the portion in use
		at any given step commented explicitly.
	*/
	
	MOVE.L   D0,D2
	MULU.W   D1,D2
	MOVE.L   D2,4(A0)  // a * b -> X (low longword)
	
	MOVEA.L  D0,A1     // save a for later
	SWAP     D0        // load A
	MOVE.L   D0,D2
	MULU.W   D1,D2     // A * b -> D2
	SWAP     D1        // load B
	MULU.W   D1,D0
	MOVE.L   D0,(A0)   // A * B -> X (high longword)
	
	MOVE.L   A1,D0     // load a
	MULU.W   D1,D0     // a * B -> D0
	
	MOVEQ.L  #0,D1
	ADD.L    D2,D0     // add (A * b) to (a * B)
	ADDX.W   D1,D1     // save carry bit in D1
	
	MOVEQ.L  #0,D2
	ADD.L    D0,2(A0)  // add (A * b) + (a * B) to X (middle)
	ADDX.W   D2,D1     // add carry bit to D1
	
	ADD.W    D1,(A0)   // add the carries to X (high longword)
	
	RTS
		
	#endif  // #if ! __MC68020__
}
