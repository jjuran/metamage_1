/*
	graphics.cc.hh
	--------------
	
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

#include "nyancat/graphics.hh"


namespace nyancat {

/*
	This header expects the following to be defined in namespace nyancat:
	
	  * these types:
	      * Color
	      * ColorRef
	      * class bitmap
	  * these colors:
	      * black
	      * white
	      * furrr
	      * blush
	      * toast
	      * icing
	      * spots
	      * red
	      * orange
	      * yellow
	      * green
	      * blue
	      * violet
*/

static const Color wave_colors[] =
{
	red,
	orange,
	yellow,
	green,
	blue,
	violet,
};

const int n_fireworks = 7;

struct firework_schedule
{
	int y;
	short t0;  // inital stage
	short dt;  // direction, 1 or -1
	
	int xes[ n_frames ];
};

/*
	Firework deltas:
		4, 6, 8, 8, 8, 6
*/

static const firework_schedule fireworks[ n_fireworks ] =
{
	{  1, 1,  1, { 42, 36, 28, 19, 10,  4,   0, -6, 68, 60, 52, 46 } },
	{ 10, 3,  1, { 68, 60, 52, 46, 42, 36,  28, 19, 10,  4,  0, -6 } },
	{ 22, 1,  1, {  1, -5, 69, 61, 53, 47,  43, 37, 29, 20, 11,  5 } },
	{ 44, 4,  1, { -9, -9, -9, -9, -9, 69,  61, 53, 44, -9, -9, -9 } },
	{ 44, 5,  1, { 11,  5,  1, -5, -9, -9,  -9, -9, -9, 37, 29, 20 } },
	{ 58, 1, -1, { 70, 66, 60, 51, 42, 34,  28, 24, 18, 10,  2, -1 } },
	{ 68, 2,  1, { 38, 30, 21, 12,  6,  2,  -4, 70, 62, 54, 48, 44 } },
};

static
void draw_wave( bitmap& bits, int x, int y, int stage, ColorRef color )
{
	const int a = ! stage;
	const int b = !! stage;
	
	int dx = 2 + a;
	int dy = 3;
	
	bits.fill_rect( x, y + a, dx, dy, color );  x += dx;  dx = 8;
	bits.fill_rect( x, y + b, dx, dy, color );  x += dx;  dx = 8;
	bits.fill_rect( x, y + a, dx, dy, color );  x += dx;  dx = 7 + b;
	bits.fill_rect( x, y + b, dx, dy, color );  x += dx;
	
	if ( stage == 0 )
	{
		bits.set_pixel( x, y, color );
	}
}

static
void draw_waves( bitmap& bits, int x, int y, bool step )
{
	for ( int i = 0;  i < 6;  ++i )
	{
		ColorRef color = wave_colors[ i ];
		
		draw_wave( bits, 0, y, step, color );
		
		y += 3;
	}
}

static
void draw_star( bitmap& bits, int x, int y, int stage )
{
	switch ( stage )
	{
		case 0:
			bits.set_pixel( x, y, white );
			break;
		
		case 1:
			bits.set_pixel( x, y - 1, white );
			bits.set_pixel( x - 1, y, white );
			bits.set_pixel( x + 1, y, white );
			bits.set_pixel( x, y + 1, white );
			break;
		
		case 2:
			bits.fill_rect( x, y - 2, 1, 2, white );
			bits.fill_rect( x - 2, y, 2, 1, white );
			bits.fill_rect( x + 1, y, 2, 1, white );
			bits.fill_rect( x, y + 1, 1, 2, white );
			break;
		
		case 3:
			bits.fill_rect( x, y - 3, 1, 2, white );
			bits.fill_rect( x - 3, y, 2, 1, white );
			bits.set_pixel( x, y, white );
			bits.fill_rect( x + 2, y, 2, 1, white );
			bits.fill_rect( x, y + 2, 1, 2, white );
			break;
		
		case 4:
			bits.set_pixel( x, y - 3, white );
			bits.set_pixel( x - 2, y - 2, white );
			bits.set_pixel( x + 2, y - 2, white );
			bits.set_pixel( x - 3, y, white );
			bits.set_pixel( x + 3, y, white );
			bits.set_pixel( x - 2, y + 2, white );
			bits.set_pixel( x + 2, y + 2, white );
			bits.set_pixel( x, y + 3, white );
			break;
		
		case 5:
			bits.set_pixel( x, y - 3, white );
			bits.set_pixel( x - 3, y, white );
			bits.set_pixel( x + 3, y, white );
			bits.set_pixel( x, y + 3, white );
			break;
		
		default:
			break;
	}
}

