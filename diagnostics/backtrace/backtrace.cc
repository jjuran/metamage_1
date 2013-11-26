/*
	backtrace.cc
	------------
*/

// POSIX
#include <unistd.h>

// plus
#include "plus/string.hh"
#include "plus/var_string.hh"

// recall
#include "recall/backtrace.hh"


static plus::string get_backtrace()
{
	recall::frame_data frames[ 64 ];
	
	unsigned n = make_stack_crawl( frames, 64 );
	
	plus::var_string report;
	
	make_report_from_stack_crawl( report, frames, frames + n );
	
	return report.move();
}

static void print_backtrace()
{
	const plus::string report = get_backtrace();
	
	write( STDOUT_FILENO, report.data(), report.size() );
}

int main( int argc, char** argv )
{
	print_backtrace();
	
	return 0;
}

