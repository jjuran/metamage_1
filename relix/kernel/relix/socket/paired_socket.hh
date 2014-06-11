/*
	paired_socket.hh
	----------------
*/

#ifndef RELIX_SOCKET_PAIREDSOCKET_HH
#define RELIX_SOCKET_PAIREDSOCKET_HH

// vfs
#include "vfs/stream.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/general_method_set.hh"
#include "vfs/filehandle/methods/stream_method_set.hh"


namespace relix
{
	
	struct pairedsocket_extra
	{
		vfs::stream*  input;
		vfs::stream*  output;
	};
	
	void pairedsocket_shutdown( vfs::filehandle* that, int how );
	
	int pairedsocket_conveying( vfs::filehandle* that );
	
	extern const vfs::stream_method_set      pairedsocket_stream_methods;
	extern const vfs::general_method_set     pairedsocket_general_methods;
	extern const vfs::filehandle_method_set  pairedsocket_methods;
	
	void close_paired_socket( vfs::filehandle* that );
	
	vfs::filehandle_ptr new_paired_socket( vfs::stream&  input,
	                                       vfs::stream&  output,
	                                       bool          nonblocking );
	
}

#endif
