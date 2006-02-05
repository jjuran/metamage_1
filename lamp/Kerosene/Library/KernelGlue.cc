/*	=============
 *	KernelGlue.cc
 *	=============
 */

// Nitrogen
#include "Nitrogen/CodeFragments.h"
#include "Nitrogen/Processes.h"
#include "Nitrogen/Str.h"

// Kerosene
#include "KernelGlue.hh"


namespace Kerosene
{
	
	#pragma mark -
	#pragma mark ¥ Glue ¥
	
	namespace N = Nitrogen;
	
	static N::CFragConnectionID KernelLib()
	{
		FSSpec appFile = N::GetProcessAppSpec( N::CurrentProcess() );
		
		static N::Owned< N::CFragConnectionID > kernelLib = N::GetDiskFragment< kReferenceCFrag >( appFile );
		
		return kernelLib;
	}
	
	static int Dummy()
	{
		return -1;
	}
	
	void* GetSymbol( const char* name )
	{
		try
		{
			return N::FindSymbol< void* >( KernelLib(), N::Str255( name ) );
		}
		catch ( ... )  {}
		
		return &Dummy;
	}
	
}

