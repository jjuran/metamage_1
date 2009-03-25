/*	===========
 *	AboutBox.hh
 *	===========
 */

#ifndef PEDESTAL_ABOUTBOX_HH
#define PEDESTAL_ABOUTBOX_HH

// Pedestal
#include "Pedestal/UniqueWindowOwner.hh"
#include "Pedestal/Window.hh"


namespace Pedestal
{
	
	class AboutBoxOwner : public UniqueWindowOwner
	{
		private:
			static std::auto_ptr< Window > New();
		
		public:
			AboutBoxOwner() : UniqueWindowOwner( &New )
			{
			}
			
			~AboutBoxOwner();
			
			void ShowAboutBox()  { Show(); }
	};
	
}

#endif

