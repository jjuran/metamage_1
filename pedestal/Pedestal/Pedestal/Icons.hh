/*	=================
 *	Pedestal/Icons.hh
 *	=================
 */

#ifndef PEDESTAL_ICONS_HH
#define PEDESTAL_ICONS_HH

// Nitrogen
#include "Nitrogen/Icons.h"

// Pedestal
#include "Pedestal/Graphic.hh"


namespace Pedestal
{
	
	class PlainIcon : public Graphic
	{
		typedef Nitrogen::PlainIconHandle Handle;
		
		typedef Nucleus::Owned< Handle >  Value;
		typedef Nucleus::Owned< Handle >  Parameter;
		typedef Handle                    GetResult;
		
		private:
			Value value;
		
		public:
			PlainIcon( Parameter v ) : value( v )  {}
			
			GetResult Get() const  { return value; }
			
			void Plot( const Rect& area ) const;
	};
	
	class MaskedIcon : public Graphic
	{
		typedef Nitrogen::MaskedIconHandle Handle;
		
		typedef Nucleus::Owned< Handle >  Value;
		typedef Nucleus::Owned< Handle >  Parameter;
		typedef Handle                    GetResult;
		
		private:
			Value value;
		
		public:
			MaskedIcon( Parameter v ) : value( v )  {}
			
			GetResult Get() const  { return value; }
			
			void Plot( const Rect& area ) const;
	};
	
	class IconID : public Graphic
	{
		typedef Nitrogen::ResID Value;
		typedef Nitrogen::ResID Parameter;
		typedef Nitrogen::ResID GetResult;
		
		private:
			Value value;
		
		public:
			IconID( Parameter v ) : value( v )  {}
			
			GetResult Get() const  { return value; }
			
			void Plot( const Rect& area ) const;
	};
	
}

#endif

