/*
	graphics.cc
	-----------
*/

#include "bitmap.hh"


bool alt_head;
bool alt_body;
bool alt_wake;

int alt_head_x;
int alt_head_y;

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

static const uint32_t wave_colors[] =
{
	red,
	orange,
	yellow,
	green,
	blue,
	violet,
};

const uint32_t alt_green  = green;
const uint32_t alt_yellow = yellow;
const uint32_t alt_orange = orange;
const uint32_t alt_red    = red;
const uint32_t magenta    = 0xFFFF00FF;
const uint32_t alt_blue   = 0xFF0000FF;

static const uint32_t alt_wave_colors[] =
{
	alt_green,
	alt_yellow,
	alt_orange,
	alt_red,
	magenta,
	alt_blue,
};

const int n_frames = 12;

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
	{ 22, 1,  1, {  1, -5, 69, 61, 53, 47,  43, 37, 29, 20, 11, 5, } },
	{ 44, 4,  1, { -9, -9, -9, -9, -9, 69,  61, 53, 44, -9, -9, -9 } },
	{ 44, 5,  1, { 11,  5,  1, -5, -9, -9,  -9, -9, -9, 37, 29, 20 } },
	{ 58, 1, -1, { 70, 66, 60, 51, 42, 34,  28, 24, 18, 10,  2, -1 } },
	{ 68, 2,  1, { 38, 30, 21, 12,  6,  2,  -4, 70, 62, 54, 48, 44 } },
};

static
void fill( uint32_t* begin, uint32_t* end, uint32_t x )
{
	while ( begin < end )
	{
		*begin++ = x;
	}
}

static
void draw_wave( bitmap& bits, int x, int y, int stage, uint32_t color )
{
	const int a = ! stage;
	const int b = !! stage;
	
	bits.set_pixel( x++, y +   a, color );
	bits.set_pixel( x++, y +   a, color );
	bits.set_pixel( x++, y + 1, color );
	bits.set_pixel( x++, y +   b, color );
	bits.set_pixel( x++, y +   b, color );
	bits.set_pixel( x++, y +   b, color );
	bits.set_pixel( x++, y +   b, color );
	bits.set_pixel( x++, y +   b, color );
	bits.set_pixel( x++, y +   b, color );
	bits.set_pixel( x++, y +   b, color );
	bits.set_pixel( x++, y +     0, color );
	bits.set_pixel( x++, y +   a, color );
	bits.set_pixel( x++, y +   a, color );
	bits.set_pixel( x++, y +   a, color );
	bits.set_pixel( x++, y +   a, color );
	bits.set_pixel( x++, y +   a, color );
	bits.set_pixel( x++, y +   a, color );
	bits.set_pixel( x++, y +   a, color );
	bits.set_pixel( x++, y + 1, color );
	bits.set_pixel( x++, y +   b, color );
	bits.set_pixel( x++, y +   b, color );
	bits.set_pixel( x++, y +   b, color );
	bits.set_pixel( x++, y +   b, color );
	bits.set_pixel( x++, y +   b, color );
	bits.set_pixel( x++, y +   b, color );
	bits.set_pixel( x++, y +   b, color );
	
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
		const uint32_t color = (alt_wake ? alt_wave_colors : wave_colors)[ i ];
		
		draw_wave( bits, 0, y++, step, color );
		draw_wave( bits, 0, y++, step, color );
		draw_wave( bits, 0, y++, step, color );
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
			bits.set_pixel( x, y - 2, white );
			bits.set_pixel( x, y - 1, white );
			bits.set_pixel( x - 2, y, white );
			bits.set_pixel( x - 1, y, white );
			bits.set_pixel( x + 1, y, white );
			bits.set_pixel( x + 2, y, white );
			bits.set_pixel( x, y + 1, white );
			bits.set_pixel( x, y + 2, white );
			break;
		
		case 3:
			bits.set_pixel( x, y - 3, white );
			bits.set_pixel( x, y - 2, white );
			bits.set_pixel( x - 3, y, white );
			bits.set_pixel( x - 2, y, white );
			bits.set_pixel( x, y, white );
			bits.set_pixel( x + 2, y, white );
			bits.set_pixel( x + 3, y, white );
			bits.set_pixel( x, y + 2, white );
			bits.set_pixel( x, y + 3, white );
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
	
	bits.set_pixel( x - 6, y, black );
	bits.set_pixel( x - 5, y, black );
	bits.set_pixel( x - 4, y, black );
	bits.set_pixel( x - 3, y, black );
	
	++y;
	
	bits.set_pixel( x - 6, y, black );
	bits.set_pixel( x - 5, y, furrr );
	bits.set_pixel( x - 4, y, furrr );
	bits.set_pixel( x - 3, y, black );
	bits.set_pixel( x - 2, y, black );
	
	++y;
	
	bits.set_pixel( x - 6, y, black );
	bits.set_pixel( x - 5, y, black );
	bits.set_pixel( x - 4, y, furrr );
	bits.set_pixel( x - 3, y, furrr );
	bits.set_pixel( x - 2, y, black );
	bits.set_pixel( x - 1, y, black );
	
	++y;
	
	bits.set_pixel( x - 5, y, black );
	bits.set_pixel( x - 4, y, black );
	bits.set_pixel( x - 3, y, furrr );
	bits.set_pixel( x - 2, y, furrr );
	bits.set_pixel( x - 1, y, black );
	
	++y;
	
	bits.set_pixel( x - 4, y, black );
	bits.set_pixel( x - 3, y, black );
	bits.set_pixel( x - 2, y, furrr );
	bits.set_pixel( x - 1, y, furrr );
	
	++y;
	
	bits.set_pixel( x - 3, y, black );
	bits.set_pixel( x - 2, y, black );
	bits.set_pixel( x - 1, y, black );
	
	++y;
	
	bits.set_pixel( x - 1, y, black );
}

