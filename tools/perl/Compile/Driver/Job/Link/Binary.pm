package Compile::Driver::Job::Link::Binary;

use     Compile::Driver::Job ;
@ISA = 'Compile::Driver::Job';

use warnings FATAL => 'all';
use strict;


*lib_pathname = \&Compile::Driver::Job::lib_pathname;


sub new
{
	my $class = shift;
	
	my %self = @_;
	
	return bless \%self, $class;
}

sub input_files
{
	my $self = shift;
	
	my $module = $self->{FROM};
	
	my $objs = $self->{OBJS};
	my $preq = $self->{PREQ};
	
	my @prereqs = grep { $module->get( $_ )->is_static_lib } @$preq;
	
	my @libs = map { lib_pathname( $module->target, $_ ) } @prereqs;
	
	@libs = reverse @libs;
	
	return @$objs, @libs;
}

sub imports
{
	my $self = shift;
	
	my $module = $self->{FROM};
	
	my $preq = $self->{PREQ};
	
	return map { $module->get( $_ )->imports } @$preq;
}

sub tool_name
{
	return "c++";
}

sub has_framework
{
	my ( $name ) = @_;
	
	return -d "/System/Library/Frameworks/$name.framework";
}

sub command
{
	my $self = shift;
	
	my $module = $self->{FROM};
	
	my @mode;
	my $dest = $self->{DEST};
	
	my $conf = $module->{CONF};
	
	my @arch = $conf->arch_option;
	
	my @frameworks;
	
	if ( $conf->is_apple_gcc )
	{
		my @names = @{$module->{DESC}{DATA}{frameworks} || []};
		
		if ( my $bundle_type = $module->bundle_type )
		{
			if ( $bundle_type ne "app" )
			{
				@mode = "-bundle";
				
				if ( $bundle_type eq "qlgenerator" )
				{
					push @names, (has_framework "CoreGraphics")
					             ? ("CoreFoundation", "CoreGraphics")
					             : "ApplicationServices";
					
					push @names, "QuickLook";
				}
			}
		}
		
		@names = "Carbon"  if @names == 0;
		
		push @frameworks, map { -framework => $_ } @names;
	}
	
	return $self->tool_name, @mode, -o => $dest, @arch, @frameworks;
}

1;
