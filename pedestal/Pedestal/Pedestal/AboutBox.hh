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
			
			AboutBox();
			~AboutBox();
	};
	
	
	class AboutBoxOwner : public UniqueWindowOwner
	{
		private:
			static Window* New()  { return new AboutBox; }
		
		public:
			AboutBoxOwner() : UniqueWindowOwner( &New )
			{
			}
			
			~AboutBoxOwner();
			
			void ShowAboutBox()  { Show(); }
	};
	
}

#endif

