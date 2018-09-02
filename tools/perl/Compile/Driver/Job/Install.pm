package Compile::Driver::Job::Install;

use     Compile::Driver::Job ;
@ISA = 'Compile::Driver::Job';

use warnings FATAL => 'all';
use strict;


sub new
{
	my $class = shift;
	
	my %self = @_;
	
	return bless \%self, $class;
}

sub input_files
{
	return;
}

sub up_to_date
{
	my $self = shift;
	
	my $out = $self->{DEST};
	
	-f $out or return 0;
	
	my $out_date = (stat _)[9];
	
	my @stat = stat $self->{ORIG};
	
	return @stat  &&  $stat[9] <= $out_date;
}

sub tool_name
{
	return "cp";
}

sub command
{
	my $self = shift;
	
	my $orig = $self->{ORIG};
	my $dest = $self->{DEST};
	
	return $self->tool_name, $orig, $dest;
}

1;
