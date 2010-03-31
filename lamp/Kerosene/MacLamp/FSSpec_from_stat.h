/*
	FSSpec_from_stat.h
	------------------
	
	Joshua Juran
*/

#ifndef KEROSENE_FSSPECFROMSTAT_H
#define KEROSENE_FSSPECFROMSTAT_H

#ifdef __cplusplus

int FSSpec_from_stat( const struct stat& stat_buffer, struct FSSpec& result );

extern "C"
#endif

int FSSpec_from_stat( const struct stat* stat_buffer, struct FSSpec* result );

#endif

