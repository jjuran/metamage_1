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
	
	class PlainIcon : public Graphic
	{
		typedef N::PlainIconHandle Handle;
		
		typedef N::Owned< Handle >  Value;
		typedef N::Owned< Handle >  Parameter;
		typedef Handle              GetResult;
		
		private:
			Value value;
		
		public:
			typedef Parameter Initializer;
			
			PlainIcon( Parameter v ) : value( v )  {}
			
			GetResult Get() const  { return value; }
			
			void Plot( const Rect& area ) const;
	};
	
	class MaskedIcon : public Graphic
	{
		typedef N::MaskedIconHandle Handle;
		
		typedef N::Owned< Handle >  Value;
		typedef N::Owned< Handle >  Parameter;
		typedef Handle              GetResult;
		
		private:
			Value value;
		
		public:
			typedef Parameter Initializer;
			
			MaskedIcon( Parameter v ) : value( v )  {}
			
			GetResult Get() const  { return value; }
			
			void Plot( const Rect& area ) const;
	};
	
	class IconID : public Graphic
	{
		typedef N::ResID Value;
		typedef N::ResID Parameter;
		typedef N::ResID GetResult;
		
		private:
			Value value;
		
		public:
			typedef Parameter Initializer;
			
			IconID( Parameter v ) : value( v )  {}
			
			GetResult Get() const  { return value; }
			
			void Plot( const Rect& area ) const;
	};
	
}

