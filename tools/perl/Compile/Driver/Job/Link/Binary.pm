package Compile::Driver::Job::Link::Binary;

use     Compile::Driver::Job ;
@ISA = 'Compile::Driver::Job';

use warnings;
use strict;


*lib_pathname = \&Compile::Driver::Job::lib_pathname;


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
	my $preq = $self->{PREQ};
	my $dest = $self->{DEST};
	
	my @prereqs = grep { $module->get( $_ )->is_static_lib } @$preq;
	
	my @libs = map { lib_pathname( $module->target, $_ ) } @prereqs;
	
	return  if $self->up_to_date( @$objs, @libs );
	
	my $conf = $module->{CONF};
	
	@libs = reverse @libs;
	
	my @arch = $conf->arch_option;
	
	if ( $conf->is_apple_gcc )
	{
		push @libs, -framework => "Carbon";
	}
	
	return qw( g++ -o ), $dest, @arch, @$objs, @libs;
}

1;