static
void draw_fireworks( bitmap& bits, int t )
{
	for ( int i = 0;  i < n_fireworks;  ++i )
	{
		const firework_schedule& schedule = fireworks[ i ];
		
		const int x = schedule.xes[ t ];
		
		int stage = (schedule.t0 + t * schedule.dt + 12) % 6;
		
		if ( x == -1 )
		{
			stage = 4;  // that one oddball
		}
		
		draw_star( bits, x, schedule.y, stage );
	}
}

static
void draw_tail_0( bitmap& bits, int x, int y )
{
	y -= 4;
	
	bits.fill_rect( x - 6, y, 4, 1, black );
	
	++y;
	
	bits.fill_rect( x - 6, y, 5, 1, black );
	bits.fill_rect( x - 5, y, 2, 1, furrr );
	
	++y;
	
	bits.fill_rect( x - 6, y, 6, 1, black );
	bits.fill_rect( x - 4, y, 2, 1, furrr );
	
	++y;
	
	bits.fill_rect( x - 5, y, 5, 1, black );
	bits.fill_rect( x - 3, y, 2, 1, furrr );
	
	++y;
	
	bits.fill_rect( x - 4, y, 2, 1, black );
	bits.fill_rect( x - 2, y, 2, 1, furrr );
	
	++y;
	
	bits.fill_rect( x - 3, y, 3, 1, black );
	
	++y;
	
	bits.set_pixel( x - 1, y, black );
}

static
void draw_tail_1( bitmap& bits, int x, int y )
{
	y -= 3;
	
	bits.fill_rect( x - 5, y, 2, 1, black );
	
	++y;
	
	bits.fill_rect( x - 6, y, 4, 1, black );
	bits.fill_rect( x - 5, y, 2, 1, furrr );
	
	++y;
	
	bits.fill_rect( x - 6, y, 6, 1, black );
	bits.fill_rect( x - 5, y, 2, 1, furrr );
	
	++y;
	
	bits.set_pixel( x - 5, y, black );
	bits.fill_rect( x - 4, y, 4, 1, furrr );
	
	++y;
	
	bits.fill_rect( x - 4, y, 2, 1, black );
	bits.fill_rect( x - 2, y, 2, 1, furrr );
	
	++y;
	
	bits.fill_rect( x - 2, y, 2, 1, black );
}

static
void draw_tail_2( bitmap& bits, int x, int y )
{
	bits.set_pixel( x - 1, y - 1, black );
	
	bits.fill_rect( x - 4, y, 4, 1, black );
	
	++y;
	
	bits.fill_rect( x - 6, y, 2, 1, black );
	bits.fill_rect( x - 4, y, 4, 1, furrr );
	
	++y;
	
	bits.fill_rect( x - 6, y, 6, 1, black );
	bits.fill_rect( x - 5, y, 3, 1, furrr );
	
	++y;
	
	bits.fill_rect( x - 5, y, 4, 1, black );
}

static
void draw_tail_3( bitmap& bits, int x, int y )
{
	bits.fill_rect( x - 2, y - 1, 2, 1, black );
	
	bits.fill_rect( x - 4, y, 2, 1, black );
	bits.fill_rect( x - 2, y, 2, 1, furrr );
	
	++y;
	
	bits.set_pixel( x - 5, y, black );
	bits.fill_rect( x - 4, y, 4, 1, furrr );
	
	++y;
	
	bits.fill_rect( x - 6, y, 6, 1, black );
	bits.fill_rect( x - 5, y, 2, 1, furrr );
	
	++y;
	
	bits.fill_rect( x - 6, y, 4, 1, black );
	bits.fill_rect( x - 5, y, 2, 1, furrr );
	
	++y;
	
	bits.fill_rect( x - 5, y, 2, 1, black );
}

