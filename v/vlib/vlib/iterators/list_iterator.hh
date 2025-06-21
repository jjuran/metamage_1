/*
	list_iterator.hh
	----------------
*/

#ifndef VLIB_ITERATORS_LISTITERATOR_HH
#define VLIB_ITERATORS_LISTITERATOR_HH

// iota
#include "iota/class.hh"

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	class list_iterator
	{
		NO_NEW_DELETE
		
		private:
			typedef const struct unspecified* boolean;
			
			Value         its_list;
			Value const*  its_mark;
		
		public:
			list_iterator( const Value& list );
			
			bool finished() const  { return is_empty_list( *its_mark ); }
			
			const Value& get() const;
			
			const Value& rest() const  { return *its_mark; }
			
			void step();
			
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
			
			list_iterator& operator++()  { step(); return *this; }
	};
	
}

#endif
