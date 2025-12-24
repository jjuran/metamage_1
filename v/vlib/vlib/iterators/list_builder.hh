/*
	list_builder.hh
	---------------
*/

#ifndef VLIB_ITERATORS_LISTBUILDER_HH
#define VLIB_ITERATORS_LISTBUILDER_HH

// iota
#include "iota/class.hh"

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	class list_builder
	{
		NON_COPYABLE( list_builder )
		NO_NEW_DELETE
		
		private:
			Value   its_list;
			Value*  its_mark;
		
		public:
			list_builder();
			
			void append( const Value& v );
			
			Value move();
			
			const Value& get() const  { return its_list; }
			
			operator const Value&() const  { return get(); }
	};
	
}

#endif
