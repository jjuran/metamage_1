/*
	byterange_iterator.hh
	---------------------
*/

#ifndef VLIB_ITERATORS_BYTERANGEITERATOR_HH
#define VLIB_ITERATORS_BYTERANGEITERATOR_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	class byterange_iterator
	{
		private:
			typedef const struct unspecified* boolean;
			
			int16_t its_next;
			int16_t its_high;
			
			mutable Value its_value;
		
		public:
			byterange_iterator( const Value& range );
			
			bool finished() const  { return its_next > its_high; }
			
			const Value& get() const;
			
			void step()  { ++its_next; }
			
			bool next()
			{
				if ( finished() )  return false;
				step();
				return true;
			}
			
			const Value& use()
			{
				const Value& result = get();
				step();
				return result;
			}
			
			operator boolean() const  { return (boolean) ! finished(); }
			
			const Value& operator*() const  { return get(); }
			const Value* operator->() const  { return &get(); }
			
			byterange_iterator& operator++()  { step(); return *this; }
	};
	
}

#endif
