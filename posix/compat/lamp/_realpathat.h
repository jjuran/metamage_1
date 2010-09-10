/*
	_realpathat.h
	-------------
*/

#ifndef LAMP_REALPATHAT_H
#define LAMP_REALPATHAT_H

#include <sys/types.h>


#define REALPATH_OUTPUT_HFS       0x0000001
#define REALPATH_OUTPUT_UTF8      0x0000002
#define REALPATH_OUTPUT_HFS_UTF8  0x0000003


#ifdef __cplusplus
extern "C" {
#endif

ssize_t _realpathat( int dirfd, const char *path, char *buffer, size_t buffer_size, int flags );

#ifdef __cplusplus
}
#endif

#endif

