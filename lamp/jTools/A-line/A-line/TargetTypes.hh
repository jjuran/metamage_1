/*	==============
 *	TargetTypes.hh
 *	==============
 */

#ifndef ALINE_TARGET_TYPES_HH
#define ALINE_TARGET_TYPES_HH


enum ProductType
{
	productNotBuilt = 0,
	productSource,
	productApplication,
	productDropIn,
	productStaticLib,
	productSharedLib,
	productTool,
	productToolkit,
	productINIT,
	productDA,
	productDriver
};

inline bool ProductGetsBuilt( ProductType product )
{
	return product > productSource;
}

enum Toolchain
{
	toolchainUnspecified = 0,
	toolchainMetrowerks,
	toolchainGNU
};

enum EnvType
{
	envUnspecified = 0,
	envRelix,
	envUnix
};

enum BuildVariety
{
	buildDefault,
	buildSymbolics,
	buildDebug,
	buildRelease
};

#endif
