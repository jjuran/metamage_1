/*
	safe_list_builder.hh
	--------------------
*/

#ifndef VLIB_ITERATORS_SAFELISTBUILDER_HH
#define VLIB_ITERATORS_SAFELISTBUILDER_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	class safe_list_builder
	{
		private:
			Value  its_list;
		
		public:
			safe_list_builder();
			
			void append( const Value& v );
			
			Value get() const;
			
			operator Value() const  { return get(); }
	};
	
}

#endif
