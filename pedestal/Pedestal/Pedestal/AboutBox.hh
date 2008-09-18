/*	===========
 *	AboutBox.hh
 *	===========
 */

#ifndef PEDESTAL_ABOUTBOX_HH
#define PEDESTAL_ABOUTBOX_HH

#if PRAGMA_ONCE
#pragma once
#endif

// Standard C++
#include <memory>

// Pedestal
#include "Pedestal/FrontBackGraphic.hh"
#include "Pedestal/GeneratedGraphic.hh"
#include "Pedestal/GraphicView.hh"
#include "Pedestal/Icons.hh"
#include "Pedestal/SolidColorGraphic.hh"
#include "Pedestal/View.hh"
#include "Pedestal/Window.hh"


#define GEN_FUNCTION AboutFunction

//#define ABOUT_GRAPHIC IconID
//#define ABOUT_GRAPHIC MaskedIcon
#define ABOUT_GRAPHIC GeneratedGraphic< GEN_FUNCTION >


namespace Pedestal
{
	
	class AboutFunction
	{
		public:
			RGBColor operator()( double x,
			                     double y,
			                     double t ) const;
	};
	
	typedef Window< GraphicView< FrontBackGraphic< ABOUT_GRAPHIC,
	                                               SolidColorGraphic > >,
	                Static_DefProcID< Nitrogen::noGrowDocProc > >
	        AboutBoxBase;
	
	class AboutBox : public AboutBoxBase
	{
		public:
			typedef AboutBoxBase Base;
			
			AboutBox( const boost::shared_ptr< WindowCloseHandler >& handler );
			~AboutBox();
	};
	
	class AboutBoxOwner : public UniqueWindowOwner< AboutBox >
	{
		public:
			AboutBoxOwner()  {}
			~AboutBoxOwner();
			
			void ShowAboutBox()  { Show(); }
	};
	
}

#endif

