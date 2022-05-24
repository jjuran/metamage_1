package Compile::Driver::Job::Rez;

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
	
	my $r = $self->{SRCS};
	
	return @$r;
}

our $Rez;

sub tool_name
{
	if ( ! defined $Rez )
	{
		$Rez = "/Developer/Tools/Rez";
		
		$Rez = "Rez"  unless -e $Rez;
	}
	
	return $Rez;
}

sub command
{
	my $self = shift;
	
	my $dest = $self->{DEST};
	
	my @d = qw( -d aeut_RezTemplateVersion=1 );
	
	return $self->tool_name, '-useDF', @d, "Carbon.r", -o => $dest;
}

1;
