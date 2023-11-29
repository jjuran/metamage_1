/*
	config/sockets.cc
	-----------------
*/

// POSIX
#ifdef __APPLE__
// This is needed to compile with Mac OS X 10.3's headers.
#include <sys/types.h>
#endif
#include <sys/socket.h>

// relix-kernel
#include "relix/config/mini.hh"
#include "relix/syscall/accept.hh"
#include "relix/syscall/bind.hh"
#include "relix/syscall/connect.hh"
#include "relix/syscall/getpeername.hh"
#include "relix/syscall/getsockname.hh"
#include "relix/syscall/listen.hh"
#include "relix/syscall/recv.hh"
#include "relix/syscall/registry.hh"
#include "relix/syscall/send.hh"
#include "relix/syscall/shutdown.hh"
#include "relix/syscall/socket.hh"
#include "relix/syscall/socketpair.hh"


#ifndef CONFIG_SOCKETS
#define CONFIG_SOCKETS  1
#endif


namespace {

using relix::accept;
using relix::bind;
using relix::connect;
using relix::getpeername;
using relix::getsockname;
using relix::listen;
using relix::recv;
using relix::send;
using relix::shutdown;
using relix::socket;
using relix::socketpair;


#if CONFIG_SOCKETS

#pragma force_active on

REGISTER_SYSTEM_CALL( socketpair  );
REGISTER_SYSTEM_CALL( socket      );
REGISTER_SYSTEM_CALL( bind        );
REGISTER_SYSTEM_CALL( listen      );
REGISTER_SYSTEM_CALL( accept      );
REGISTER_SYSTEM_CALL( connect     );
REGISTER_SYSTEM_CALL( getsockname );
REGISTER_SYSTEM_CALL( getpeername );
REGISTER_SYSTEM_CALL( recv        );
REGISTER_SYSTEM_CALL( send        );
REGISTER_SYSTEM_CALL( shutdown    );

#pragma force_active reset

#endif

}  // namespace
