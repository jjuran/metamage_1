/*
	full_iterator.hh
	----------------
*/

#ifndef VLIB_ITERATORS_FULLITERATOR_HH
#define VLIB_ITERATORS_FULLITERATOR_HH

// iota
#include "iota/class.hh"

// vxo
#include "vxo/ptrvec.hh"


namespace vlib
{

	class Value;
	
	class full_iterator
	{
		NO_NEW_DELETE
		
		private:
			vxo::UPtrVec< const Value > its_stack;
		
		public:
			full_iterator( const Value& start );
			
			bool finished() const  { return its_stack.empty(); }
			
			const Value& get() const;
			
			void step();
			
			const Value& operator*() const  { return get(); }
			
			full_iterator& operator++()  { step(); return *this; }
			
			void prune();
	};
	
}

#endif
