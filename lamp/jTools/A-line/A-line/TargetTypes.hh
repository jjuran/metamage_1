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
		productSource,
		productApplication,
		productStaticLib,
		productSharedLib,
		productTool,
		productINIT,
		productDriver
	};
	
	inline bool ProductGetsBuilt( ProductType product )
	{
		return product > productSource;
	}
	
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

