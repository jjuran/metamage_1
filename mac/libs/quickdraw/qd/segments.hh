/*
	segments.hh
	-----------
*/

#ifndef QUICKDRAW_SEGMENTS_HH
#define QUICKDRAW_SEGMENTS_HH

// Standard C
#include <stddef.h>


namespace quickdraw
{
	
	class segments_box
	{
		private:
			short*  its_data;
			size_t  its_size;  // words
			
			// non-copyable
			segments_box           ( const segments_box& );
			segments_box& operator=( const segments_box& );
		
		public:
			typedef short*        iterator;
			typedef short const*  const_iterator;
			
			explicit segments_box( size_t capacity );  // bytes
			~segments_box();
			
			size_t size() const  { return its_size; }
			
			bool empty() const  { return size() == 0; }
			
			short const* data() const  { return its_data; }
			
			short const* begin() const  { return data(); }
			short const* end  () const  { return data() + size(); }
			
			short* begin()  { return its_data; }
			short* end  ()  { return its_data + size(); }
			
			void clear()  { its_size = 0; }
			
			void erase( short* it );
			
			void insert( short* it, short h );
			
			void push_back( short h );
			
			void toggle( short h );
			
			void swap( segments_box& that );
			
			segments_box& operator^=( short h )
			{
				toggle( h );
				return *this;
			}
	};
	
	inline
	void xor_segments( segments_box& segments, short coord )
	{
		segments ^= coord;
	}
	
	void xor_segments( segments_box&        a,
	                   segments_box const&  b );
	
	void accumulate_row( short*& r, short v, const segments_box& segments );
	
	typedef segments_box malloc_segments_box;
	
}

#endif
