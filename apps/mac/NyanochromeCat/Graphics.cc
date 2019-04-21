/*
	Graphics.cc
	-----------
	
	Programmatic drawing of Josh Juran's Nyanochrome Cat, a parody of
	Christopher Torres' 2011 Nyan Cat animation.
	
	Copyright 2017, Josh Juran.  All rights reserved.
	
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Affero General Public License as published
	by the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Affero General Public License for more details.
	
	You should have received a copy of the GNU Affero General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
	
	Nyanochrome Cat is copyright 2017, Josh Juran.  All rights reserved.
*/

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// NyanochromeCat
#include "Bitmap.hh"


namespace nyancat {

typedef Pattern         Color;
typedef Pattern const&  ColorRef;

const Pattern black  = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
const Pattern dkGray = { 0x77, 0xDD, 0x77, 0xDD, 0x77, 0xDD, 0x77, 0xDD };
const Pattern gray   = { 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 };
const Pattern ltGray = { 0x88, 0x22, 0x88, 0x22, 0x88, 0x22, 0x88, 0x22 };
const Pattern white  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

const Pattern veryDarkGray = { 0x77, 0xFF, 0xDD, 0xFF, 0x77, 0xFF, 0xDD, 0xFF };

// Cat head/legs/tail
const Pattern& furrr = gray;
const Pattern& blush = ltGray;

// Pop-tart body
const Pattern& toast = white;
const Pattern& icing = ltGray;
const Pattern& spots = dkGray;

// Rainbow waves
const Pattern& red    = gray;
const Pattern& orange = ltGray;
const Pattern& yellow = white;
const Pattern& green  = ltGray;
const Pattern& blue   = gray;
const Pattern& violet = dkGray;

}  // namespace nyancat

// nyancatlib
#include "nyancat/graphics.cc.hh"
