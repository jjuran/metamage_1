/*	===========
 *	AboutBox.cc
 *	===========
 */

#include "Pedestal/AboutBox.hh"

// Nitrogen
#include "Nitrogen/Icons.h"
#include "Nitrogen/QuickDraw.h"
#include "Nitrogen/Timer.h"

// Pedestal
#include "Pedestal/FrontBackGraphic.hh"
#include "Pedestal/GeneratedGraphic.hh"
#include "Pedestal/GraphicView.hh"
#include "Pedestal/Icons.hh"
#include "Pedestal/SolidColorGraphic.hh"


#define GEN_FUNCTION AboutFunction

//#define ABOUT_GRAPHIC IconID
//#define ABOUT_GRAPHIC MaskedIcon
#define ABOUT_GRAPHIC GeneratedGraphic< GEN_FUNCTION >


namespace Pedestal
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	class AboutFunction
	{
		public:
			RGBColor operator()( double x,
			                     double y,
			                     double t ) const;
	};
	
	typedef GraphicView< FrontBackGraphic< ABOUT_GRAPHIC,
	                                       SolidColorGraphic > >
	        AboutBoxView;
	
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
	
	typedef std::pair< ABOUT_GRAPHIC    ::Initializer,
	                   SolidColorGraphic::Initializer > AboutBoxInit;
	
	static inline Rect MakeBounds()
	{
		return N::SetRect( 0, 0, 128, 128 );
	}
	
	static inline AboutBoxInit MakeInit()
	{
		return
		
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
		;
	}
	
	static inline std::auto_ptr< View > MakeView()
	{
		return std::auto_ptr< View >( new AboutBoxView( MakeBounds(), MakeInit() ) );
	}
	
	AboutBox::AboutBox( const boost::shared_ptr< WindowCloseHandler >& handler )
	: 
		Base( NewWindowContext( N::OffsetRect( MakeBounds(),
		                                       300,
		                                       200 ),
		                        "\pPedestal" ),
		      N::noGrowDocProc
		      )
	{
		SetCloseHandler( handler );
		
		SetView( MakeView() );
	}
	
	AboutBox::~AboutBox()
	{
	}
	
	AboutBoxOwner::~AboutBoxOwner()
	{
	}
	
}

