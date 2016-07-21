/*
	string_iterator.hh
	------------------
*/

#ifndef VLIB_ITERATORS_STRINGITERATOR_HH
#define VLIB_ITERATORS_STRINGITERATOR_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	class string_iterator
	{
		private:
			typedef const struct unspecified* boolean;
			
			Value its_string;
			
			const char* begin;
			const char* end;
			
			mutable Value its_value;
		
		public:
			string_iterator( const Value& s );
			
			bool finished() const  { return begin >= end; }
			
			const Value& get() const;
			
			void step()  { ++begin; }
			
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
			
			string_iterator& operator++()  { step(); return *this; }
	};
	
}

#endif
