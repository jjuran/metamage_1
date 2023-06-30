#!/usr/bin/perl

#use warnings FATAL => 'all';
#use strict;

my @ignored_routines =
qw
(
	abort
	atexit
	clock
	exit
	getcwd
	getenv
	gmtime
	localtime
	perror
	raise
	remove
	rename
	signal
	strerror
	system
	time
	utime
	__atexit
	__exit
	__stdio_exit
	
	__read_console
	__write_console
	
	__open_file
	__read_file
	__write_file
	__position_file
	__close_file
	
	__convert_from_newlines
	__convert_to_newlines
	
	malloc
	calloc
	realloc
	free
	
	chdir
	close
	creat
	fcntl
	fdopen
	fileno
	fstat
	isatty
	lseek
	mkdir
	open
	read
	rmdir
	sleep
	stat
	ttyname
	uname
	unlink
	write
);

my @ignored_symbols = @ignored_routines;

my @loser_libs =
qw
(
	abort_exit.c
	alloc.c
	buffer_io.c
	file_io.c
	misc_io.c
	signal.c
	string.c
	sysenv.c
	time.c
	console_io.mac.c
	fcntl.mac.c
	file_io.mac.c
	unistd.mac.c
	unix.mac.c
	utime.mac.c
	utsname.c
	stat.mac.c
);

my @winner_libs =
map { ":?$_" }
qw
(
	clock.cc.cpp
	environ.cc.cpp
	Library.cc.cpp
	MSL_buffer_io.c.cpp
	MSL_exit.c.cpp
	MSL_file_io.c.cpp
	nomalloc.c.cpp
	readonly-environ.cc.cpp
	SIOUXConsole.cc.cpp
	syscalls.c.cpp
	time.c.cpp
	time.cc.cpp
	ttyname.cc.cpp
	utime.cc.cpp
);

my $warning_template = << '[END]';
### MWLink68K Linker Warning:
# ignored: ‘SYMBOL’ in LOSER
# previously defined in WINNER
[END]

my %substitute_for =
(
	SYMBOL => \@ignored_symbols,
	LOSER  => \@loser_libs,
	WINNER => \@winner_libs,
);

my $warning_regex = $warning_template;

$warning_regex =~ s{ ( [()] ) }{\\$1}gx;  # Replace ( and ) with \( and \)

foreach my $macro ( keys %substitute_for )
{
	my $alternates = join '|', @{ $substitute_for{$macro} };
	$warning_regex =~ s{$macro}{(?:$alternates)};
}

$warning_regex =~ s{ ( [.] ) }{\\$1}gx;  # Replace . with \.

my $filter = eval << '[END]';
sub
{
	s{$warning_regex}{}g;
};
[END]

local $/ = undef;

my $link_output = <>;

$filter->() for $link_output;

print $link_output;
