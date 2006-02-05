/*	===============
 *	KEnvironment.hh
 *	===============
 */

#ifndef KEROSENE_KENVIRONMENT_HH
#define KEROSENE_KENVIRONMENT_HH

namespace Kerosene
{
	
	enum
	{
		kCurrentVersion = 2,
		kLastCompatibleAppVersion = 2,
		kLastCompatibleLibVersion = 2
	};
	
	struct Versions
	{
		int current;
		int lastCompatible;
		
		Versions()
		:
			current       ( kCurrentVersion           ),
			lastCompatible( kLastCompatibleAppVersion )
		{}
	};
	
}

#endif