static
void draw_tail_4( bitmap& bits, int x, int y )
{
	y -= 3;
	
	bits.fill_rect( x - 6, y, 4, 1, black );
	
	++y;
	
	bits.fill_rect( x - 7, y, 7, 2, black );
	bits.fill_rect( x - 6, y, 3, 1, furrr );
	
	++y;
	
	bits.fill_rect( x - 5, y, 4, 1, furrr );
	
	++y;
	
	bits.fill_rect( x - 5, y, 4, 1, black );
	bits.set_pixel( x - 1, y, furrr );
	
	++y;
	
	bits.set_pixel( x - 2, y, black );
	bits.set_pixel( x - 1, y, black );
}

static
void draw_tail_5( bitmap& bits, int x, int y )
{
	y -= 4;
	
	bits.fill_rect( x - 5, y, 2, 1, black );
	
	++y;
	
	bits.fill_rect( x - 6, y, 4, 1, black );
	bits.fill_rect( x - 5, y, 2, 1, furrr );
	
	++y;
	
	bits.fill_rect( x - 6, y, 6, 1, black );
	bits.fill_rect( x - 5, y, 2, 1, furrr );
	
	++y;
	
	bits.set_pixel( x - 5, y, black );
	bits.fill_rect( x - 4, y, 4, 1, furrr );
	
	++y;
	
	bits.fill_rect( x - 4, y, 2, 1, black );
	bits.fill_rect( x - 2, y, 2, 1, furrr );
	
	++y;
	
	bits.fill_rect( x - 2, y, 2, 1, black );
}

static
void draw_tail( bitmap& bits, int x, int y, int stage )
{
	switch ( stage )
	{
		case 0:  draw_tail_0( bits, x, y );  break;
		case 1:  draw_tail_1( bits, x, y );  break;
		case 2:  draw_tail_2( bits, x, y );  break;
		case 3:  draw_tail_3( bits, x, y );  break;
		case 4:  draw_tail_4( bits, x, y );  break;
		case 5:  draw_tail_5( bits, x, y );  break;
		
		default:
			break;
	}
}

static
void draw_paw_a( bitmap& bits, int x, int y )
{
	bits.fill_rect( x,     y, 4, 1, black );
	bits.fill_rect( x + 1, y, 2, 1, furrr );
	
	++y;
	
	bits.fill_rect( x + 1, y, 2, 1, black );
}

static
void draw_paw_b( bitmap& bits, int x, int y )
{
	draw_paw_a( bits, x, y );
	
	bits.set_pixel( x + 2, y - 1, furrr );
	bits.fill_rect( x + 3, y - 1, 1, 3, black );
}

static
void draw_paw_c( bitmap& bits, int x, int y )
{
	draw_paw_a( bits, x, y );
	
	bits.set_pixel( x, y + 1, black );
}

static
void draw_paw_d( bitmap& bits, int x, int y )
{
	draw_paw_c( bits, x - 2, y );
	
	y -= 2;
	
	bits.set_pixel( x - 1, y, black );
	
	++y;
	
	bits.set_pixel( x - 2, y, black );
	bits.fill_rect( x - 1, y, 3, 1, furrr );
	
	++y;
	
	bits.set_pixel( x + 2, y, black );
	
	++y;
	
	bits.set_pixel( x + 1, y, black );
}

static
void draw_paw_e( bitmap& bits, int x, int y )
{
	draw_paw_c( bits, x - 1, y );
	
	--y;
	
	bits.set_pixel( x - 1, y, black );
	bits.fill_rect( x,     y, 2, 1, furrr );
}

static
void draw_paw_f( bitmap& bits, int x, int y )
{
	draw_paw_c( bits, x, y );
	
	--y;
	
	bits.set_pixel( x,     y, black );
	bits.set_pixel( x + 1, y, furrr );
}

static
void draw_paw_gh( bitmap& bits, int x, int y )
{
	draw_paw_c( bits, x - 3, y );
	
	y -= 3;
	
	bits.set_pixel( x - 1, y, black );
	
	++y;
	
	bits.set_pixel( x - 2, y, black );
	
	++y;
	
	bits.set_pixel( x - 3, y, black );
	bits.set_pixel( x - 2, y, furrr );
	bits.set_pixel( x - 1, y, furrr );
	bits.set_pixel( x,     y, furrr );
	bits.set_pixel( x + 1, y, black );
}

