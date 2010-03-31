/*
	FSSpec_from_stat.cc
	-------------------
	
	Joshua Juran
*/

#include "FSSpec_from_stat.h"

// Standard C
#include <errno.h>
#include <string.h>

// POSIX
#include <sys/stat.h>

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif


#pragma exceptions off


int FSSpec_from_stat( const struct stat& stat_buffer, FSSpec& result )
{
	typedef UInt32 FSDirID;
	
	const FSVolumeRefNum vRefNum = -stat_buffer.st_dev;
	const FSDirID        parID   = stat_buffer.st_rdev;
	
	const unsigned name_length = stat_buffer.st_name[0];
	
	if ( vRefNum == 0  ||  name_length == 0 )
	{
		errno = EXDEV;
		
		return -1;
	}
	
	if ( name_length > 31 )
	{
		errno = ENAMETOOLONG;
		
		return -1;
	}
	
	result.vRefNum = vRefNum;
	result.parID   = parID;
	
	memcpy( result.name, stat_buffer.st_name, 1 + name_length );
	
	return 0;
}

int FSSpec_from_stat( const struct stat* stat_buffer, FSSpec* result )
{
	if ( stat_buffer == NULL )
	{
		errno = EFAULT;
		
		return -1;
	}
	
	FSSpec dummy;
	
	return FSSpec_from_stat( *stat_buffer, result ? *result : dummy );
}

