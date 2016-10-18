/*
	icon.cc
	-------
	
	A program that draws the MacRelix icon using Core Graphics
	
	Copyright 2016, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	NOTE:  The AGPL applies to the following /software/ only.
	When executed, this code draws an image, which is itself
	a copyrighted work -- to which the AGPL does NOT apply.
	You are free to execute the program as provided (thereby
	generating the copyrighted image as output), but you are
	not granted any other rights with respect to the image.
	
	You are, of course, free to modify this software to generate
	an image of your own creation and redistribute your modified
	version of the software under the AGPL.
	
*/

#include "icon.hh"


static
void draw_cube( CGContextRef c )
{
	// Outline:
	
	CGContextMoveToPoint   ( c,  0,  8 );
	CGContextAddLineToPoint( c, -7,  4 );
	CGContextAddLineToPoint( c, -7, -4 );
	CGContextAddLineToPoint( c,  0, -8 );
	CGContextAddLineToPoint( c,  7, -4 );
	CGContextAddLineToPoint( c,  7,  4 );
	
	CGContextClosePath( c );
	CGContextStrokePath( c );
	
	CGContextSetLineJoin( c, kCGLineJoinBevel );
	
	// Top face:
	
	CGContextMoveToPoint   ( c,  0,  0 );
	CGContextAddLineToPoint( c,  7,  4 );
	CGContextAddLineToPoint( c,  0,  8 );
	CGContextAddLineToPoint( c, -7,  4 );
	
	CGContextClosePath( c );
	
	CGContextSetRGBFillColor( c, 0.85, 0.85, 1.0, 1.0 );
	CGContextDrawPath( c, kCGPathFillStroke );
	
	// Left face:
	
	CGContextMoveToPoint   ( c,  0,  0 );
	CGContextAddLineToPoint( c, -7,  4 );
	CGContextAddLineToPoint( c, -7, -4 );
	CGContextAddLineToPoint( c,  0, -8 );
	
	CGContextClosePath( c );
	
	CGContextSetRGBFillColor( c, 0.4, 0.4, 0.7, 1.0 );
	CGContextDrawPath( c, kCGPathFillStroke );
	
	// Right face:
	
	CGContextMoveToPoint   ( c,  0,  0 );
	CGContextAddLineToPoint( c,  7,  4 );
	CGContextAddLineToPoint( c,  7, -4 );
	CGContextAddLineToPoint( c,  0, -8 );
	
	CGContextClosePath( c );
	
	CGContextSetRGBFillColor( c, 0.2, 0.1, 0.5, 1.0 );
	CGContextDrawPath( c, kCGPathFillStroke );
}

static
void draw_cube_at( CGContextRef c, float x, float y )
{
	CGContextSaveGState( c );
	CGContextTranslateCTM( c, x, y );
	
	draw_cube( c );
	
	CGContextRestoreGState( c );
}

void draw_macrelix_icon( CGContextRef c, size_t width, size_t height )
{
	CGContextScaleCTM( c, width / 32, height / 32 );
	
	CGContextTranslateCTM( c, 16, 14 );
	
	draw_cube_at( c,  0,  8 );
	draw_cube_at( c, -7, -4 );
	draw_cube_at( c,  7, -4 );
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
