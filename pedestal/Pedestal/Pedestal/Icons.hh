/*	========
 *	Icons.hh
 *	========
 */

#pragma once

// Pedestal
#include "Pedestal/Graphic.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	class IconID : public Graphic
	{
		private:
			N::ResID resID;
		
		public:
			typedef N::ResID Initializer;
			
			IconID( const N::ResID& resID ) : resID( resID )  {}
			
			N::ResID Get() const  { return resID; }
			
			void Plot( const Rect& area );
	};
	
}

