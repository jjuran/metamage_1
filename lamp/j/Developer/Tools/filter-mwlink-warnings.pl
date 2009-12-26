#!/usr/bin/perl

use warnings;
use strict;

my @ignored_routines =
qw
(
	abort
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
	
	__write_console
	
	__open_file
	__read_file
	__write_file
	__position_file
	__close_file
	
	__convert_from_newlines
	__convert_to_newlines
	
	getc
	putc
	
	DisposeControlColorUPP
	NewControlColorUPP
	InvokeControlColorUPP
	
	OTInetMailExchange
);

my @ignored_data = qw( errno );

my @ignored_symbols = ( @ignored_routines, @ignored_data );

my @symbol_types = qw( code descriptor data );

my @loser_libs =
(
	'MSL C.PPC.Lib',
	'MSL C.Carbon.Lib',
	'ControlsLib',
	'Apple;Carbon;Networking',
);
my @winner_libs =
qw
(
	:KSIOUXConsole.cc
	Abort.cc
	KSIOUXConsole.cc
	syscalls.c
	Kerosene-Library.lib
	Kerosene-User.lib
	MSL-fixup.lib
	openbsd-libc.lib
	OTXTILib
	PascalPreCarbonUPPGlue.o
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

