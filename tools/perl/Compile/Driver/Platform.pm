package Compile::Driver::Platform;

use warnings FATAL => 'all';
use strict;

my %Bit_for_label = qw
(
	68k 0
	ppc 1
	x86 2
	arm 12
	
	near   13
	a4     3
	a5     4
	cfm    5
	mach-o 6
	
	blue   8
	carbon 9
	
	mac  10
	unix 11
);

sub mask_for_values
{
	my @values = map { s/_?64//; $_ } grep { ! /^ m\d\d $/x } @_;
	
	my $mask = 0;
	
	foreach my $label ( @values )
	{
		my $bit = $Bit_for_label{ $label };
		
		die "Bad platform '$label'\n"  if !defined $bit;
		
		$mask |= 1 << $bit;
	}
	
	return $mask;
}

1;
