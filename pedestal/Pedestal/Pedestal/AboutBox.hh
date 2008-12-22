/*	===========
 *	AboutBox.hh
 *	===========
 */

#ifndef PEDESTAL_ABOUTBOX_HH
#define PEDESTAL_ABOUTBOX_HH

// Pedestal
#include "Pedestal/Window.hh"


namespace Pedestal
{
	
	class AboutBox : public Window
	{
		public:
			typedef Window Base;
			
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

