package Compile::Driver::Options;

use warnings FATAL => 'all';
use strict;

my %Flag_for_option = qw
(
	v verbose
	i install
);

my %Spec_for_option = qw
(
	P ppc
	8 x86
	
	3 m32
	W m64
	
	R opt
	X sym
);

my %Flags;
my @Specs;

sub verbose
{
	return $Flags{ verbose };
}

sub installing
{
	return $Flags{ install };
}

sub specs
{
	return @Specs;
}

sub set_option
{
	my ( $option ) = @_;
	
	if ( my $flag = $Flag_for_option{ $option } )
	{
		$Flags{ $flag } = 1;
		
		return;
	}
	
	my $spec = $Spec_for_option{ $option } or die "No such option '$option'\n";
	
	push @Specs, $spec;
}

sub set_options
{
	my ( @argv ) = @_;
	
	my @args;
	
	while ( my $arg = shift )
	{
		if ( $arg !~ m{^ - }x )
		{
			push @args, $arg;
		}
		elsif ( $arg !~ m{^ -- }x )
		{
			my @chars = split "", $arg;
			
			shift @chars;  # lose the '-'
			
			set_option( $_ ) foreach @chars;
		}
		else
		{
			die "No long options yet\n";
		}
	}
	
	return @args;
}

1;
