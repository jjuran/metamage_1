#!/usr/bin/perl

use warnings;
use strict;

my @ignored_routines =
qw
(
	abort
	getenv
	perror
	raise
	rename
	signal
	strerror
	system
	__atexit
	__exit
	
	__write_console
	
	__convert_from_newlines
	__convert_to_newlines
	
	getc
	putc
	
	DisposeControlColorUPP
	NewControlColorUPP
	InvokeControlColorUPP
);

my @ignored_data = qw( errno );

my @ignored_symbols = ( @ignored_routines, @ignored_data );

my @symbol_types = qw( code descriptor data );

my @loser_libs = ('MSL C.PPC.Lib', 'MSL C.Carbon.Lib', 'ControlsLib' );
my @winner_libs =
qw
(
	MSL-fixup.lib
	Kerosene-User.lib
	Kerosene-Library.lib
	PascalPreCarbonUPPGlue.o
	OTXTILib
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

