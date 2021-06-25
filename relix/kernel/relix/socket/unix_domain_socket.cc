/*
	unix_domain_socket.cc
	---------------------
*/

#include "relix/socket/unix_domain_socket.hh"

// POSIX
#include <sys/stat.h>
#include <sys/un.h>

// Standard C
#include <string.h>

// Standard C++
#include <map>

// plus
#include "plus/string.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/functions/file-tests.hh"
#include "vfs/functions/pathname.hh"
#include "vfs/primitives/mknod.hh"

// relix-kernel
#include "relix/fs/resolve_path_at.hh"
#include "relix/socket/paired_socket.hh"

// Standard C
#include <signal.h>

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/enum/poll_result.hh"
#include "vfs/filehandle/functions/nonblocking.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/stream_method_set.hh"
#include "vfs/filehandle/methods/socket_method_set.hh"

// relix-kernel
#include "relix/api/try_again.hh"


namespace relix
{
	
	namespace p7 = poseven;
	
	
	struct unix_domain_socket_connection
	{
		vfs::filehandle*  server;
		vfs::filehandle*  client;
		
		unix_domain_socket_connection() : server(), client()
		{
		}
	};
	
	typedef std::map< plus::string, unix_domain_socket_connection > Map;
	
	typedef Map::iterator  Iter;
	
	static Map the_pending_connections;
	
	enum
	{
		State_none,
		State_bound,
		State_listening,
		State_connected,
	};
	
	struct unix_domain_extra : pairedsocket_extra
	{
		int   state;
		
		const plus::string*             key;
		unix_domain_socket_connection*  connection;
	};
	
	
	static vfs::node_ptr get_node( const sockaddr& addr, socklen_t len )
	{
		const sockaddr_un& un = (const sockaddr_un&) addr;
		
		const char* p = un.sun_path;
		
		return resolve_path_at( -100, p, strlen( p ) );
	}
	
	static void unix_domain_bind( vfs::filehandle*  that,
	                              const sockaddr*   addr,
	                              socklen_t         len )
	{
		unix_domain_extra& extra = *(unix_domain_extra*) that->extra();
		
		if ( extra.state >= State_bound )
		{
			p7::throw_errno( EINVAL );
		}
		
		vfs::node_ptr node = get_node( *addr, len );
		
		if ( exists( *node ) )
		{
			p7::throw_errno( EEXIST );
		}
		
		plus::string path = pathname( *node );
		
		Iter it = the_pending_connections.find( path );
		
		if ( it != the_pending_connections.end() )
		{
			p7::throw_errno( EADDRINUSE );
		}
		
		mknod( *node, S_IFSOCK, 0 );
		
		Map::value_type value( path, unix_domain_socket_connection() );
		
		it = the_pending_connections.insert( value ).first;
		
		extra.key        = &it->first;
		extra.connection = &it->second;
		
		extra.state = State_bound;
	}
	
	static void unix_domain_listen( vfs::filehandle* that, int backlog )
	{
		unix_domain_extra& extra = *(unix_domain_extra*) that->extra();
		
		if ( extra.state == State_bound )
		{
			extra.connection->server = that;
			
			extra.state = State_listening;
		}
	}
	
