/*	==============
 *	TargetTypes.hh
 *	==============
 */

#ifndef ALINE_TARGET_TYPES_HH
#define ALINE_TARGET_TYPES_HH


namespace ALine
{
	
	enum ProductType
	{
		productNotBuilt = 0,
		productApplication,
		productStaticLib,
		productSharedLib,
		productWish,
		productINIT,
		productDriver
	};
	
	enum Toolkit
	{
		toolkitUnspecified = 0, 
		toolkitCodeWarrior, 
		toolkitGNU
	};
	
	enum BuildVariety
	{
		buildDefault,
		buildDebug,
		buildRelease,
		buildDemo
	};
	
}

#endif

