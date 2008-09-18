/*	=========
 *	Filter.hh
 *	=========
 */

#ifndef BACKTRACE_FILTER_HH
#define BACKTRACE_FILTER_HH

#if PRAGMA_ONCE
#pragma once
#endif

// Standard C++
#include <string>


namespace Backtrace
{
	
	std::string FilterSymbol( const std::string& name );
	
}

#endif

