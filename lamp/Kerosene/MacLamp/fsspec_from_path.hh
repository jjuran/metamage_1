/*	===================
 *	fsspec_from_path.hh
 *	===================
 */

#ifndef KEROSENE_FSSPECFROMPATH_HH
#define KEROSENE_FSSPECFROMPATH_HH

// Mac OS
#include <TargetConditionals.h>
#if TARGET_RT_MAC_MACHO
#include <Carbon/Carbon.h>
#else
#include <Files.h>
#endif


#ifdef __cplusplus
#include <string>
#endif


#ifdef __cplusplus
extern "C" {
#endif
	
	int fsspec_from_path( const char*  pathname,
	                      FSSpec*      outFSS );
	
	FSSpec make_fsspec_from_path( const char* pathname );
	
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
	
	inline FSSpec make_fsspec_from_path( const std::string& pathname )
	{
		return make_fsspec_from_path( pathname.c_str() );
	}
	
#endif

#endif

