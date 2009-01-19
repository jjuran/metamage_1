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
#include "Genie/FileSystem/FSTree_Property.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	struct GetCPUProc
	{
		typedef long Result;
		
		static Result Get()
		{
			long proc = N::Gestalt( N::Gestalt_Selector( 'proc' ) );
			
			return 68000 + 10 * (proc - 1);
		}
	};
	
	struct GetPrivilegeMode
	{
		typedef std::string Result;
		
		static Result Get()
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
	struct sys_cpu_Property
	{
		static std::string Read( const FSTree* that, bool binary )
		{
			return NN::Convert< std::string >( Accessor::Get() );
		}
	};
	
	template < class Accessor >
	static FSTreePtr Property_Factory( const FSTreePtr&    parent,
	                                   const std::string&  name )
	{
		typedef sys_cpu_Property< Accessor > Property;
		
		return FSTreePtr( new FSTree_Property( parent,
		                                       name,
		                                       &Property::Read ) );
	}
	
	const FSTree_Premapped::Mapping sys_cpu_Mappings[] =
	{
		{ "proc", &Property_Factory< GetCPUProc > },
		
	#if TARGET_CPU_68K
		
		{ "mode", &Property_Factory< GetPrivilegeMode > },
		
	#endif
		
		{ NULL, NULL }
		
	};
	
}

