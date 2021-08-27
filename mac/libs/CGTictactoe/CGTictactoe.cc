/*
	CGTictactoe.cc
	--------------
*/

#include "CGTictactoe.hh"

static
void draw_X( CGContextRef context )
{
	const float offset = 0.35;
	
	const float min = 0 + offset;
	const float max = 6 - offset;
	
	CGContextMoveToPoint   ( context, min, min );
	CGContextAddLineToPoint( context, max, max );
	
	CGContextMoveToPoint   ( context, min, max );
	CGContextAddLineToPoint( context, max, min );
	
	CGContextClosePath( context );
	CGContextStrokePath( context );
}

static
void draw_O( CGContextRef context )
{
#ifdef MAC_OS_X_VERSION_10_4
	
	CGContextStrokeEllipseInRect( context, CGRectMake( 0.5, 0.5, 5, 5 ) );
	
#endif
}

static
void draw_grid_lines( CGContextRef context )
{
	CGContextFillRect( context, CGRectMake( 3, 11, 26, 1 ) );
	CGContextFillRect( context, CGRectMake( 3, 20, 26, 1 ) );
	
	CGContextFillRect( context, CGRectMake( 11, 3, 1, 26 ) );
	CGContextFillRect( context, CGRectMake( 20, 3, 1, 26 ) );
}

void draw_token( CGContextRef context, int token, int x, int y )
{
	CGContextSaveGState( context );
	
	CGContextTranslateCTM( context, 4 + 9 * x, 4 + 9 * y );
	
	if ( token > 0 )
	{
		draw_X( context );
	}
	else
	{
		draw_O( context );
	}
	
	CGContextRestoreGState( context );
}

void draw_board( CGContextRef context, signed char board[ 9 ] )
{
	draw_grid_lines( context );
	
	int i = 0;
	
	for ( int row = 0;  row < 3;  ++row )
	{
		for ( int col = 0;  col < 3;  ++col )
		{
			if ( signed char token = board[ i++ ] )
			{
				draw_token( context, token, col, row );
			}
		}
	}
}
