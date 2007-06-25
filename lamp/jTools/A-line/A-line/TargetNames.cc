/*	==============
 *	TargetNames.cc
 *	==============
 */

#include "A-line/TargetNames.hh"


namespace ALine
{
	
	std::string ArchName( CD::Architecture arch )
	{
		switch ( arch )
		{
			case CD::arch68K:  return "68K";
			case CD::archPPC:  return "PPC";
			case CD::archX86:  return "X86";
		};
		
		return "";
	}
	
	static std::string ToolkitName( Toolkit tools )
	{
		switch ( tools )
		{
			case toolkitCodeWarrior:  return "CW";
			case toolkitGNU:          return "GNU";
		};
		
		return "";
	}
	
	static std::string RuntimeName( CD::Runtime rt )
	{
		switch ( rt )
		{
			case CD::runtimeA4CodeResource:  return "Res";
			case CD::runtimeA5CodeSegments:  return "Code";
			case CD::runtimeCodeFragments:   return "CFM";
			case CD::runtimeMachO:           return "MachO";
			case CD::runtimeELF:             return "ELF";
		};
		
		return "";
	}
	
	static std::string APIName( CD::MacAPI api )
	{
		switch ( api )
		{
			case CD::apiNotApplicable:  return "NA";
			case CD::apiMacToolbox:     return "Toolbox";
			case CD::apiMacCarbon:      return "Carbon";
		};
		
		return "";
	}
	
	static std::string BuildName( BuildVariety build )
	{
		switch ( build )
		{
			case buildDebug:    return "Debug";
			case buildRelease:  return "Release";
			case buildDemo:     return "Demo";
		};
		
		return "";
	}
	
	TargetName MakeTargetName( CD::Architecture  arch,
	                           CD::Runtime       runtime,
	                           CD::MacAPI        api,
	                           BuildVariety      build )
	{
		return ArchName   ( arch    ) + "-" +
		       RuntimeName( runtime ) + "-" +
		       APIName    ( api     ) + "-" +
		       BuildName  ( build   );
			
	}
	
}

