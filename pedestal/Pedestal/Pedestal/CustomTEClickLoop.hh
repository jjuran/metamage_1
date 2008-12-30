/*	====================
 *	CustomTEClickLoop.hh
 *	====================
 */

#ifndef PEDESTAL_CUSTOMTECLICKLOOP_HH
#define PEDESTAL_CUSTOMTECLICKLOOP_HH


struct TERec;


namespace Pedestal
{
	
	class TEClickLoop_User
	{
		protected:
			struct TEClickLoop_Scope
			{
				TEClickLoop_Scope( TEClickLoop_User* user );
				
				~TEClickLoop_Scope();
			};
		
		public:
			virtual void ClickInLoop() = 0;
	};
	
	class TEClickLoop_Subject
	{
		protected:
			struct TEClickLoop_Scope
			{
				TEClickLoop_Scope( TEClickLoop_Subject* subject );
				
				~TEClickLoop_Scope();
			};
			
			static void InstallCustomTEClickLoop( TERec** hTE );
		
		public:
			virtual void ClickInLoop()  {}
	};
	
}

#endif

