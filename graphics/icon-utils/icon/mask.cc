/*
	mask.cc
	-------
*/

#include "icon/mask.hh"

// Standard C
#include <string.h>

// iota
#include "iota/endian.hh"

// icon-utils
#include "icon/icon.hh"


#pragma exceptions off


namespace icon
{
	
	static
	void set_if( icon32_data&        target,
	             const icon32_data&  mask,
	             int                 x,
	             int                 y )
	{
		if ( test( mask, x, y ) )
		{
			set( target, x, y );
		}
	}
	
	bool derive_mask( const icon32_data& icon, icon32_data& mask )
	{
		memset( &mask, 0xFF, sizeof mask );
		
		int i;
		
		// Bottom
		
		for ( i = 31;  i >= 0;  --i )
		{
			if ( icon.u32[ i ] != 0 )
			{
				break;
			}
			
			mask.u32[ i ] = 0;
		}
		
		if ( i < 0 )
		{
			return false;  // icon is entirely clear
		}
		
		int max = i;
		
		// Top
		
		/*
			We don't check the last row, because if it's blank then we handled
			it above.
		*/
		
		for ( i = 0;  i < 31;  ++i )
		{
			if ( icon.u32[ i ] != 0 )
			{
				break;
			}
			
			mask.u32[ i ] = 0;
		}
		
		int min = i;
		
		icon32_data body = mask;   // frontier body
		icon32_data edge = { 0 };  // frontier edge
		
		edge.u32[ min ] = 0xFFFFFFFF;
		edge.u32[ max ] = 0xFFFFFFFF;
		
		for ( i = min + 1;  i < max;  ++i )
		{
			edge.u32[ i ] |= iota::big_u32( 0x80000001 );
		}
		
		bool hit;
		
		do
		{
			hit = false;
			
			for ( i = min;  i <= max;  ++i )
			{
				for ( int j = 31;  j >= 0;  --j )
				{
					int x = j;
					const int y = i;
					
					if ( test( edge, x, y ) )
					{
						if ( ! test( icon, x, y ) )
						{
							clear( mask, x, y );
							
							if ( i - 1 >= min )
							{
								const int y = i - 1;
								
								set_if( edge, body, x, y );
							}
							
							if ( i + 1 <= max )
							{
								const int y = i + 1;
								
								set_if( edge, body, x, y );
							}
							
							x = j - 1;
							
							if ( x >= 0 )
							{
								set_if( edge, body, x, y );
							}
							
							x = j + 1;
							
							if ( x < 32 )
							{
								set_if( edge, body, x, y );
							}
							
							hit = true;
						}
						
						clear( edge, j, i );
						clear( body, j, i );
					}
				}
			}
		}
		while ( hit );
		
		return true;
	}
	
}
