/*
	resize.hh
	---------
*/

#ifndef AMICUS_RESIZE_HH
#define AMICUS_RESIZE_HH


namespace amicus
{

template < class Number >
Number get_proportional_coordinate( Number outer, Number inner, Number offset )
{
	Number delta = outer - inner;
	
	if ( offset <= 0  ||  delta <= 0 )
	{
		return 0;
	}
	
	if ( offset >= delta )
	{
		return outer;
	}
	
	return offset * outer / delta;
}

template < class Number >
Number get_proportional_offset( Number outer, Number inner, Number coordinate )
{
	Number delta = outer - inner;
	
	if ( delta <= 0 )
	{
		return 0;
	}
	
	return coordinate * delta / outer;
}

}

#endif
