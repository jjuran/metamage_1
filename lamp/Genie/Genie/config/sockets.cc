/*
	config/sockets.cc
	-----------------
*/

// POSIX
#include <sys/socket.h>

// Genie
#include "Genie/SystemCallRegistry.hh"


#pragma force_active on

REGISTER_SYSTEM_CALL( socketpair  );
REGISTER_SYSTEM_CALL( socket      );
REGISTER_SYSTEM_CALL( bind        );
REGISTER_SYSTEM_CALL( listen      );
REGISTER_SYSTEM_CALL( accept      );
REGISTER_SYSTEM_CALL( connect     );
REGISTER_SYSTEM_CALL( getsockname );
REGISTER_SYSTEM_CALL( getpeername );
REGISTER_SYSTEM_CALL( shutdown    );

#pragma force_active reset

