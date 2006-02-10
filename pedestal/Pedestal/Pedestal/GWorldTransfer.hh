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
	namespace NN = Nucleus;
	
	class GWorld : public Graphic
	{
		typedef NN::Owned< N::GWorldPtr, N::GWorldDisposer > Value;
		
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
			NN::Shared< N::GWorldPtr, N::GWorldDisposer > fGWorld;
			N::TransferMode fMode;
		
		public:
			struct Initializer
			{
				NN::Shared< N::GWorldPtr, N::GWorldDisposer > gworld;
				N::TransferMode mode;
				
				Initializer( const NN::Shared< N::GWorldPtr, N::GWorldDisposer >&  gworld,
				             N::TransferMode                                      mode )
				:
					gworld( gworld ),
					mode  ( mode   )
				{}
			};
			
			GWorldTransfer( Initializer init ) : fGWorld( init.gworld ), fMode( init.mode )  {}
			
			NN::Shared< N::GWorldPtr, N::GWorldDisposer > Get() const  { return fGWorld; }
			
			void Plot( const Rect& area ) const;
	};
	
}

