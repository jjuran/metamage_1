/*
	iterator_template.hh
	--------------------
*/

#ifndef VLIB_ITERATORS_ITERATORTEMPLATE_HH
#define VLIB_ITERATORS_ITERATORTEMPLATE_HH

// vlib
#include "vlib/iterators/iterator_base.hh"


namespace vlib
{
	
	template < class T >
	class iterator_template : public iterator_base
	{
		private:
			T its_impl;
		
		public:
			iterator_template( const Value& con ) : its_impl( con )
			{
			}
			
			bool finished() const  { return its_impl.finished(); }
			
			const Value& get() const  { return its_impl.get(); }
			
			void step()  { its_impl.step(); }
	};
	
}

#endif
