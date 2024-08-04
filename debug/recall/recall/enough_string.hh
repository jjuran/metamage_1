/*
	enough_string.hh
	----------------
*/

#ifndef RECALL_ENOUGHSTRING_HH
#define RECALL_ENOUGHSTRING_HH


namespace recall
{
	
	class enough_string
	{
		private:
			char*    its_buffer;
			char*    its_marker;
			char*    its_limit;
		
		public:
			enough_string( char* p, unsigned n )
			:
				its_buffer( p ),
				its_marker( p ),
				its_limit ( p + n )
			{
			}
			
			char operator[]( long i ) const  { return data()[ i ]; }
			
			const char* data() const  { return its_buffer; }
			
			unsigned size() const  { return its_marker - its_buffer; }
			
			bool empty() const  { return size() == 0; }
			
			void operator=( const char* s );
			
			template < class String >
			void operator=( const String& s )
			{
				assign( s.data(), s.size() );
			}
			
			void operator+=( const char* s );
			
			template < class String >
			void operator+=( const String& s )
			{
				append( s.data(), s.size() );
			}
			
			void assign( const char* p, unsigned n );
			
			void append( const char* p, unsigned n );
			
			void append( const char* p, const char* q )
			{
				append( p, q - p );
			}
	};
	
}

#endif
