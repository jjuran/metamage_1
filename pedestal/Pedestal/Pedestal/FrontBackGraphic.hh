/*	===================
 *	FrontBackGraphic.hh
 *	===================
 */

#ifndef PEDESTAL_FRONTBACKGRAPHIC_HH
#define PEDESTAL_FRONTBACKGRAPHIC_HH

// Standard C++
#include <utility>

// Pedestal
#include "Pedestal/Graphic.hh"


namespace Pedestal
{
	
	template < class Front, class Back >
	class FrontBackGraphic : public Graphic
	{
		private:
			Front front;
			Back  back;
		
		public:
			typedef std::pair< typename Front::Initializer,
			                   typename Back ::Initializer > Initializer;
			
			FrontBackGraphic( Initializer init = Initializer() )
			:
				front( init.first  ),
				back ( init.second )
			{}
			
			Front& GetFront() const  { return front; }
			Back & GetBack () const  { return back;  }
			
			void Plot( const Rect& area )
			{
				back .Plot( area );
				front.Plot( area );
			}
	};
	
}

#endif

