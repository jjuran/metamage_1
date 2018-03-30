/*
	graphics.cc
	-----------
	
	Programmatic drawing of Christopher Torres' 2011 Nyan Cat animation.
	
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
*/

// nyancat
#include "bitmap.hh"


namespace nyancat {

typedef uint32_t Color, ColorRef;

const uint32_t black = 0xFF000000;
const uint32_t white = 0xFFFFFFFF;

// Cat head/legs/tail
const uint32_t furrr = 0xFFBBBBBB;
const uint32_t blush = 0xFFFFCCCC;

// Pop-tart body
const uint32_t toast = 0xFFFFCC88;
const uint32_t icing = 0xFFFF99FF;
const uint32_t spots = 0xFFFF00FF;

// Rainbow waves
const uint32_t red    = 0xFFFF0000;
const uint32_t orange = 0xFFFF8800;
const uint32_t yellow = 0xFFFFFF00;
const uint32_t green  = 0xFF00FF00;
const uint32_t blue   = 0xFF5588FF;
const uint32_t violet = 0xFF6644AA;

}  // namespace nyancat

// nyancatlib
#include "nyancat/graphics.cc.hh"
