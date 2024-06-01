package Compile::Driver::Job::InfoPList;

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
	my $self = shift;
	
	my $path = $self->{ORIG};
	
	return $path;
}

sub tool_name
{
	return "bin/make-info-plist";
}

sub command
{
	my $self = shift;
	
	my $dest = $self->{DEST};
	
	my @args = ($dest, $self->tool_name);
	
	return qw(minivx -e), 'dup2(rewrite argv[1], fd 1); run argv[2..3]', @args;
}

1;
