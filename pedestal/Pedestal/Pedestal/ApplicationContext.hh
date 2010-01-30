/*	=====================
 *	ApplicationContext.hh
 *	=====================
 */

#ifndef PEDESTAL_APPLICATIONCONTEXT_HH
#define PEDESTAL_APPLICATIONCONTEXT_HH


namespace Pedestal
{
	
	struct MacToolboxInit
	{
		MacToolboxInit();
	};
	
	struct MemoryInit
	{
		MemoryInit( unsigned moreMasters );
	};
	
	class ApplicationContext
	{
		private:
			MacToolboxInit  itsMacToolboxInit;
			MemoryInit      itsMemoryInit;
		
		public:
			ApplicationContext();
	};
	
}

#endif

