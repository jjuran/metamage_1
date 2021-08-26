/*
	DrawGameFromURLIntoContext.cc
	-----------------------------
*/

#include "DrawGameFromURLIntoContext.hh"

// tictactoe-logic
#include "tictactoe.hh"

// CGTictactoe
#include "CGTictactoe.hh"


static
bool load_game( const char* path, signed char board[] )
{
	int game_fd = open( path, O_RDONLY );
	
	if ( game_fd < 0 )
	{
		return false;
	}
	
	uint8_t block[ 4096 ];
	
	size_t n = read( game_fd, block, sizeof block );
	
	close( game_fd );
	
	return n <= 9  &&  tictactoe::restore( block, n, board );
}

static
void DrawGameIntoContext( signed char board[], CGContextRef c, CGFloat height )
{
	/*
		V-flip the coordinate system and scale it to 32x32.
	*/
	
	CGFloat scale = height / 32;
	
	CGContextTranslateCTM( c, 0, height );
	
	CGContextScaleCTM( c, scale, -scale );
	
	draw_board( c, board );
}

void DrawGameFromURLIntoContext( CFURLRef url, CGContextRef c, CGFloat height )
{
	const CFStringEncoding encoding = kCFStringEncodingUTF8;
	
	CFStringRef string = CFURLCopyFileSystemPath( url, kCFURLPOSIXPathStyle );
	
	const char* path = CFStringGetCStringPtr( string, encoding );
	
	char buffer[ PATH_MAX ];
	
	if ( path == NULL )
	{
		if ( ! CFStringGetCString( string, buffer, sizeof buffer, encoding ) )
		{
			return;
		}
		
		path = buffer;
	}
	
	signed char board[ tictactoe::n_squares ] = {};
	
	if ( load_game( path, board ) )
	{
		DrawGameIntoContext( board, c, height );
	}
	
	CFRelease( string );
}
