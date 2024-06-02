/*
	console.cc
	----------
*/

#include "relix/fs/console.hh"

// POSIX
#include <errno.h>
#include <fcntl.h>

// iota
#include "iota/strings.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/primitives/get_file.hh"
#include "vfs/filehandle/primitives/read.hh"
#include "vfs/filehandle/primitives/write.hh"
#include "vfs/functions/file-tests.hh"
#include "vfs/functions/resolve_pathname.hh"
#include "vfs/primitives/hardlink.hh"
#include "vfs/primitives/open.hh"
#include "vfs/primitives/opendir.hh"
#include "vfs/primitives/remove.hh"
#include "vfs/primitives/symlink.hh"
#include "vfs/primitives/touch.hh"

// relix-kernel
#include "relix/api/root.hh"


namespace relix
{
namespace console
{

static
ssize_t spew( const vfs::node& file, const char* buffer, std::size_t length )
{
	return write( *open( file, O_WRONLY | O_TRUNC, 0 ), buffer, length );
}

static
ssize_t append( const vfs::node& file, const char* buffer, std::size_t length )
{
	return write( *open( file, O_WRONLY | O_APPEND, 0 ), buffer, length );
}

static
void make_window( vfs::filehandle& port_dir )
{
	vfs::node_ptr root = relix::root();
	
	vfs::node_ptr port = get_file( port_dir );
	
	const vfs::node& cwd = *port;
	
	vfs::node_ptr window = resolve_relative_path( *root, STR_LEN( "window" ), cwd );
	
	if ( exists( *window ) )
	{
		return;
	}
	
	vfs::node_ptr view = resolve_relative_path( *root, STR_LEN( "view" ), cwd );
	
	if ( exists( *view ) )
	{
		remove( *view );
		
		view = resolve_relative_path( *root, STR_LEN( "view" ), cwd );
	}
	
	spew( *resolve_relative_path( *root, STR_LEN( "title" ), cwd ), STR_LEN( "System Console" "\n" ) );
	
	spew( *resolve_relative_path( *root, STR_LEN( "size" ),  cwd ), STR_LEN( "495x272" "\n" ) );
	
	touch( *window );
	
	spew( *resolve_relative_path( *root, STR_LEN( "w/text-font" ), cwd ), STR_LEN( "4" "\n" ) );
	spew( *resolve_relative_path( *root, STR_LEN( "w/text-size" ), cwd ), STR_LEN( "9" "\n" ) );
	
	hardlink( *vfs::resolve_absolute_path( *root, STR_LEN( "/gui/new/scrollframe" ) ), *view );
	
	vfs::node_ptr subview = resolve_relative_path( *root, STR_LEN( "v/view" ), cwd );
	
	hardlink( *vfs::resolve_absolute_path( *root, STR_LEN( "/gui/new/frame" ) ), *subview );
	
	vfs::node_ptr subsubview = resolve_relative_path( *root, STR_LEN( "v/v/view" ), cwd );
	
	hardlink( *vfs::resolve_absolute_path( *root, STR_LEN( "/gui/new/textedit" ) ), *subsubview );
	
	symlink( *resolve_relative_path( *root, STR_LEN( "v/target" ), cwd ), "v/v" );
	
	spew( *resolve_relative_path( *root, STR_LEN( "v/vertical"  ), cwd ), STR_LEN( "1" "\n" ) );
	spew( *resolve_relative_path( *root, STR_LEN( "v/v/padding" ), cwd ), STR_LEN( "4" "\n" ) );
}

static
vfs::node_ptr get_console_window()
{
	static const vfs::filehandle_ptr the_port = opendir( *vfs::resolve_absolute_path( *root(), STR_LEN( "/gui/new/port" ) ) );
	
	make_window( *the_port );
	
	return get_file( *the_port );
}

static
vfs::node_ptr get_console_text()
{
	vfs::node_ptr text = resolve_relative_path( *root(), STR_LEN( "v/v/v/text" ), *get_console_window() );
	
	return text;
}

long log( const char* buffer, unsigned long n )
{
	try
	{
		return append( *get_console_text(), buffer, n );
	}
	catch ( ... )
	{
	}
	
	errno = EIO;
	
	return -1;
}

}  // namespace console
}  // namespace relix
