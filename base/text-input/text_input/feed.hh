/*
	text_input/feed.hh
	------------------
*/

#ifndef TEXTINPUT_FEED_HH
#define TEXTINPUT_FEED_HH

// plus
#include "plus/var_string.hh"


namespace text_input
{
	
	class feed
	{
		public:
			typedef plus::string::size_type size_type;
			
			static const size_type buffer_length = 4096;
			
			class buffer_occupied {};
			class buffer_overrun  {};
		
		private:
			char its_buffer[ buffer_length ];
			
			size_type its_data_length;
			size_type its_mark;
			
			plus::var_string its_next_line;
			
			bool it_was_returned;
			bool its_last_end_was_CR;
		
		private:
			void advance_CRLF();
			
			void prime();
		
		public:
			feed()
			:
				its_data_length(),
				its_mark(),
				it_was_returned(),
				its_last_end_was_CR()
			{
			}
			
			const plus::string* get_line();
			
			const plus::string* get_fragment();
			const plus::string& get_fragment_ref();
			
			char* buffer();
			
			void accept_input( size_type length );
			
			void accept_input( const char* buffer, size_type length );
	};
	
}

#endif

