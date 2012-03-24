package Compile::Driver::Job::Link::Archive;

use     Compile::Driver::Job ;
@ISA = 'Compile::Driver::Job';

use warnings;
use strict;


*lib_filename = \&Compile::Driver::Job::lib_filename;
*up_to_date   = \&Compile::Driver::Job::up_to_date;


sub new
{
	my $class = shift;
	
	my %self = @_;
	
	return bless \%self, $class;
}

sub command
{
	my $self = shift;
	
	my $module = $self->{FROM};
	
	my $objs = $self->{OBJS};
	my $dest = $self->{DEST};
	
	$self->{PATH} = lib_filename( $module->name );
	
	return  if up_to_date( $dest, @$objs );
	
	unlink( $dest );
	
	return qw( ar rcs ), $dest, @$objs;
}

1;

