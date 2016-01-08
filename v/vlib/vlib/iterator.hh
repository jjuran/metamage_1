/*
	iterator.hh
	-----------
*/

#ifndef VLIB_ITERATOR_HH
#define VLIB_ITERATOR_HH

// Standard C++
#include <vector>

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	class full_iterator
	{
		private:
			std::vector< Value > its_stack;
		
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
