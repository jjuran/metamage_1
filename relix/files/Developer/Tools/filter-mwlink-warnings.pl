#!/usr/bin/perl

#use warnings FATAL => 'all';
#use strict;

my @ignored_routines =
qw
(
	abort
	clock
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
	_getcwd
	__atexit
	__exit
	
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
	
	getc
	putc
	
	OTInetMailExchange
);

my @ignored_data = qw( errno );

my @ignored_symbols = ( @ignored_routines, @ignored_data );

my @symbol_types = qw( code descriptor data );

my @loser_libs =
(
	'MSL C.PPC.Lib',
	'MSL C.Carbon.Lib',
	'MSL-fixup.lib',
	'OTXTILib',
	'Apple;Carbon;Networking',
);
my @winner_libs =
map { ":?$_" }
qw
(
	Abort.cc
	Genie/Abort.cc.cpp
	:Genie/Abort.cc
	OTInetMailExchange.cc
	OTInetMailExchange.cc.cpp
	dlmalloc.c
	dlmalloc.c.cpp
	nomalloc.c
	nomalloc.c.cpp
	readonly-environ.cc
	readonly-environ.cc.cpp
	stacked-environ.lib
	syscalls.c
	Kerosene-Library.lib
	MSL-fixup.lib
	openbsd-libc.lib
	PosixLib.lib
	sfio.lib
);

my $warning_template = << '[END]';
### MWLinkPPC Linker Warning:
#   Link Warning : ignored 'SYMBOL' (TYPE) in LOSER
#   Previously defined in WINNER
[END]

my %substitute_for =
(
	SYMBOL => \@ignored_symbols,
	TYPE   => \@symbol_types,
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
