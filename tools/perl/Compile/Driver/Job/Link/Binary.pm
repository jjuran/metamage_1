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

my $macOS_SDK_version;

sub macOS_SDK_version
{
	return $macOS_SDK_version if defined $macOS_SDK_version;
	
	$macOS_SDK_version = 0;
	
	my $header = "/usr/include/Availability.h";
	
	if ( -f $header )
	{
		if ( my $last = `grep "^#define __MAC_" $header | tail -n1` )
		{
			my ($version) = $last =~ m{(\d+)$};
			
			$macOS_SDK_version = $version || 0;
		}
	}
	
	return $macOS_SDK_version;
}

sub command
{
	my $self = shift;
	
	my $module = $self->{FROM};
	
	my @w;
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
		
		if ( ! exists $ENV{SDKROOT} )
		{
			my $vers = int macOS_SDK_version() / 100;
			
			if ( $vers == 1013 )
			{
				if ( @names  ||  $arch[ 1 ] eq 'i386' )
				{
					@w = '-w';
				}
			}
			elsif ( $vers <= 1005  &&  ! @names )
			{
				# Apple's g++ 4.0 and earlier don't work with Boost.Atomic
				# for 32-bit builds, so instead we use OTAtomicAdd32() from
				# CoreServices to implement vxo::reference_count_t.
				
				# The Mac OS X 10.6 SDK includes g++ 4.2 and clang, so the
				# issue doesn't occur, but prior to that, we need to link
				# with CoreServices to make OTAtomicAdd32() available.
				
				@names = "CoreServices";
			}
		}
		
		push @frameworks, map { -framework => $_ } @names;
		
		push @mode, "-dead_strip";
	}
	
	return $self->tool_name, @w, @mode, -o => $dest, @arch, @frameworks;
}

1;