static
void draw_paw_g( bitmap& bits, int x, int y )
{
	draw_paw_gh( bits, x, y );
	
	bits.set_pixel( x - 1, y - 2, black );
}

static
void draw_paw_h( bitmap& bits, int x, int y )
{
	draw_paw_gh( bits, x, y );
	
	bits.set_pixel( x - 1, y - 2, furrr );
}

static
void draw_paws_0( bitmap& bits, int x, int y )
{
	draw_paw_a( bits, x + 18, y );
	draw_paw_b( bits, x + 13, y );
	draw_paw_c( bits, x +  4, y );
	
	draw_paw_d( bits, x, y );
}

static
void draw_paws_1( bitmap& bits, int x, int y )
{
	draw_paw_b( bits, x + 19, y );
	draw_paw_b( bits, x + 14, y );
	draw_paw_b( bits, x +  4, y );
	
	draw_paw_e( bits, x, y );
}

static
void draw_paws_2( bitmap& bits, int x, int y )
{
	draw_paw_b( bits, x + 20, y );
	draw_paw_b( bits, x + 15, y );
	draw_paw_b( bits, x +  5, y );
	
	draw_paw_f( bits, x, y );
}

static
void draw_paws_3( bitmap& bits, int x, int y )
{
	draw_paws_1( bits, x, y );
	return;
	
	draw_paw_b( bits, x + 19, y );
	draw_paw_b( bits, x + 14, y );
	draw_paw_b( bits, x +  4, y );
	
	draw_paw_e( bits, x, y );
}

static
void draw_paws_4( bitmap& bits, int x, int y )
{
	draw_paw_b( bits, x + 17, y );
	draw_paw_b( bits, x + 12, y );
	draw_paw_b( bits, x +  2, y );
	
	draw_paw_g( bits, x, y );
}

static
void draw_paws_5( bitmap& bits, int x, int y )
{
	draw_paw_b( bits, x + 17, y );
	draw_paw_c( bits, x + 12, y );
	draw_paw_c( bits, x +  2, y );
	
	draw_paw_h( bits, x, y );
}

static
void draw_paws( bitmap& bits, int x, int y, int stage )
{
	switch ( stage )
	{
		case 0:  draw_paws_0( bits, x, y );  break;
		case 1:  draw_paws_1( bits, x, y );  break;
		case 2:  draw_paws_2( bits, x, y );  break;
		case 3:  draw_paws_3( bits, x, y );  break;
		case 4:  draw_paws_4( bits, x, y );  break;
		case 5:  draw_paws_5( bits, x, y );  break;
		
		default:
			break;
	}
}

static
void draw_body( bitmap& bits, int x, int y )
{
	const int width  = 21;
	const int height = 18;
	
	bits.fill_rect( x + 2, y,              width - 4, 1, black );
	bits.fill_rect( x + 2, y + 1,          width - 4, 1, toast );
	bits.fill_rect( x + 2, y + height - 2, width - 4, 1, toast );
	bits.fill_rect( x + 2, y + height - 1, width - 4, 1, black );
	
	bits.set_pixel( x +         1, y +          1, black );
	bits.set_pixel( x + width - 2, y +          1, black );
	bits.set_pixel( x +         1, y + height - 2, black );
	bits.set_pixel( x + width - 2, y + height - 2, black );
	
	bits.fill_rect( x,             y + 2, 1, height - 4, black );
	bits.fill_rect( x +         1, y + 2, 1, height - 4, toast );
	
	bits.fill_rect( x + 2, y + 2, width - 4, height - 4, icing );
	
	bits.fill_rect( x + width - 2, y + 2, 1, height - 4, toast );
	bits.fill_rect( x + width - 1, y + 2, 1, height - 4, black );
	
	bits.set_pixel( x, y + height - 2, black );
	
	bits.fill_rect( x +         2, y +          2, 2, 1, toast );
	bits.fill_rect( x + width - 4, y +          2, 2, 1, toast );
	bits.set_pixel( x +         2, y +          3, toast );
	bits.set_pixel( x + width - 3, y +          3, toast );
	bits.set_pixel( x +         2, y + height - 4, toast );
	bits.set_pixel( x + width - 3, y + height - 4, toast );
	bits.fill_rect( x +         2, y + height - 3, 2, 1, toast );
	bits.fill_rect( x + width - 4, y + height - 3, 2, 1, toast );
	
	bits.set_pixel( x +  9, y +  3, spots );
	bits.set_pixel( x + 12, y +  3, spots );
	bits.set_pixel( x +  4, y +  4, spots );
	bits.set_pixel( x + 16, y +  5, spots );
	bits.set_pixel( x +  8, y +  7, spots );
	bits.set_pixel( x +  5, y +  9, spots );
	bits.set_pixel( x +  9, y + 10, spots );
	bits.set_pixel( x +  3, y + 11, spots );
	bits.set_pixel( x +  7, y + 13, spots );
	bits.set_pixel( x +  4, y + 14, spots );
}