static
void draw_tail_1( bitmap& bits, int x, int y )
{
	y -= 3;
	
	bits.set_pixel( x - 5, y, black );
	bits.set_pixel( x - 4, y, black );
	
	++y;
	
	bits.set_pixel( x - 6, y, black );
	bits.set_pixel( x - 5, y, furrr );
	bits.set_pixel( x - 4, y, furrr );
	bits.set_pixel( x - 3, y, black );
	
	++y;
	
	bits.set_pixel( x - 6, y, black );
	bits.set_pixel( x - 5, y, furrr );
	bits.set_pixel( x - 4, y, furrr );
	bits.set_pixel( x - 3, y, black );
	bits.set_pixel( x - 2, y, black );
	bits.set_pixel( x - 1, y, black );
	
	++y;
	
	bits.set_pixel( x - 5, y, black );
	bits.set_pixel( x - 4, y, furrr );
	bits.set_pixel( x - 3, y, furrr );
	bits.set_pixel( x - 2, y, furrr );
	bits.set_pixel( x - 1, y, furrr );
	
	++y;
	
	bits.set_pixel( x - 4, y, black );
	bits.set_pixel( x - 3, y, black );
	bits.set_pixel( x - 2, y, furrr );
	bits.set_pixel( x - 1, y, furrr );
	
	++y;
	
	bits.set_pixel( x - 2, y, black );
	bits.set_pixel( x - 1, y, black );
}

static
void draw_tail_2( bitmap& bits, int x, int y )
{
	bits.set_pixel( x - 1, y - 1, black );
	
	bits.set_pixel( x - 4, y, black );
	bits.set_pixel( x - 3, y, black );
	bits.set_pixel( x - 2, y, black );
	bits.set_pixel( x - 1, y, black );
	
	++y;
	
	bits.set_pixel( x - 6, y, black );
	bits.set_pixel( x - 5, y, black );
	bits.set_pixel( x - 4, y, furrr );
	bits.set_pixel( x - 3, y, furrr );
	bits.set_pixel( x - 2, y, furrr );
	bits.set_pixel( x - 1, y, furrr );
	
	++y;
	
	bits.set_pixel( x - 6, y, black );
	bits.set_pixel( x - 5, y, furrr );
	bits.set_pixel( x - 4, y, furrr );
	bits.set_pixel( x - 3, y, furrr );
	bits.set_pixel( x - 2, y, black );
	bits.set_pixel( x - 1, y, black );
	
	++y;
	
	bits.set_pixel( x - 5, y, black );
	bits.set_pixel( x - 4, y, black );
	bits.set_pixel( x - 3, y, black );
	bits.set_pixel( x - 2, y, black );
}

