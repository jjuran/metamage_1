/*
	icon.cc
	-------
	
	A program that draws an upscaled 'ICN#' using Core Graphics
	
	Copyright 2026, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
*/

// mac-cg-utils
#include "mac_cg/images.hh"

// icongen
#include "icongen/icongen.hh"

// upscaled-icongen
#include "icon.hh"


static
void augment_mask_with_face( Byte* mask, const Byte* face, int n )
{
	/*
		Some artists created icons in which some pixels
		were set in the icon's face but clear in the mask,
		taking advantage of the way icons were drawn in
		System 6 and earlier.  Later Mac OS treated such
		"wayward pixels" as outside, painting them white
		instead of black.  Since there's no difference in
		behavior between wayward pixels and outside pixels
		in later Mac OS, there's no motivation to use the
		former at all in that context -- thus all icons
		that use them can be assumed to have been made
		prior to the change and with the earlier behavior
		intended.  The operation below converts wayward
		pixels into normal (masked) black pixels.  The
		originally intended effect upon selection will be
		lost, but at least the unselected appearance will
		be correct.
	*/
	
	while ( n-- > 0 )
	{
		*mask++ |= *face++;
	}
}

static
CGImageRef create_icon_image( Byte* bits )
{
	using mac::cg::create_gray_paint_image;
	using mac::cg::create_image_mask;
	
	Byte* face_bits = bits;
	Byte* mask_bits = bits + 128;
	
	augment_mask_with_face( mask_bits, face_bits, 128 );
	
	CGImageRef face = create_gray_paint_image( 32, 32, 1, 4, face_bits );
	CGImageRef mask = create_image_mask      ( 32, 32, 1, 4, mask_bits );
	
	CGImageRef icon = NULL;
	
	if ( face  &&  mask )
	{
		icon = CGImageCreateWithMask( face, mask );
	}
	
	CGImageRelease( face );
	CGImageRelease( mask );
	
	return icon;
}

static
CGImageRef create_icon_image()
{
	int fd = open( input_path, O_RDONLY );
	
	if ( fd >= 0 )
	{
		Byte data[ 256 ];
		
		ssize_t n_read = read( fd, data, sizeof data );
		
		close( fd );
		
		if ( n_read == sizeof data )
		{
			return create_icon_image( data );
		}
	}
	
	return NULL;
}

void draw_upscaled_icon( CGContextRef c, size_t width, size_t height )
{
	if ( CGImageRef image = create_icon_image() )
	{
		CGContextScaleCTM( c, width / 32, height / 32 );
		
		CGContextDrawImage( c, CGRectMake( 0, 0, 32, 32 ), image );
		
		CGImageRelease( image );
	}
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
