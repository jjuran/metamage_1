/*	=================
 *	GWorldTransfer.hh
 *	=================
 */

#pragma once

// Pedestal
#include "Pedestal/Graphic.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	class GWorld : public Graphic
	{
		typedef N::Owned< N::GWorldPtr, N::GWorldDisposer > Value;
		
		private:
			Value fGWorld;
		
		public:
			struct Initializer
			{
				int depth;
				Rect bounds;
				
				Initializer( int depth, const Rect& bounds ) : depth( depth ), bounds( bounds )  {}
			};
			
			GWorld( const Initializer& init );
			
			void Plot( const Rect& area ) const;
	};
	
	class GWorldTransfer : public Graphic
	{
		private:
			N::Shared< N::GWorldPtr, N::GWorldDisposer > fGWorld;
			N::TransferMode fMode;
		
		public:
			struct Initializer
			{
				N::Shared< N::GWorldPtr, N::GWorldDisposer > gworld;
				N::TransferMode mode;
				
				Initializer( const N::Shared< N::GWorldPtr, N::GWorldDisposer >&  gworld,
				             N::TransferMode                                      mode )
				:
					gworld( gworld ),
					mode  ( mode   )
				{}
			};
			
			GWorldTransfer( Initializer init ) : fGWorld( init.gworld ), fMode( init.mode )  {}
			
			N::Shared< N::GWorldPtr, N::GWorldDisposer > Get() const  { return fGWorld; }
			
			void Plot( const Rect& area ) const;
	};
	
}