static
void draw_tail_3( bitmap& bits, int x, int y )
{
	bits.set_pixel( x - 2, y - 1, black );
	bits.set_pixel( x - 1, y - 1, black );
	
	bits.set_pixel( x - 4, y, black );
	bits.set_pixel( x - 3, y, black );
	bits.set_pixel( x - 2, y, furrr );
	bits.set_pixel( x - 1, y, furrr );
	
	++y;
	
	bits.set_pixel( x - 5, y, black );
	bits.set_pixel( x - 4, y, furrr );
	bits.set_pixel( x - 3, y, furrr );
	bits.set_pixel( x - 2, y, furrr );
	bits.set_pixel( x - 1, y, furrr );
	
	++y;
	
	bits.set_pixel( x - 6, y, black );
	bits.set_pixel( x - 5, y, furrr );
	bits.set_pixel( x - 4, y, furrr );
	bits.set_pixel( x - 3, y, black );
	bits.set_pixel( x - 2, y, black );
	bits.set_pixel( x - 1, y, black );
	
	++y;
	
	bits.set_pixel( x - 6, y, black );
	bits.set_pixel( x - 5, y, furrr );
	bits.set_pixel( x - 4, y, furrr );
	bits.set_pixel( x - 3, y, black );
	
	++y;
	
	bits.set_pixel( x - 5, y, black );
	bits.set_pixel( x - 4, y, black );
}

static
void draw_tail_4( bitmap& bits, int x, int y )
{
	y -= 3;
	
	bits.set_pixel( x - 6, y, black );
	bits.set_pixel( x - 5, y, black );
	bits.set_pixel( x - 4, y, black );
	bits.set_pixel( x - 3, y, black );
	
	++y;
	
	bits.set_pixel( x - 7, y, black );
	bits.set_pixel( x - 6, y, furrr );
	bits.set_pixel( x - 5, y, furrr );
	bits.set_pixel( x - 4, y, furrr );
	bits.set_pixel( x - 3, y, black );
	bits.set_pixel( x - 2, y, black );
	bits.set_pixel( x - 1, y, black );
	
	++y;
	
	bits.set_pixel( x - 7, y, black );
	bits.set_pixel( x - 6, y, black );
	bits.set_pixel( x - 5, y, furrr );
	bits.set_pixel( x - 4, y, furrr );
	bits.set_pixel( x - 3, y, furrr );
	bits.set_pixel( x - 2, y, furrr );
	bits.set_pixel( x - 1, y, black );
	
	++y;
	
	bits.set_pixel( x - 5, y, black );
	bits.set_pixel( x - 4, y, black );
	bits.set_pixel( x - 3, y, black );
	bits.set_pixel( x - 2, y, black );
	bits.set_pixel( x - 1, y, furrr );
	
	++y;
	
	bits.set_pixel( x - 2, y, black );
	bits.set_pixel( x - 1, y, black );
}

static
void draw_tail_5( bitmap& bits, int x, int y )
{
	y -= 4;
	
	bits.set_pixel( x - 5, y, black );
	bits.set_pixel( x - 4, y, black );
	
	++y;
	
	bits.set_pixel( x - 6, y, black );
	bits.set_pixel( x - 5, y, furrr );
	bits.set_pixel( x - 4, y, furrr );
	bits.set_pixel( x - 3, y, black );
	
	++y;
	
	bits.set_pixel( x - 6, y, black );
	bits.set_pixel( x - 5, y, furrr );
	bits.set_pixel( x - 4, y, furrr );
	bits.set_pixel( x - 3, y, black );
	bits.set_pixel( x - 2, y, black );
	bits.set_pixel( x - 1, y, black );
	
	++y;
	
	bits.set_pixel( x - 5, y, black );
	bits.set_pixel( x - 4, y, furrr );
	bits.set_pixel( x - 3, y, furrr );
	bits.set_pixel( x - 2, y, furrr );
	bits.set_pixel( x - 1, y, furrr );
	
	++y;
	
	bits.set_pixel( x - 4, y, black );
	bits.set_pixel( x - 3, y, black );
	bits.set_pixel( x - 2, y, furrr );
	bits.set_pixel( x - 1, y, furrr );
	
	++y;
	
	bits.set_pixel( x - 2, y, black );
	bits.set_pixel( x - 1, y, black );
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
	bits.set_pixel( x,     y, black );
	bits.set_pixel( x + 1, y, furrr );
	bits.set_pixel( x + 2, y, furrr );
	bits.set_pixel( x + 3, y, black );
	
	++y;
	
	bits.set_pixel( x + 1, y, black );
	bits.set_pixel( x + 2, y, black );
}

