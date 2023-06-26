/*
	alloca.c
	--------
	
	Implementation of alloca() in 68K assembly language
	
	Copyright 2023, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
*/

#ifdef __MC68K__

#include <alloca.h>


asm
void* __relix_alloca( unsigned long size : __D0 )
{
	MOVEA.L  (SP)+,A1  // pop the return address
	NEG.L    D0        // reverse the polarity to match the stack direction
	AND.B    #~3,D0    // round up to a four-byte boundary
	ADDA.L   D0,SP     // add the alloc size to the stack
	MOVEA.L  SP,A0     // return the address of the new allocation
	JMP      (A1)      // jump to the return address
}

#endif

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
