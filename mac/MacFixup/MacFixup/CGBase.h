/*
	MacFixup/CGBase.h
	-----------------
	
	Copyright 2010, Joshua Juran
*/

#ifndef MACFIXUP_CGBASE_H
#define MACFIXUP_CGBASE_H

#ifdef __CGBASE__
#error CGBase.h included too early to divert duplicate definitions
#endif

#if defined(_MACHTYPES_H_)
#define _defined_MACHTYPES_H_
#else
#define _MACHTYPES_H_
#endif

#include <CIncludes/CGBase.h>

#ifdef _defined_MACHTYPES_H_
#undef _defined_MACHTYPES_H_
#else
#undef _MACHTYPES_H_
#endif

#endif