static
void draw_paw_b( bitmap& bits, int x, int y )
{
	draw_paw_a( bits, x, y );
	
	bits.set_pixel( x + 2, y - 1, furrr );
	bits.set_pixel( x + 3, y - 1, black );
	bits.set_pixel( x + 3, y + 1, black );
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
	//bits.set_pixel( x,     y, black );
	
	++y;
	
	bits.set_pixel( x - 2, y, black );
	bits.set_pixel( x - 1, y, furrr );
	bits.set_pixel( x,     y, furrr );
	bits.set_pixel( x + 1, y, furrr );
	
	++y;
	
	bits.set_pixel( x + 2, y, black );
	
	++y;
	
	bits.set_pixel( x + 1, y, black );
}

static
void draw_paw_e( bitmap& bits, int x, int y )
{
	draw_paw_c( bits, x - 1, y );
	
	y -= 2;
	
	//bits.set_pixel( x, y, black );
	
	++y;
	
	bits.set_pixel( x - 1, y, black );
	bits.set_pixel( x,     y, furrr );
	bits.set_pixel( x + 1, y, furrr );
}

static
void draw_paw_f( bitmap& bits, int x, int y )
{
	draw_paw_c( bits, x, y );
	
	y -= 2;
	
	//bits.set_pixel( x, y, black );
	
	++y;
	
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
	
	for ( int i = x + 2;  i < x + width - 2;  ++i )
	{
		bits.set_pixel( i, y,              black );
		bits.set_pixel( i, y + 1,          toast );
		bits.set_pixel( i, y + height - 2, toast );
		bits.set_pixel( i, y + height - 1, black );
	}
	
	bits.set_pixel( x +         1, y +          1, black );
	bits.set_pixel( x + width - 2, y +          1, black );
	bits.set_pixel( x +         1, y + height - 2, black );
	bits.set_pixel( x + width - 2, y + height - 2, black );
	
	for ( int j = y + 2;  j < y + height - 2;  ++j )
	{
		bits.set_pixel( x,             j, black );
		bits.set_pixel( x +         1, j, toast );
		
		for ( int i = x + 2;  i < x + width - 2;  ++i )
		{
			bits.set_pixel( i, j, icing );
		}
		
		bits.set_pixel( x + width - 2, j, toast );
		bits.set_pixel( x + width - 1, j, black );
	}
	
	bits.set_pixel( x, y + height - 2, black );
	
	bits.set_pixel( x +         2, y +          2, toast );
	bits.set_pixel( x +         3, y +          2, toast );
	bits.set_pixel( x + width - 4, y +          2, toast );
	bits.set_pixel( x + width - 3, y +          2, toast );
	bits.set_pixel( x +         2, y +          3, toast );
	bits.set_pixel( x + width - 3, y +          3, toast );
	bits.set_pixel( x +         2, y + height - 4, toast );
	bits.set_pixel( x + width - 3, y + height - 4, toast );
	bits.set_pixel( x +         2, y + height - 3, toast );
	bits.set_pixel( x +         3, y + height - 3, toast );
	bits.set_pixel( x + width - 4, y + height - 3, toast );
	bits.set_pixel( x + width - 3, y + height - 3, toast );
	
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
void draw_alt_body( bitmap& bits, int x, int y )
{
	const int width  = 21;
	const int height = 18;
	
	const uint32_t ltGray = 0xFFAAAAAA;
	const uint32_t dkGray = 0xFF555555;
	
	for ( int j = y + 1;  j < y + height - 1;  ++j )
	{
		for ( int i = x + 1;  i < x + width - 1;  ++i )
		{
			const uint32_t gray = i + j - y & 1 ? ltGray : dkGray;
			
			bits.set_pixel( i, j, gray );
		}
	}
	
	for ( int i = x + 2;  i < x + width - 2;  ++i )
	{
		bits.set_pixel( i, y,              black );
		bits.set_pixel( i, y + 1,          white );
		bits.set_pixel( i, y + height - 1, black );
	}
	
	for ( int i = x + 1;  i < x + width - 1;  ++i )
	{
		bits.set_pixel( i, y + 2, white );
		bits.set_pixel( i, y + 3, black );
	}
	
	bits.set_pixel( x +         1, y +          1, black );
	bits.set_pixel( x + width - 2, y +          1, black );
	bits.set_pixel( x +         1, y + height - 2, black );
	bits.set_pixel( x + width - 2, y + height - 2, black );
	
	for ( int j = y + 2;  j < y + height - 2;  ++j )
	{
		bits.set_pixel( x,             j, black );
		bits.set_pixel( x + width - 1, j, black );
	}
	
	bits.set_pixel( x, y + height - 2, black );
}

static
void draw_head( bitmap& bits, int x, int y )
{
	const int width  = 16;
	const int height = 13;
	
	bits.set_pixel( x +  2, y, black );
	bits.set_pixel( x +  3, y, black );
	
	bits.set_pixel( x + 12, y, black );
	bits.set_pixel( x + 13, y, black );
	
	++y;
	
	bits.set_pixel( x +  1, y, black );
	bits.set_pixel( x +  2, y, furrr );
	bits.set_pixel( x +  3, y, furrr );
	bits.set_pixel( x +  4, y, black );
	
	bits.set_pixel( x + 11, y, black );
	bits.set_pixel( x + 12, y, furrr );
	bits.set_pixel( x + 13, y, furrr );
	bits.set_pixel( x + 14, y, black );
	
	++y;
	
	bits.set_pixel( x +  1, y, black );
	bits.set_pixel( x +  2, y, furrr );
	bits.set_pixel( x +  3, y, furrr );
	bits.set_pixel( x +  4, y, furrr );
	bits.set_pixel( x +  5, y, black );
	
	bits.set_pixel( x + 10, y, black );
	bits.set_pixel( x + 11, y, furrr );
	bits.set_pixel( x + 12, y, furrr );
	bits.set_pixel( x + 13, y, furrr );
	bits.set_pixel( x + 14, y, black );
	
	++y;
	
	bits.set_pixel( x +  1, y, black );
	bits.set_pixel( x +  2, y, furrr );
	bits.set_pixel( x +  3, y, furrr );
	bits.set_pixel( x +  4, y, furrr );
	bits.set_pixel( x +  5, y, furrr );
	bits.set_pixel( x +  6, y, black );
	bits.set_pixel( x +  7, y, black );
	bits.set_pixel( x +  8, y, black );
	bits.set_pixel( x +  9, y, black );
	bits.set_pixel( x + 10, y, furrr );
	bits.set_pixel( x + 11, y, furrr );
	bits.set_pixel( x + 12, y, furrr );
	bits.set_pixel( x + 13, y, furrr );
	bits.set_pixel( x + 14, y, black );
	
	++y;
	
	bits.set_pixel( x +  1, y, black );
	bits.set_pixel( x +  2, y, furrr );
	bits.set_pixel( x +  3, y, furrr );
	bits.set_pixel( x +  4, y, furrr );
	bits.set_pixel( x +  5, y, furrr );
	bits.set_pixel( x +  6, y, furrr );
	bits.set_pixel( x +  7, y, furrr );
	bits.set_pixel( x +  8, y, furrr );
	bits.set_pixel( x +  9, y, furrr );
	bits.set_pixel( x + 10, y, furrr );
	bits.set_pixel( x + 11, y, furrr );
	bits.set_pixel( x + 12, y, furrr );
	bits.set_pixel( x + 13, y, furrr );
	bits.set_pixel( x + 14, y, black );
	
	++y;
	
	bits.set_pixel( x +  0, y, black );
	bits.set_pixel( x +  1, y, furrr );
	bits.set_pixel( x +  2, y, furrr );
	bits.set_pixel( x +  3, y, furrr );
	bits.set_pixel( x +  4, y, furrr );
	bits.set_pixel( x +  5, y, furrr );
	bits.set_pixel( x +  6, y, furrr );
	bits.set_pixel( x +  7, y, furrr );
	bits.set_pixel( x +  8, y, furrr );
	bits.set_pixel( x +  9, y, furrr );
	bits.set_pixel( x + 10, y, furrr );
	bits.set_pixel( x + 11, y, furrr );
	bits.set_pixel( x + 12, y, furrr );
	bits.set_pixel( x + 13, y, furrr );
	bits.set_pixel( x + 14, y, furrr );
	bits.set_pixel( x + 15, y, black );
	
	++y;
	
	bits.set_pixel( x +  0, y, black );
	bits.set_pixel( x +  1, y, furrr );
	bits.set_pixel( x +  2, y, furrr );
	bits.set_pixel( x +  3, y, furrr );
	bits.set_pixel( x +  4, y, white );
	bits.set_pixel( x +  5, y, black );
	bits.set_pixel( x +  6, y, furrr );
	bits.set_pixel( x +  7, y, furrr );
	bits.set_pixel( x +  8, y, furrr );
	bits.set_pixel( x +  9, y, furrr );
	bits.set_pixel( x + 10, y, furrr );
	bits.set_pixel( x + 11, y, white );
	bits.set_pixel( x + 12, y, black );
	bits.set_pixel( x + 13, y, furrr );
	bits.set_pixel( x + 14, y, furrr );
	bits.set_pixel( x + 15, y, black );
	
	++y;
	
	bits.set_pixel( x +  0, y, black );
	bits.set_pixel( x +  1, y, furrr );
	bits.set_pixel( x +  2, y, furrr );
	bits.set_pixel( x +  3, y, furrr );
	bits.set_pixel( x +  4, y, black );
	bits.set_pixel( x +  5, y, black );
	bits.set_pixel( x +  6, y, furrr );
	bits.set_pixel( x +  7, y, furrr );
	bits.set_pixel( x +  8, y, furrr );
	bits.set_pixel( x +  9, y, black );
	bits.set_pixel( x + 10, y, furrr );
	bits.set_pixel( x + 11, y, black );
	bits.set_pixel( x + 12, y, black );
	bits.set_pixel( x + 13, y, furrr );
	bits.set_pixel( x + 14, y, furrr );
	bits.set_pixel( x + 15, y, black );
	
	++y;
	
	bits.set_pixel( x +  0, y, black );
	bits.set_pixel( x +  1, y, furrr );
	bits.set_pixel( x +  2, y, blush );
	bits.set_pixel( x +  3, y, blush );
	bits.set_pixel( x +  4, y, furrr );
	bits.set_pixel( x +  5, y, furrr );
	bits.set_pixel( x +  6, y, furrr );
	bits.set_pixel( x +  7, y, furrr );
	bits.set_pixel( x +  8, y, furrr );
	bits.set_pixel( x +  9, y, furrr );
	bits.set_pixel( x + 10, y, furrr );
	bits.set_pixel( x + 11, y, furrr );
	bits.set_pixel( x + 12, y, furrr );
	bits.set_pixel( x + 13, y, blush );
	bits.set_pixel( x + 14, y, blush );
	bits.set_pixel( x + 15, y, black );
	
	++y;
	
	bits.set_pixel( x +  0, y, black );
	bits.set_pixel( x +  1, y, furrr );
	bits.set_pixel( x +  2, y, blush );
	bits.set_pixel( x +  3, y, blush );
	bits.set_pixel( x +  4, y, furrr );
	bits.set_pixel( x +  5, y, black );
	bits.set_pixel( x +  6, y, furrr );
	bits.set_pixel( x +  7, y, furrr );
	bits.set_pixel( x +  8, y, black );
	bits.set_pixel( x +  9, y, furrr );
	bits.set_pixel( x + 10, y, furrr );
	bits.set_pixel( x + 11, y, black );
	bits.set_pixel( x + 12, y, furrr );
	bits.set_pixel( x + 13, y, blush );
	bits.set_pixel( x + 14, y, blush );
	bits.set_pixel( x + 15, y, black );
	
	++y;
	
	bits.set_pixel( x +  1, y, black );
	bits.set_pixel( x +  2, y, furrr );
	bits.set_pixel( x +  3, y, furrr );
	bits.set_pixel( x +  4, y, furrr );
	bits.set_pixel( x +  5, y, black );
	bits.set_pixel( x +  6, y, black );
	bits.set_pixel( x +  7, y, black );
	bits.set_pixel( x +  8, y, black );
	bits.set_pixel( x +  9, y, black );
	bits.set_pixel( x + 10, y, black );
	bits.set_pixel( x + 11, y, black );
	bits.set_pixel( x + 12, y, furrr );
	bits.set_pixel( x + 13, y, furrr );
	bits.set_pixel( x + 14, y, black );
	
	++y;
	
	bits.set_pixel( x +  2, y, black );
	bits.set_pixel( x +  3, y, furrr );
	bits.set_pixel( x +  4, y, furrr );
	bits.set_pixel( x +  5, y, furrr );
	bits.set_pixel( x +  6, y, furrr );
	bits.set_pixel( x +  7, y, furrr );
	bits.set_pixel( x +  8, y, furrr );
	bits.set_pixel( x +  9, y, furrr );
	bits.set_pixel( x + 10, y, furrr );
	bits.set_pixel( x + 11, y, furrr );
	bits.set_pixel( x + 12, y, furrr );
	bits.set_pixel( x + 13, y, black );
	
	++y;
	
	bits.set_pixel( x +  3, y, black );
	bits.set_pixel( x +  4, y, black );
	bits.set_pixel( x +  5, y, black );
	bits.set_pixel( x +  6, y, black );
	bits.set_pixel( x +  7, y, black );
	bits.set_pixel( x +  8, y, black );
	bits.set_pixel( x +  9, y, black );
	bits.set_pixel( x + 10, y, black );
	bits.set_pixel( x + 11, y, black );
	bits.set_pixel( x + 12, y, black );
}

static int face_boundary[] = { 6, 6, 5, 5, 5, 5, 7, 7, 7, 7 };

static
void draw_alt_head( bitmap& bits, int x, int y )
{
	const int width  = 16;
	const int height = 13;
	
	const uint32_t back_face = 0xFF5544CC;
	const uint32_t fore_face = 0xFFCCCCFF;
	
	++y;
	
	for ( int i = 0;  i < 15;  ++i )
	{
		bits.set_pixel( x + i, y,      black );
		bits.set_pixel( x + i, y + 11, black );
	}
	
	for ( int j = 1;  j < 11;  ++j )
	{
		const int bound = face_boundary[ j - 1 ];
		
		bits.set_pixel( x, y + j, black );
		
		int i = 1;
		
		for ( ;  i < bound;  ++i )
		{
			bits.set_pixel( x + i, y + j, back_face );
		}
		
		bits.set_pixel( x + i, y + j, black );
		
		++i;
		
		for ( ;  i < 14;  ++i )
		{
			bits.set_pixel( x + i, y + j, fore_face );
		}
		
		bits.set_pixel( x + i, y + j, black );
	}
	
	bits.set_pixel( x + 3, y + 2, black );
	bits.set_pixel( x + 3, y + 3, black );
	bits.set_pixel( x + 9, y + 2, black );
	bits.set_pixel( x + 9, y + 3, black );
	
	bits.set_pixel( x + 6, y + 6, black );
	bits.set_pixel( x + 7, y + 6, black );
	bits.set_pixel( x + 8, y + 6, black );
	
	bits.set_pixel( x +  3, y + 8, black );
	bits.set_pixel( x +  4, y + 8, black );
	bits.set_pixel( x +  5, y + 9, black );
	bits.set_pixel( x +  6, y + 9, black );
	bits.set_pixel( x +  7, y + 9, black );
	bits.set_pixel( x +  8, y + 9, black );
	bits.set_pixel( x +  9, y + 9, black );
	bits.set_pixel( x + 10, y + 8, black );
	bits.set_pixel( x + 11, y + 8, black );
	
	bits.set_pixel( x + 9, y -  2, black );
	bits.set_pixel( x + 8, y -  1, black );
	bits.set_pixel( x + 8, y + 13, black );
	bits.set_pixel( x + 9, y + 14, black );
}

static unsigned t = 0;

void draw_frame( bitmap& bits, int dt )
{
	t += dt;
	
	t %= 12;
	
	uint32_t* dst = bits.begin();
	uint32_t* end = bits.end();
	
	const uint32_t dusk = 0xFF224477;
	
	fill( dst, end, dusk );
	
	const bool step = t & 0x2;
	
	draw_waves( bits, 0, 26, step );
	
	draw_fireworks( bits, t );
	
	const int body_base_x = 25;
	const int body_base_y = 25;
	
	const int head_base_x = body_base_x + 10 + alt_head_x;
	const int head_base_y = body_base_y +  5 + alt_head_y;
	
	const int body_y = body_base_y + (t/2 % 3 > 0);
	const int head_x = head_base_x + (t + 2) / 3 % 2;
	const int head_y = head_base_y + (t + 1) / 3 % 2;
	
	const int tail_x = body_base_x;
	const int tail_y = body_y + 11;
	
	const int paws_x = body_base_x;
	const int paws_y = body_y + 18;
	
	draw_paws( bits, paws_x,      paws_y, t % 6 );
	draw_tail( bits, tail_x,      tail_y, t % 6 );
	(alt_body ? draw_alt_body : draw_body)( bits, body_base_x, body_y );
	(alt_head ? draw_alt_head : draw_head)( bits, head_x,      head_y );
}
