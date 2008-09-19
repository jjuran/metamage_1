/*	=================
 *	GWorldTransfer.hh
 *	=================
 */

#ifndef PEDESTAL_QWORLDTRANSFER_HH
#define PEDESTAL_QWORLDTRANSFER_HH

// Nucleus
#include "Nucleus/Shared.h"

// Nitrogen
#include "Nitrogen/QDOffscreen.h"

// Pedestal
#include "Pedestal/Graphic.hh"


namespace Pedestal
{
	
	class GWorld : public Graphic
	{
		typedef Nucleus::Owned< Nitrogen::GWorldPtr > Value;
		
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
			Nucleus::Shared< Nitrogen::GWorldPtr > fGWorld;
			Nitrogen::TransferMode fMode;
		
		public:
			struct Initializer
			{
				Nucleus::Shared< Nitrogen::GWorldPtr > gworld;
				Nitrogen::TransferMode mode;
				
				Initializer( const Nucleus::Shared< Nitrogen::GWorldPtr >&  gworld,
				             Nitrogen::TransferMode                         mode )
				:
					gworld( gworld ),
					mode  ( mode   )
				{}
			};
			
			GWorldTransfer( Initializer init ) : fGWorld( init.gworld ), fMode( init.mode )  {}
			
			Nucleus::Shared< Nitrogen::GWorldPtr > Get() const  { return fGWorld; }
			
			void Plot( const Rect& area ) const;
	};
	
}

#endif

