/*
	empty_signal_handler.hh
	-----------------------
*/

#ifndef VARYX_POSIX_EMPTYSIGNALHANDLER_HH
#define VARYX_POSIX_EMPTYSIGNALHANDLER_HH

namespace varyx {
namespace posix {

void install_empty_signal_handler( int signum );
bool verify_empty_signal_handler ( int signum );

}
}

#endif
