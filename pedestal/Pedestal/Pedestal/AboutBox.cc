/*	===========
 *	AboutBox.cc
 *	===========
 */

#include "Pedestal/AboutBox.hh"

// Nitrogen
#include "Nitrogen/Icons.h"
#include "Nitrogen/QuickDraw.h"
#include "Nitrogen/Timer.h"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	inline double Inv( double x )  { return 1.0 - x; }
	
	RGBColor AboutFunction::operator()( double x,
			                            double y,
			                            double t ) const
	{
		return DenormalizeRGBColor( x, y, t );
	}
	
	template < class A, class B >
	std::pair< A, B > MakePair( A a, B b )
	{
		std::pair< A, B > result;
		
		result.first  = a;
		result.second = b;
		
		return result;
	}
	
	AboutBox::AboutBox( const boost::shared_ptr< WindowCloseHandler >& handler )
	: 
		Base( NewWindowContext( N::OffsetRect( N::SetRect( 0, 0, 128, 128 ),
		                                       300,
		                                       200 ),
		                        "\pPedestal" ),
		      handler,
		      #if 0
		      MakePair( N::DetachResource( N::GetResource< kLarge1BitMask >( N::ResID( 128 ) ) ),
		                NN::Make< RGBColor >( 0xDDDD ) )
		      #elif 0
		      std::make_pair( N::ResID( 128 ),
		                      NN::Make< RGBColor >( 0xDDDD ) )
		      #else
		      std::make_pair( GEN_FUNCTION(),
		                      NN::Make< RGBColor >( 0xDDDD ) )
		      #endif
		)
	{
	}
	
	AboutBox::~AboutBox()
	{
	}
	
	AboutBoxOwner::~AboutBoxOwner()
	{
	}
	
}

