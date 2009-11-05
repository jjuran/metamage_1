/*
	copyfile.h
	----------
	
	Joshua Juran
*/

#ifndef LAMP_COPYFILE_H
#define LAMP_COPYFILE_H

#ifdef __cplusplus
extern "C" {
#endif

int copyfile( const char* src, const char* dest );

int copyfileat( int olddirfd, const char* oldpath, int newdirfd, const char* newpath, unsigned flags );

#ifdef __cplusplus
}
#endif

#endif

