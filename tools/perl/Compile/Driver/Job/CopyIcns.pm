package Compile::Driver::Job::CopyIcns;

use     Compile::Driver::Job::Copy ;
@ISA = 'Compile::Driver::Job::Copy';

use warnings FATAL => 'all';
use strict;


sub new
{
	my $class = shift;
	
	my %self = @_;
	
	return bless \%self, $class;
}

sub tool_name
{
	return qw( minivx v/bin/icns-cp-trimmed.vx );
}

1;
