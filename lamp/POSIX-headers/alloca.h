/*
	alloca.h
	--------
	
	Interface to alloca()
	
	Copyright 2023, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	To use the provided 68K implementation of alloca(), this header
	must be included rather than the one in CWCIncludes.  (Including
	the wrong header will result in a link error.)
	
	The distinct name `__relix_alloca()` is used because the signature
	is different, with the result in A0 rather than D0.  Despite the
	name, it's functional in any context, not just MacRelix.
	
	For PPC, the <alloca.h> headers are interchangeable and it doesn't
	matter which one is included.  (MWCPPC has __alloca() built-in.)
	
*/

#ifndef _ALLOCA_H_
#define _ALLOCA_H_


#ifdef __MC68K__

extern void* __relix_alloca( unsigned long size : __D0 );

#define __alloca( size )  __relix_alloca( size )

#endif


#define alloca( size )  __alloca( size )

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
