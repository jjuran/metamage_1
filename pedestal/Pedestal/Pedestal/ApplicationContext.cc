/*	=====================
 *	ApplicationContext.cc
 *	=====================
 */

#include "Pedestal/ApplicationContext.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	MacToolboxInit::MacToolboxInit()
	{
	#if !TARGET_API_MAC_CARBON
		
		::InitGraf( &qd.thePort );
		::InitFonts();
		::InitWindows();
		::InitMenus();
		::TEInit();
		::InitDialogs( NULL );
		
	#endif
		
		::InitCursor();
		// FlushEvents?
	}
	
	MemoryInit::MemoryInit( std::size_t moreMasters )
	{
	#if !TARGET_API_MAC_CARBON
		
		::MaxApplZone();
		
	#endif
		
		for ( std::size_t i = 0;  i < moreMasters;  ++i )
		{
			::MoreMasters();
		}
	}
	
	static VersRec ReadVersion()
	{
		// Read our version resource.
		try
		{
			return N::Get1Resource< N::kVersionResType >( N::ResID( 1 ) );
		}
		catch ( const N::OSStatus& err )
		{
			if ( err != resNotFound )
			{
				throw;
			}
			
			// Return below
		}
		
		return VersRec();
	}
	
	ApplicationContext::ApplicationContext()
	:
		itsMemoryInit   ( 0 ), 
		itsResFileRefNum( N::CurResFile() ), 
		itsVersion      ( ReadVersion() )
	{
	}
	
}

