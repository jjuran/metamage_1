/*
	text_input/get_line_from_feed.hh
	--------------------------------
	
	Copyright 2009, Joshua Juran
*/

#ifndef TEXTINPUT_GETLINEFROMFEED_HH
#define TEXTINPUT_GETLINEFROMFEED_HH

// text-input
#include "text_input/feed.hh"


namespace text_input
{
	
	template < class Reader >
	const plus::string* get_line_from_feed( text_input::feed&  feed,
	                                        Reader             read )
	{
		typedef plus::string::size_type size_type;
		
		while ( true )
		{
			if ( const plus::string* result = feed.get_line() )
			{
				return result;
			}
			
			const size_type length = feed.buffer_length;
			
			const size_type n_read = read( feed.buffer(), length );
			
			if ( n_read == 0 )
			{
				// end of file
				return feed.get_fragment();
			}
			
			feed.accept_input( n_read );
		}
	}
	
}

#endif

