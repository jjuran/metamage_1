#!/usr/bin/perl -T

delete $ENV{PATH};

use warnings;
use strict;

my $AMIXER = '/usr/bin/amixer';

=pod
	
	mixctl - a sensible amixer front end
	
	This is a script that provides a sensible interface to amixer on ALSA-
	based systems, such as many distributions of GNU/Linux.  Instead of
	having to run
	
		amixer cset numid=3 2
	
	to route a Raspberry Pi's audio through HDMI, using mixctl, the command
	
		mixctl route hdmi
	
	is sufficient.  The user is spared having to memorize (a) the syntax of
	amixer, (b) the fact that the Raspberry Pi's route control is number 3,
	and (c) that HDMI is route number 2.  Also note that on another system,
	numid=3 might control the volume, for example.  When mixctl runs, it
	queries amixer first to locate controls, and then calls it again if it
	has to change something.
	
	When installing mixctl, it's recommended to remove the ".pl" suffix from
	the filename and disable the "warnings" and "strict" directives above.
	
=cut

die "amixer: $!\n" if ! -x $AMIXER;

my @ROUTES = qw( auto jack hdmi );
my %ROUTES = map { $ROUTES[ $_ ] => $_ } 0 .. $#ROUTES;

my %playback_control_map;

sub percentify
{
	my ($x, $min, $max) = @_;
	
	my $range = $max - $min;
	
	my $numeral = int( ($x - $min) / $range * 100 );
	
	return "$numeral%";
}

sub control_percentify
{
	my ( $control, $x ) = @_;
	
	my $attrs = $control->{ attrs };
	
	my $min = $attrs->{min};
	my $max = $attrs->{max};
	
	return percentify( $x, $min, $max );
}

sub route
{
	my ( $control, $arg ) = @_;
	
	if ( ! defined $arg )
	{
		my @values = @{ $control->{value} };
		
		return $ROUTES[ $values[0] ];
	}
	
	my $index = $ROUTES{ $arg };
	
	if ( ! defined $index )
	{
		die "Invalid route '$arg'\n";
	}
	
	my $numid = $control->{numid};
	
	`$AMIXER cset numid=$numid $index`;
	
	return;
}

sub switch
{
	my ( $control, $arg ) = @_;
	
	if ( ! defined $arg )
	{
		my @values = @{ $control->{value} };
		
		return $values[0];
	}
	
	my ($state) = $arg =~ m{^ (on | off) $}x
		or die "Invalid switch state '$arg'\n";
	
	my $numid = $control->{numid};
	
	`$AMIXER cset numid=$numid $state`;
	
	return;
}

sub volume
{
	my ( $control, $arg ) = @_;
	
	if ( ! defined $arg )
	{
		my @values = @{ $control->{value} };
		
		return map { control_percentify( $control, $_ ) } @values;
	}
	
	my ($percentage) = $arg =~ m{^ ([0-9]+)% $}x
		or die "Invalid volume percentage '$arg'\n";
	
	if ( $percentage > 100 )
	{
		die "Volume '$percentage%' exceeds 100%\n";
	}
	
	my $numid = $control->{numid};
	
	`$AMIXER cset numid=$numid $percentage%`;
	
	return;
}

my %commands =
(
	route  => \&route,
	switch => \&switch,
	volume => \&volume,
);

my $contents = `$AMIXER contents` or die "amixer: $!\n";

my @contents = split m{ (numid= .* \n (?: \s+ [;:|] .* \n )+)}x, $contents
	or die "Can't parse amixer output\n";

@contents = grep { $_ } @contents;

sub parse_control
{
	my ( $control ) = @_;
	
	my ($numid, $iface, $name, $attrs, $values) = $control =~ m
	{^
		numid=([0-9]+), iface=(\w+), name='([^']*)' \s+
		  ; \s+ (.*) \s+
		  : \s+ values=(.*) \s+
		  (?: \| \s+ (.*) \n )?
	$}x;
	
	my %attrs = map { m{^ (.*) = (.*) $}x } split /,/, $attrs;
	
	my @values = split /,/, $values;
	
	return
	{
		numid => $numid,
		iface => $iface,
		name  => $name,
		attrs => \ %attrs,
		value => \ @values,
	};
}

foreach my $control ( @contents )
{
	$control = parse_control( $control );
	
	my $name = $control->{name};
	
	if ( my ($which, $type) = $name =~ m{^ (.*) \s Playback \s (.*) $}x )
	{
		$playback_control_map{ lc $type } = $control;
	}
}

foreach my $type ( keys %playback_control_map )
{
	my $control = $playback_control_map{ $type };
	
	my $numid = $control->{ numid };
	my $attrs = $control->{ attrs };
	my $value = $control->{ value };
	
	my $values = join ", ", @$value;
	
	if ( $attrs->{type} eq "INTEGER" )
	{
		my $details = "";
		
		if ( $control->{name} =~ /Route$/ )
		{
			my @routes = map{ $ROUTES[ $_ ] } @$value;
			
			my $routes = join ", ", @routes;
			
			$details = " ($routes)";
		}
		else
		{
			my $min = $attrs->{min};
			my $max = $attrs->{max};
		
			my @percentages = map { percentify( $_, $min, $max ) } @$value;
		
			my $percentages = join ", ", @percentages;
		
			$details = " [$percentages]";
		}
		
		$values .= $details;
	}
}

if ( my $cmd = shift @ARGV )
{
	if ( my $sub = $commands{ $cmd } )
	{
		if ( my $control = $playback_control_map{ $cmd } )
		{
			if ( my @result = $sub->( $control, @ARGV ) )
			{
				my $result = join ", ", @result;
				
				print "$result\n";
			}
		}
		else
		{
			die "No such control '$cmd'\n";
		}
	}
	else
	{
		die "No such command '$cmd'\n";
	}
}
else
{
	warn "usage: $0 route\n";
	exit 2;
}

__END__
