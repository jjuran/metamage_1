/*	=============
 *	TargetInfo.cc
 *	=============
 */

#include "A-line/TargetInfo.hh"

// Nitrogen
//#include "Nitrogen/MacErrors.hh"
//#include "Nitrogen/OSStatus.hh"


namespace tool
{
	
	//namespace N = Nitrogen;
	
	/*
	template < class Type >
	void SetWithoutConflict( Type& var, Type value, Type null = Type( 0 ) )
	{
		if ( var != value )
		{
			if ( var == null )
			{
				var = value;
			}
			else
			{
				// Error
				throw N::ParamErr();
			}
		}
	}
	
	void TargetInfo::DeduceFromProduct( ProductType product )
	{
		// Possible values on input (besides unspecified):
		// arch:  arch68K, archPPC
		// runtime:  runtimeCodeFragments, runtimeMachO
		// api:  apiMacCarbon
		
		switch ( product )
		{
			case productApplication:
				break;
			case productStaticLib:
				break;
			case productSharedLib:
			case productWish:
				//SetWithoutConflict( runtime, runtimeCodeFragments );
				//runtime = runtimeCodeFragments;
				break;
			case productINIT:
			case productDriver:
				SetWithoutConflict( arch, arch68K );
				SetWithoutConflict( runtime, runtimeA4CodeResource );
				SetWithoutConflict( api, apiMacToolbox );
				break;
			};
	}
	*/
	
	/*
	void TargetInfo::ApplyOptions( TargetInfo options )
	{
		if ( arch == archUnspecified )
		{
			arch = options.arch;
		}
		if ( runtime == runtimeUnspecified )
		{
			runtime = options.runtime;
		}
		if ( api == apiUnspecified )
		{
			api = options.api;
		}
		if ( toolkit == toolkitUnspecified )
		{
			toolkit = options.toolkit;
		}
		if ( build == buildDefault )
		{
			build = options.build;
		}
	}
	
	void TargetInfo::ApplyDefaults()
	{
		// Check requirements
		
	#if TARGET_RT_MAC_MACHO
		
		if ( runtime == runtimeUnspecified )
		{
			runtime = runtimeMachO;
		}
		
	#endif
		
		// Mach-O requires PPC/Carbon (Cross-compiler?  Anyone?  Anyone?)
		// (Carbon requires PPC, which we check for later.)
		if ( runtime == runtimeMachO )
		{
			SetWithoutConflict( api, apiMacCarbon );
			SetWithoutConflict( toolkit, toolkitGNU );
		}
		else
		{
			SetWithoutConflict( toolkit, toolkitCodeWarrior );
		}
		// Carbon requires PPC
		if ( api == apiMacCarbon )
		{
			SetWithoutConflict( arch, archPPC );
		}
		
		if ( arch == archUnspecified )
		{
			arch = archPPC;
		}
		if ( runtime == runtimeUnspecified )
		{
			if ( arch == archPPC )
			{
				runtime = runtimeCodeFragments;
			}
			else
			{
				// Must be an app (A5 code).
				// Code resources would already have specified such,
				// and CFM must be set explicitly.
				runtime = runtimeA5CodeSegments;
				SetWithoutConflict( api, apiMacToolbox );
			}
		}
		
		// Carbon or classic Toolbox?  Only applies to PPC/CFM.
		if ( arch == archPPC  &&  runtime == runtimeCodeFragments  &&  api == apiUnspecified )
		{
			api = TARGET_API_MAC_CARBON ? apiMacCarbon : apiMacToolbox;
		}
		
		if ( build == buildDefault )
		{
			build = buildDebug;
		}
	}
	*/
	
}

