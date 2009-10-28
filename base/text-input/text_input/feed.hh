/*
	text_input/feed.hh
	------------------
	
	Copyright 2009, Joshua Juran
*/

#ifndef TEXTINPUT_FEED_HH
#define TEXTINPUT_FEED_HH

// Standard C++
#include <string>


namespace text_input
{
	
	class feed
	{
		public:
			static const std::size_t buffer_length = 4096;
			
			class buffer_occupied {};
			class buffer_overrun  {};
		
		private:
			char its_buffer[ buffer_length ];
			
			std::size_t its_data_length;
			std::size_t its_mark;
			
			std::string its_next_line;
			
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
			
			bool buffer_empty();
			
			bool empty();
			
			bool has_complete_line();
			
			const std::string* get_line();
			const std::string& get_line_ref();
			
			const std::string* get_fragment();
			const std::string& get_fragment_ref();
			
			char* buffer();
			
			void accept_input( std::size_t length );
			
			void accept_input( const char* buffer, std::size_t length );
	};
	
}

#endif

