/*
	range_iterator.hh
	-----------------
*/

#ifndef VLIB_ITERATORS_RANGEITERATOR_HH
#define VLIB_ITERATORS_RANGEITERATOR_HH

// bignum
#include "bignum/integer.hh"

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	class range_iterator
	{
		private:
			typedef const struct unspecified* boolean;
			
			bignum::integer its_next;
			bignum::integer its_high;
			
			mutable Value its_value;
		
		public:
			range_iterator( const Value& range );
			
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
			
			range_iterator& operator++()  { step(); return *this; }
	};
	
}

#endif