	static vfs::filehandle_ptr unix_domain_accept( vfs::filehandle*  that,
	                                               sockaddr*         addr,
	                                               socklen_t*        len )
	{
		unix_domain_extra& extra = *(unix_domain_extra*) that->extra();
		
		if ( extra.state != State_listening )
		{
			p7::throw_errno( EINVAL );
		}
		
		unix_domain_socket_connection& connection = *extra.connection;
		
		while ( connection.client == NULL )
		{
			try_again( is_nonblocking( *that ) );
		}
		
		void* client_extra = connection.client->extra();
		unix_domain_extra& client = *(unix_domain_extra*) client_extra;
		
		boost::intrusive_ptr< vfs::stream > up  ( new vfs::stream );
		boost::intrusive_ptr< vfs::stream > down( new vfs::stream );
		
		using vfs::filehandle;
		
		filehandle* result = new filehandle( NULL,
		                                     O_RDWR,
		                                     &pairedsocket_methods,
		                                     sizeof (pairedsocket_extra),
		                                     &close_paired_socket );
		
		pairedsocket_extra& server = *(pairedsocket_extra*) result->extra();
		
		client.output = server.input = up  .get();
		server.output = client.input = down.get();
		
		intrusive_ptr_add_ref( up  .get() );
		intrusive_ptr_add_ref( up  .get() );
		intrusive_ptr_add_ref( down.get() );
		intrusive_ptr_add_ref( down.get() );
		
		if ( len != NULL )
		{
			memset( addr, '\0', *len );
			
			addr->sa_family = AF_UNIX;
		}
		
		client.state = State_connected;
		
		connection.client = NULL;
		
		return result;
	}
	
	static void unix_domain_connect( vfs::filehandle*  that,
	                                 const sockaddr*   server,
	                                 socklen_t         len )
	{
		unix_domain_extra& extra = *(unix_domain_extra*) that->extra();
		
		if ( extra.state == State_connected )
		{
			p7::throw_errno( EISCONN );
		}
		
		if ( extra.state != State_none )
		{
			p7::throw_errno( EINVAL );
		}
		
		vfs::node_ptr node = get_node( *server, len );
		
		const mode_t mode = node->filemode();
		
		if ( mode == 0 )
		{
			p7::throw_errno( ENOENT );
		}
		
		if ( !S_ISSOCK( mode ) )
		{
			p7::throw_errno( ENOTSOCK );
		}
		
		plus::string path = pathname( *node );
		
		Iter it = the_pending_connections.find( path );
		
		if ( it == the_pending_connections.end()  ||  !it->second.server )
		{
			p7::throw_errno( ECONNREFUSED );
		}
		
		unix_domain_socket_connection& connection = it->second;
		
		while ( connection.client != NULL )
		{
			try_again( is_nonblocking( *that ) );
		}
		
		connection.client = that;
		
		while ( extra.state != State_connected )
		{
			try_again( is_nonblocking( *that ) );
		}
	}
	
	static const sockaddr* unix_domain_getsockname( vfs::filehandle* that )
	{
		return NULL;
	}
	
	static const sockaddr* unix_domain_getpeername( vfs::filehandle* that )
	{
		unix_domain_extra& extra = *(unix_domain_extra*) that->extra();
		
		if ( extra.state != State_connected )
		{
			p7::throw_errno( ENOTCONN );
		}
		
		return NULL;
	}
	
	static const vfs::socket_method_set unix_domain_socket_methods =
	{
		&unix_domain_bind,
		&unix_domain_listen,
		&unix_domain_accept,
		&unix_domain_connect,
		&pairedsocket_shutdown,
		&unix_domain_getsockname,
		&unix_domain_getpeername,
		&pairedsocket_conveying,
	};
	
	static const vfs::filehandle_method_set unix_domain_methods =
	{
		NULL,
		&unix_domain_socket_methods,
		&pairedsocket_stream_methods,
		&pairedsocket_general_methods,
	};
	
	static void close_unix_domain_socket( vfs::filehandle* that )
	{
		unix_domain_extra& extra = *(unix_domain_extra*) that->extra();
		
		if ( extra.state == State_connected )
		{
			close_paired_socket( that );
		}
		else if ( extra.state != State_none )
		{
			the_pending_connections.erase( *extra.key );
		}
	}
	
	vfs::filehandle_ptr new_unix_domain_socket( int flags )
	{
		using vfs::filehandle;
		
		filehandle* result = new filehandle( NULL,
		                                     flags,
		                                     &unix_domain_methods,
		                                     sizeof (unix_domain_extra),
		                                     &close_unix_domain_socket );
		
		unix_domain_extra& extra = *(unix_domain_extra*) result->extra();
		
		extra.state = State_none;
		
		extra.key        = NULL;
		extra.connection = NULL;
		
		return result;
	}
	
}