static
void draw_head( bitmap& bits, int x, int y )
{
	const int width  = 16;
	const int height = 13;
	
	bits.fill_rect( x +  2, y, 2, 1, black );
	bits.fill_rect( x + 12, y, 2, 1, black );
	
	++y;
	
	bits.fill_rect( x +  1, y, 4, 4, black );
	bits.fill_rect( x +  2, y, 2, 1, furrr );
	
	bits.fill_rect( x + 11, y, 4, 4, black );
	bits.fill_rect( x + 12, y, 2, 1, furrr );
	
	++y;
	
	bits.fill_rect( x +  2, y, 3, 1, furrr );
	bits.set_pixel( x +  5, y, black );
	
	bits.set_pixel( x + 10, y, black );
	bits.fill_rect( x + 11, y, 3, 1, furrr );
	
	++y;
	
	bits.fill_rect( x +  2, y, 12, 2, furrr );
	bits.fill_rect( x +  6, y,  4, 1, black );
	
	++y;
	++y;
	
	bits.fill_rect( x +  0, y, 16, 5, black );
	bits.fill_rect( x +  1, y, 14, 5, furrr );
	
	++y;
	
	bits.fill_rect( x +  4, y, 2, 2, black );
	bits.set_pixel( x +  4, y, white );
	
	bits.fill_rect( x + 11, y, 2, 2, black );
	bits.set_pixel( x + 11, y, white );
	
	++y;
	
	bits.set_pixel( x +  9, y, black );
	
	++y;
	
	bits.fill_rect( x +  2, y, 2, 2, blush );
	bits.fill_rect( x + 13, y, 2, 2, blush );
	
	++y;
	
	bits.set_pixel( x +  5, y, black );
	bits.set_pixel( x +  8, y, black );
	bits.set_pixel( x + 11, y, black );
	
	++y;
	
	bits.fill_rect( x +  1, y, 14, 1, black );
	bits.fill_rect( x +  2, y,  3, 1, furrr );
	bits.fill_rect( x + 12, y,  2, 1, furrr );
	
	++y;
	
	bits.fill_rect( x +  2, y, 12, 1, black );
	bits.fill_rect( x +  3, y, 10, 1, furrr );
	
	++y;
	
	bits.fill_rect( x +  3, y, 10, 1, black );
}

void draw_frame( bitmap& bits, int t )
{
	const bool step = t & 0x2;
	
	draw_waves( bits, 0, 26, step );
	
	draw_fireworks( bits, t );
	
	const int body_base_x = 25;
	const int body_base_y = 25;
	
	const int head_base_x = body_base_x + 10;
	const int head_base_y = body_base_y +  5;
	
	const int body_y = body_base_y + (t/2u % 3 > 0);
	const int head_x = head_base_x + (t + 2) / 3 % 2u;
	const int head_y = head_base_y + (t + 1) / 3 % 2u;
	
	const int tail_x = body_base_x;
	const int tail_y = body_y + 11;
	
	const int paws_x = body_base_x;
	const int paws_y = body_y + 18;
	
	draw_paws( bits, paws_x,      paws_y, t % 6 );
	draw_tail( bits, tail_x,      tail_y, t % 6 );
	draw_body( bits, body_base_x, body_y );
	draw_head( bits, head_x,      head_y );
}

}  // namespace nyancat
