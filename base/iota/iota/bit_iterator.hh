/*
	bit_iterator.hh
	---------------
*/

#ifndef IOTA_BITITERATOR_HH
#define IOTA_BITITERATOR_HH


namespace iota
{
	
	class bit_iterator
	{
		private:
			const char*    its_addr;
			unsigned char  its_mask;
		
		public:
			bit_iterator()
			{
				// uninitialized
			}
			
			bit_iterator( const void* ptr )
			:
				its_addr( (const char*) ptr ),
				its_mask( 128 )
			{
			}
			
			bool operator*() const
			{
				return *its_addr & its_mask;
			}
			
			bit_iterator& operator++()
			{
				if ( (its_mask >>= 1) == 0 )
				{
					its_mask = 128;
					++its_addr;
				}
				
				return *this;
			}
			
			bit_iterator operator++( int )
			{
				bit_iterator result = *this;
				
				++*this;
				
				return result;
			}
			
			bit_iterator& operator--()
			{
				if ( (its_mask <<= 1) == 0 )
				{
					its_mask = 1;
					--its_addr;
				}
				
				return *this;
			}
			
			bit_iterator operator--( int )
			{
				bit_iterator result = *this;
				
				--*this;
				
				return result;
			}
			
			friend
			bool operator==( const bit_iterator& a, const bit_iterator& b )
			{
				return a.its_addr == b.its_addr  &&  a.its_mask == b.its_mask;
			}
			
			friend
			bool operator<( const bit_iterator& a, const bit_iterator& b )
			{
				// More significant bits, which come first, have greater masks.
				
				return a.its_addr <  b.its_addr  ||
				      (a.its_addr == b.its_addr  &&  a.its_mask > b.its_mask);
			}
	};
	
	inline
	bool operator!=( const bit_iterator& a, const bit_iterator& b )
	{
		return !(a == b);
	}
	
	inline
	bool operator>( const bit_iterator& a, const bit_iterator& b )
	{
		return b < a;
	}
	
	inline
	bool operator<=( const bit_iterator& a, const bit_iterator& b )
	{
		return !(b < a);
	}
	
	inline
	bool operator>=( const bit_iterator& a, const bit_iterator& b )
	{
		return !(a < b);
	}
	
}

#endif
