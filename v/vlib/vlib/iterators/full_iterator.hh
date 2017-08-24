/*
	full_iterator.hh
	----------------
*/

#ifndef VLIB_ITERATORS_FULLITERATOR_HH
#define VLIB_ITERATORS_FULLITERATOR_HH

// Standard C++
#include <vector>


namespace vlib
{
	
	class Symbol;
	class Value;
	
	class full_iterator
	{
		private:
			std::vector< const Value* > its_stack;
		
		public:
			full_iterator( const Value& start );
			
			bool finished() const  { return its_stack.empty(); }
			
			const Value& get() const;
			
			void step();
			
			const Value& operator*() const  { return get(); }
			
			full_iterator& operator++()  { step(); return *this; }
			
			void prune();
	};
	
	Symbol* next_symbol( full_iterator& it );
	
}

#endif
