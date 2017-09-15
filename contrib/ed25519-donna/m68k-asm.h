/*
	Copyright 2017, Josh Juran.  All rights reserved.
	
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
}
