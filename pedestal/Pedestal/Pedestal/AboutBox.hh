/*	===========
 *	AboutBox.hh
 *	===========
 */

#pragma once

// Standard C++
#include <memory>

// Pedestal
#include "Pedestal/FrontBackGraphic.hh"
#include "Pedestal/GraphicView.hh"
#include "Pedestal/Icons.hh"
#include "Pedestal/SolidColorGraphic.hh"
#include "Pedestal/View.hh"
#include "Pedestal/Window.hh"


namespace Pedestal
{
	
	//typedef Window< AboutView, noGrowDocProc > AboutBoxBase;
	typedef Window< GraphicView< FrontBackGraphic< IconID,
	                                               SolidColorGraphic > >,
	                noGrowDocProc >
	        AboutBoxBase;
	
	class AboutBox : public AboutBoxBase
	{
		public:
			typedef AboutBoxBase Base;
			
			AboutBox( WindowClosure& closure );
	};
	
	class AboutBoxOwner : public UniqueWindowOwner< AboutBox >
	{
		public:
			AboutBoxOwner()  {}
			
			void ShowAboutBox()  { Show(); }
	};
	
}

