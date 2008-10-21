/*	===========
 *	AboutBox.hh
 *	===========
 */

#ifndef PEDESTAL_ABOUTBOX_HH
#define PEDESTAL_ABOUTBOX_HH

// Standard C++
#include <memory>

// Pedestal
#include "Pedestal/UserWindow.hh"


namespace Pedestal
{
	
	class AboutBox : public UserWindow
	{
		public:
			typedef UserWindow Base;
			
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

