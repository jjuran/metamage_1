/*	=================
 *	FSTree_sys_cpu.cc
 *	=================
 */

#include "Genie/FileSystem/FSTree_sys_cpu.hh"

// Nucleus
#include "Nucleus/Convert.h"

// Nitrogen
#include "Nitrogen/Gestalt.h"

// Genie
#include "Genie/FileSystem/FSTree_QueryFile.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	struct GetCPUProc
	{
		typedef long Result;
		
		Result Get() const
		{
			long proc = N::Gestalt( N::Gestalt_Selector( 'proc' ) );
			
			return 68000 + 10 * (proc - 1);
		}
	};
	
	struct GetPrivilegeMode
	{
		typedef std::string Result;
		
		Result Get() const
		{
			UInt16 status = 0;
			
		#if TARGET_CPU_68K
			
			asm
			{
				MOVE.W		SR,status
			}
			
		#endif
			
			std::string text = status & (1 << 13) ? "supervisor" : "user";
			
			return text;
		}
	};
	
	template < class Accessor >
	class sys_app_Query
	{
		public:
			std::string Get() const
			{
				std::string output = NN::Convert< std::string >( Accessor().Get() ) + "\n";
				
				return output;
			}
	};
	
	template < class Accessor >
	static FSTreePtr Query_Factory( const FSTreePtr&    parent,
	                                const std::string&  name )
	{
		typedef sys_app_Query< Accessor > Query;
		
		typedef FSTree_QueryFile< Query > QueryFile;
		
		return MakeFSTree( new QueryFile( parent, name ) );
	}
	
	const Singleton_Mapping sys_cpu_Mappings[] =
	{
		{ "proc", &Query_Factory< GetCPUProc > },
		
	#if TARGET_CPU_68K
		
		{ "mode", &Query_Factory< GetPrivilegeMode > },
		
	#endif
		
		{ NULL, NULL }
		
	};
	
}

