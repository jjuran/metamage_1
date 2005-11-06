/*	=========
 *	A-line.cc
 *	=========
 */

#pragma once

// C++
#include <string>


namespace ALine
{
	
	using std::string;
	
	/*
	struct TargetLibrary        {};
	struct TargetApplication    {};
	struct TargetSharedLibrary  {};
	struct TargetCodeResource   {};
	
	static TargetLibrary       targetLibrary;
	static TargetApplication   targetApplication;
	static TargetSharedLibrary targetSharedLibrary;
	static TargetCodeResource  targetCodeResource;
	*/
	
	string& operator<<( string& str, const string& appendage );
	
}

