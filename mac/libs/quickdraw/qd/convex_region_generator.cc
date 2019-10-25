/*
	convex_region_generator.cc
	--------------------------
*/

#include "qd/convex_region_generator.hh"

// quickdraw
#include "qd/region_detail.hh"


namespace quickdraw
{
	
	convex_region_generator::convex_region_generator( short* out )
	:
		its_mark( out )
	{
	}
	
	void convex_region_generator::start( short v, short left, short right )
	{
		*its_mark++ = v;
		*its_mark++ = left;
		*its_mark++ = right;
		*its_mark++ = Region_end;
		
		its_prev_left  = left;
		its_prev_right = right;
	}
	
	void convex_region_generator::extend_left( short v, short left )
	{
		*its_mark++ = v;
		
		*its_mark++ = left;
		*its_mark++ = its_prev_left;
		*its_mark++ = Region_end;
		
		its_prev_left = left;
	}
	
	void convex_region_generator::extend_right( short v, short right )
	{
		*its_mark++ = v;
		
		*its_mark++ = its_prev_right;
		*its_mark++ = right;
		*its_mark++ = Region_end;
		
		its_prev_right = right;
	}
	
	void convex_region_generator::condense_left( short v, short right )
	{
		*its_mark++ = v;
		
		*its_mark++ = right;
		*its_mark++ = its_prev_right;
		*its_mark++ = Region_end;
		
		its_prev_right = right;
	}
	
	void convex_region_generator::condense_right( short v, short left )
	{
		*its_mark++ = v;
		
		*its_mark++ = its_prev_left;
		*its_mark++ = left;
		*its_mark++ = Region_end;
		
		its_prev_left = left;
	}
	
	void convex_region_generator::tack_left( short v, short left, short right )
	{
		*its_mark++ = v;
		
		*its_mark++ = left;
		
		if ( right != its_prev_left )
		{
			*its_mark++ = its_prev_left;
			*its_mark++ = right;
		}
		
		*its_mark++ = its_prev_right;
		*its_mark++ = Region_end;
		
		its_prev_left  = left;
		its_prev_right = right;
	}
	
	void convex_region_generator::tack_right( short v, short left, short right )
	{
		*its_mark++ = v;
		
		*its_mark++ = its_prev_left;
		
		if ( left != its_prev_right )
		{
			*its_mark++ = left;
			*its_mark++ = its_prev_right;
		}
		
		*its_mark++ = right;
		*its_mark++ = Region_end;
		
		its_prev_left  = left;
		its_prev_right = right;
	}
	
	void convex_region_generator::move_left( short v, short left, short right )
	{
		const int state = (left == its_prev_left) * 2
		                | (right == its_prev_right);
		
		switch ( state )
		{
			case 0:  tack_left    ( v, left, right );  break;
			case 1:  extend_left  ( v, left        );  break;
			case 2:  condense_left( v,       right );  break;
			
			default:
				break;
		}
	}
	
	void convex_region_generator::move_right( short v, short left, short right )
	{
		const int state = (left == its_prev_left) * 2
		                | (right == its_prev_right);
		
		switch ( state )
		{
			case 0:  tack_right    ( v, left, right );  break;
			case 1:  condense_right( v, left        );  break;
			case 2:  extend_right  ( v,       right );  break;
			
			default:
				break;
		}
	}
	
	void convex_region_generator::expand( short v, short left, short right )
	{
		*its_mark++ = v;
		
		*its_mark++ = left;
		*its_mark++ = its_prev_left;
		*its_mark++ = its_prev_right;
		*its_mark++ = right;
		*its_mark++ = Region_end;
		
		its_prev_left  = left;
		its_prev_right = right;
	}
	
	void convex_region_generator::contract( short v, short left, short right )
	{
		*its_mark++ = v;
		
		*its_mark++ = its_prev_left;
		*its_mark++ = left;
		*its_mark++ = right;
		*its_mark++ = its_prev_right;
		*its_mark++ = Region_end;
		
		its_prev_left  = left;
		its_prev_right = right;
	}
	
	short* convex_region_generator::finish( short v )
	{
		*its_mark++ = v;
		*its_mark++ = its_prev_left;
		*its_mark++ = its_prev_right;
		*its_mark++ = Region_end;
		*its_mark++ = Region_end;
		
		return its_mark;
	}
	
	short* convex_region_generator::finish_reflected( short v, short* begin )
	{
		const short top    = *begin;
		const short bottom = v;
		const short height = bottom - top;
		
		const short* first_sentinel = begin + 3;
		
		const short* sentinel = its_mark - 1;
		
		const short* q = its_mark;
		const short* p = q - 1;
		
		while ( sentinel > first_sentinel )
		{
			const short* q = sentinel;
			const short* p = sentinel - 3;  // v, h0, and h1 at minumum
			
			while ( *--p != Region_end )
			{
				continue;
			}
			
			sentinel = p++;
			
			v = bottom - *p++ + top;
			
			*its_mark++ = v;
			
			do
			{
				*its_mark++ = *p++;
			}
			while ( p <= q );
		}
		
		++begin;
		
		*its_mark++ = bottom;
		*its_mark++ = *begin++;
		*its_mark++ = *begin++;
		*its_mark++ = Region_end;
		
		*its_mark++ = Region_end;
		
		return its_mark;
	}
	
}
