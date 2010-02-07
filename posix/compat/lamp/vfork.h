/*
	vfork.h
	-------
	
	Copyright 2010, Joshua Juran
*/

#ifndef LAMP_VFORK_H
#define LAMP_VFORK_H

// POSIX
#include <unistd.h>


typedef void (*_resume_handler_t)( const struct _vfork_pad* );

struct _vfork_pad
{
	union
	{
		_resume_handler_t  resume_handler;
		long               pid;
	};
	
#ifdef __MC68K__

	// 48 bytes
	
	// pid (addressed as d0)
	// d0-d2 are volatile
	long  d3;
	long  d4;
	long  d5;
	long  d6;
	long  d7;
	
	void* pc;  // addressed as a1
	// a0,a1 are volatile
	void* a2;
	void* a3;
	void* a4;
	// a5 is constant
	void* fp;  // frame pointer, a6
	void* sp;  // stack pointer, a7

#endif

#ifdef __POWERPC__

	// 248 bytes
	
	// pid
	void* pc;
	long  cr;
	void* fp;  // combined stack/frame pointer, r1, aka sp
	void* backlink;
	
	long gpr[ 1 - 13 + 31 ];  // 19
	
	double fs;
	double fpr[ 1 - 14 + 31 ];  // 18

#endif
};

#endif

