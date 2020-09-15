/*
	xor_region.cc
	-------------
*/

#include "qd/xor_region.hh"

// quickdraw
#include "qd/region_detail.hh"


#ifndef NULL
#define NULL  0
#endif


namespace quickdraw
{
	
	static void copy_region_line( const short*& in, short*& out )
	{
		short word;
		
		do
		{
			word = *in++;
			
			*out++ = word;
		}
		while ( word != Region_end );
	}
	
	void xor_region( const short*  a,
	                 const short*  b,
	                 short*        r )
	{
		while ( true )
		{
			short va = *a;
			short vb = *b;
		
			if ( precedes_in_region( va, vb ) )
			{
				copy_region_line( a, r );
			}
			else if ( precedes_in_region( vb, va ) )
			{
				copy_region_line( b, r );
			}
			else if ( va == Region_end )
			{
				*r++ = Region_end;
				break;
			}
			else
			{
				*r++ = va;
				
				short* old_r = r;
				
				++a;
				++b;
				
				while ( true )
				{
					if ( precedes_in_region( *a, *b ) )
					{
						*r++ = *a++;
					}
					else if ( precedes_in_region( *b, *a ) )
					{
						*r++ = *b++;
					}
					else
					{
						short ha = *a++;
						short hb = *b++;
						
						if ( hb == Region_end )
						{
							if ( r == old_r )
							{
								--r;
							}
							else
							{
								*r++ = Region_end;
							}
							
							break;
						}
					}
				}
			}
		}
	}
	
}
