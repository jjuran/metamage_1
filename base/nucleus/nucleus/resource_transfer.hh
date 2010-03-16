// nucleus/resource_transfer.hh
// ----------------------------

// Part of the Nitrogen project.
//
// Written 2002 by Lisa Lippincott.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NUCLEUS_RESOURCETRANSFER_HH
#define NUCLEUS_RESOURCETRANSFER_HH

namespace nucleus
{
	
	template < class Transferred >
	class resource_transfer
	{
		private:
			Transferred& transferred;
		
		public:
			explicit resource_transfer( Transferred& t )
			:
				transferred( t )
			{
			}
			
			Transferred& operator*() const   { return transferred;  }
			Transferred *operator->() const  { return &transferred; }
	};
	
}

#endif

