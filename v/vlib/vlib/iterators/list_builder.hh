/*
	list_builder.hh
	---------------
*/

#ifndef VLIB_ITERATORS_LISTBUILDER_HH
#define VLIB_ITERATORS_LISTBUILDER_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	class list_builder
	{
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
