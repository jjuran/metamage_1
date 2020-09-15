/*
	iterator_base.hh
	----------------
*/

#ifndef VLIB_ITERATORS_ITERATORBASE_HH
#define VLIB_ITERATORS_ITERATORBASE_HH


namespace vlib
{
	
	class Value;
	
	class iterator_base
	{
		public:
			virtual ~iterator_base()
			{
			}
			
			virtual bool finished() const = 0;
			
			virtual const Value& get() const = 0;
			
			virtual void step() = 0;
	};
	
}

#endif
