/*
	argv.hh
	-------
*/

#ifndef VARYX_POSIX_ARGV_HH
#define VARYX_POSIX_ARGV_HH

namespace vlib
{
	
	class Symbol;
	
}

namespace varyx
{
namespace posix
{
	
	using vlib::Symbol;
	
	void set_argv( Symbol& sym, const char* arg0, char* const* args );
	
}
}

#endif
