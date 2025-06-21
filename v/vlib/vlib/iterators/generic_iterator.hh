/*
	generic_iterator.hh
	-------------------
*/

#ifndef VLIB_ITERATORS_GENERICITERATOR_HH
#define VLIB_ITERATORS_GENERICITERATOR_HH

// iota
#include "iota/class.hh"

// vlib
#include "vlib/value.hh"
#include "vlib/iterators/iterator_base.hh"


namespace vlib
{
	
	class generic_iterator
	{
		NON_COPYABLE( generic_iterator )
		NO_NEW_DELETE
		
		private:
			typedef const struct unspecified* boolean;
			
			iterator_base* its_impl;
		
		public:
			generic_iterator( const Value& container );
			
			~generic_iterator();
			
			bool finished() const  { return its_impl->finished(); }
			
			const Value& get() const  { return its_impl->get(); }
			
			void step()  { its_impl->step(); }
			
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
	};
	
}

#endif
