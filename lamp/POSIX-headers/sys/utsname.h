/*	=============
 *	sys/utsname.h
 *	=============
 */

#ifndef LAMP_SYS_UTSNAME_H
#define LAMP_SYS_UTSNAME_H

#ifdef __cplusplus
extern "C" {
#endif
	
	#define _UTSNAME_LENGTH 65
	
	struct utsname
	{
		char sysname [_UTSNAME_LENGTH];
		char nodename[_UTSNAME_LENGTH];
		char release [_UTSNAME_LENGTH];
		char version [_UTSNAME_LENGTH];
		char machine [_UTSNAME_LENGTH];
	};
	
	int uname( struct utsname *uts );
	
#ifdef __cplusplus
}
#endif

#endif

