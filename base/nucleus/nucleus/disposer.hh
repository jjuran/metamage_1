/*
	nucleus/disposer.hh
	-------------------
*/


#ifndef NUCLEUS_DISPOSER_HH
#define NUCLEUS_DISPOSER_HH


namespace nucleus
{
	
	template < class Resource > struct disposer;
	
	template < class Resource >
	struct disposer_class
	{
		typedef disposer< Resource > type;
	};
	
}

#endif

