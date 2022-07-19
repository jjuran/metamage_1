/*
	exhibit.cc
	----------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>

// Standard C
#include <errno.h>
#include <signal.h>
#include <stdlib.h>

// more-posix
#include "more/perror.hh"

// command
#include "command/get_option.hh"


#define EXHIBIT_DISPLAY   "EXHIBIT_DISPLAY"
#define EXHIBIT_INTERACT  "EXHIBIT_INTERACT"

#define DISPLAY   "display"
#define INTERACT  "interact"

#define PROGRAM  "exhibit"

#define USAGE  \
"usage: " PROGRAM " --raster <raster-path> <raster-author-program>\n"  \
"       where raster-path is a raster file\n"

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )


enum
{
	Opt_title   = 't',
	Opt_magnify = 'x',
	
	Opt_last_byte = 255,
	
	Opt_raster,
	Opt_events_fd,
	Opt_events_fifo,
};

static command::option options[] =
{
	{ "raster",  Opt_raster,  command::Param_required },
	{ "title",   Opt_title,   command::Param_required },
	{ "magnify", Opt_magnify, command::Param_required },
	{ "events-fd", Opt_events_fd, command::Param_required },
	{ "events-fifo", Opt_events_fifo, command::Param_required },
	{ NULL }
};


static const char* raster_path;
static const char* fifo_path;
static const char* title;

static const char* magnifier = "1";

static int events_fd = 0;

static pid_t viewer_pid = 0;  // either interact or display
static pid_t author_pid = 0;

static sig_atomic_t child_terminated = 0;


static inline
const char* display()
{
	if ( const char* var = getenv( EXHIBIT_DISPLAY ) )
	{
		return var;
	}
	
	return DISPLAY;
}

static inline
const char* interact()
{
	if ( const char* var = getenv( EXHIBIT_INTERACT ) )
	{
		return var;
	}
	
	return INTERACT;
}

static
void sigchld( int )
{
	++child_terminated;
}

static inline
void report_error( const char* path, int err )
{
	more::perror( PROGRAM, path, err );
}

static
char* const* get_options( char** argv )
{
	int opt;
	
	++argv;  // skip arg 0
	
	while ( (opt = command::get_option( (char* const**) &argv, options )) > 0 )
	{
		using command::global_result;
		
		switch ( opt )
		{
			case Opt_raster:
				raster_path = global_result.param;
				break;
			
			case Opt_title:
				title = global_result.param;
				break;
			
			case Opt_magnify:
				magnifier = global_result.param;
				break;
			
			case Opt_events_fd:
				events_fd = atoi( global_result.param );
				break;
			
			case Opt_events_fifo:
				fifo_path = global_result.param;
				break;
			
			default:
				break;
		}
	}
	
	return argv;
}

static
void exec_or_exit( const char* const argv[] )
{
	execvp( *argv, (char**) argv );
	
	const int saved_errno = errno;
	
	report_error( *argv, saved_errno );
	
	_exit( saved_errno == ENOENT ? 127 : 126 );
}

static
void exec_or_exit_endpoint( const char* const  argv[],
                            int                our_fd,
                            int                other_fd,
                            int                input_fd = STDIN_FILENO,
                            int                output_fd = STDOUT_FILENO )
{
	dup2( our_fd, input_fd  );
	dup2( our_fd, output_fd );
	
	close( our_fd   );
	close( other_fd );
	
	exec_or_exit( argv );
}

static
void init_raster( const char* raster_path )
{
	pid_t pid = fork();
	
	if ( pid < 0 )
	{
		report_error( "fork", errno );
		exit( 1 );
	}
	
	if ( pid == 0 )
	{
		const char* argv[] = { "skif", "init", raster_path, NULL };
		
		exec_or_exit( argv );
	}
	
	int wait_status;
	pid_t child_pid = wait( &wait_status );
	
	if ( child_pid < 0 )
	{
		report_error( "wait", errno );
		exit( 1 );
	}
	
	if ( WIFSIGNALED( wait_status ) )
	{
		WARN( "skif init: fatal signal" );
		exit( 1 );
	}
	
	if ( ! WIFEXITED( wait_status ) )
	{
		WARN( "skif init: non-signal, non-exit termination (I'm confused)" );
		exit( 1 );
	}
	
	if ( const int exit_status = WEXITSTATUS( wait_status ) )
	{
		WARN( "skif init failed" );
		exit( 1 );
	}
}

static
void launch_viewer( const char* raster_path )
{
	viewer_pid = fork();
	
	if ( viewer_pid < 0 )
	{
		report_error( "fork", errno );
		exit( 1 );
	}
	
	if ( viewer_pid == 0 )
	{
		const char* argv[ 7 ] = { display(), "-x", "1" };
		
		argv[ 2 ] = magnifier;
		
		const char** p = argv + 3;
		
		if ( title )
		{
			*p++ = "-t";
			*p++ = title;
		}
		
		*p++ = raster_path;
		
		exec_or_exit( argv );
	}
}

static
void launch_interactive( char* const* args )
{
	/*
		The author process reads from the front end of a socket
		and the viewer process writes to the back end.
		
		Alternatively, we can create and use a named pipe instead.
	*/
	
	int author_fd;
	int viewer_fd;
	
	if ( fifo_path == NULL )
	{
		int socket_fds[ 2 ];
		
		int nok = socketpair( PF_UNIX, SOCK_STREAM, 0, socket_fds );
		
		if ( nok )
		{
			report_error( "socketpair", errno );
			exit( 1 );
		}
		
		author_fd = socket_fds[ 0 ];
		viewer_fd = socket_fds[ 1 ];
	}
	else
	{
		struct stat st;
		int nok = stat( fifo_path, &st );
		
		/*
			There are four possible outcomes:
			
			  * a FIFO exists
			  * something else exists
			  * nothing exists
			  * some other error occurred
		*/
		
		if ( nok == 0 )
		{
			/*
				Something exists.  If it's not a FIFO, the user erred and we
				shouldn't continue, lest we risk deleting something important.
			*/
			
			if ( ! S_ISFIFO( st.st_mode ) )
			{
				more::perror( PROGRAM, fifo_path, "not a named pipe" );
				exit( 1 );
			}
			
			/*
				A FIFO exists.  Delete it anyway and create a fresh one, so
				we're sure that we won't be interfering with an existing set
				of processes.
				
				No need to error-check:  If unlink() fails, then mkfifo()
				will fail below.
			*/
			
			unlink( fifo_path );
		}
		else if ( errno != ENOENT )
		{
			report_error( fifo_path, errno );
			exit( 1 );
		}
		
		nok = mkfifo( fifo_path, 0666 );
		
		if ( nok )
		{
			report_error( "fifo_path", errno );
			exit( 1 );
		}
		
		int fifo_reader = open( fifo_path, O_RDONLY | O_NONBLOCK );
		
		if ( fifo_reader < 0 )
		{
			report_error( "fifo_path", errno );
			exit( 1 );
		}
		
		int fifo_writer = open( fifo_path, O_WRONLY );
		
		if ( fifo_writer < 0 )
		{
			report_error( "fifo_path", errno );
			exit( 1 );
		}
		
		close( fifo_reader );
		
		fifo_reader = open( fifo_path, O_RDONLY );
		
		if ( fifo_reader < 0 )
		{
			report_error( "fifo_path", errno );
			exit( 1 );
		}
		
		/*
			Since we're using a FIFO, have the author read from it
			and the viewer write to it.
		*/
		
		author_fd = fifo_reader;
		viewer_fd = fifo_writer;
	}
	
	viewer_pid = fork();
	
	if ( viewer_pid < 0 )
	{
		report_error( "fork", errno );
		exit( 1 );
	}
	
	if ( viewer_pid == 0 )
	{
		const char* argv[ 8 ] = { interact(), "--raster", raster_path, "-x" };
		
		argv[ 4 ] = magnifier;
		
		if ( title )
		{
			argv[ 5 ] = "-t";
			argv[ 6 ] = title;
		}
		
		exec_or_exit_endpoint( argv, viewer_fd, author_fd );
	}
	
	char ready;
	ssize_t n = read( author_fd, &ready, 1 );
	
	if ( n < 0 )
	{
		report_error( "waiting for interactive viewer", errno );
		exit( 1 );
	}
	
	if ( n == 0  ||  ready != '\0' )
	{
		// Interactive didn't send ready byte.
		WARN( "interactive viewer not ready" );
		exit( 1 );
	}
	
	author_pid = fork();
	
	if ( author_pid < 0 )
	{
		report_error( "fork", errno );
		exit( 1 );
	}
	
	if ( author_pid == 0 )
	{
		const int ours  = author_fd;
		const int other = viewer_fd;
		
		const int keep_output = -1;  // Don't clobber stdout
		
		exec_or_exit_endpoint( args, ours, other, events_fd, keep_output );
	}
	
	close( author_fd );
	close( viewer_fd );
}

int main( int argc, char** argv )
{
	if ( argc == 0 )
	{
		return 0;
	}
	
	char* const* args = get_options( argv );
	
	int argn = argc - (args - argv);
	
	if ( raster_path == NULL )
	{
		write( STDERR_FILENO, STR_LEN( USAGE ) );
		return 2;
	}
	
	if ( argn > 0 )
	{
		// Only init the sync relay if we have an interactive author.
		init_raster( raster_path );
	}
	
	struct sigaction action;
	
	action.sa_handler = &sigchld;
	action.sa_flags   = 0;
	
	sigemptyset( &action.sa_mask );
	
	sigaction( SIGCHLD, &action, NULL );
	
	if ( argn == 0 )
	{
		// No author program: just display the raster image.
		launch_viewer( raster_path );
		
		int wait_status;
		pid_t child = wait( &wait_status );
	}
	else
	{
		signal( SIGINT, SIG_IGN );
		
		launch_interactive( args );
	}
	
	int wait_status;
	
	pid_t child = wait( &wait_status );
	
	if ( viewer_pid )  kill( viewer_pid, SIGTERM );
	if ( author_pid )  kill( author_pid, SIGTERM );
	
	child = wait( &wait_status );
	
	return 0;
}
