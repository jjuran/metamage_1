package Compile::Driver::Job;

use FindBin '$RealBin';

use Compile::Driver::Files;
use Compile::Driver::Options;

use warnings FATAL => 'all';
use strict;


*make_ancestor_dirs = \&Compile::Driver::Files::make_ancestor_dirs;


# Assume we're called from the repo's top level
our $build_dir = "$RealBin/var/build";


sub new
{
	my $class = shift;
	
	my %self = @_;
	
	return bless \%self, $class;
}

sub derived_filename
{
	my ( $path ) = @_;
	
	my ( $subpath ) = $path =~ m{ // (.*) $}x;
	
	if ( $^O eq "msys" )
	{
		$subpath =~ s{/}{--}g;
	}
	else
	{
		$subpath =~ tr{/}{:};
	}
	
	return $subpath;
}

sub derived_pathname
{
	my ( $dir, $path, $extension ) = @_;
	
	my $filename = derived_filename( $path );
	
	return "$dir/$filename" . "$extension";
}

sub obj_pathname
{
	my ( $target, $project_name, $path ) = @_;
	
	my $obj_dir = "$build_dir/$target/obj/$project_name";
	
	return derived_pathname( $obj_dir, $path, ".o" );
}

sub lib_filename
{
	my ( $project_name ) = @_;
	
	return "lib$project_name.a";
}

sub lib_pathname
{
	my ( $target, $project_name ) = @_;
	
	my $libname = lib_filename( $project_name );
	
	return "$build_dir/$target/lib/$libname";
}

sub bin_pathname
{
	my ( $target, $project_name, $program_name ) = @_;
	
	my $binname = defined $program_name ? $program_name
	                                    : $project_name;
	
	return "$build_dir/$target/bin/$project_name/$binname";
}

sub up_to_date
{
	my $self = shift;
	
	my $out = $self->{DEST};
	
	-f $out or return 0;
	
	my @in = $self->input_files;
	
	my $out_date = (stat _)[9];
	
	foreach my $in ( @in )
	{
		my @stat = stat $in;
		
		return 0  if !@stat || $stat[9] > $out_date;
	}
	
	return 1;
}

sub imports
{
	return;
}

sub print
{
	my $self = shift;
	
	my $type = $self->{TYPE};
	my $dest = $self->{DEST};
	my $item = $self->{PATH} || $self->{FROM}->name;
	
	$type .= " " x (5 - length( $type ));
	
	$item =~ s{ .* // }{}x;
	
	print "$type $item\n";
}

sub run_command
{
	print join( " ", @_ ), "\n"  if Compile::Driver::Options::verbose();
	
	my $wait = system( @_ );
	
	if ( $wait != 0 )
	{
		die "wait status from command: $wait\n";
	}
}

sub perform
{
	my $self = shift;
	
	return ""  if $self->up_to_date;
	
	my @input = $self->input_files;
	
	my @command = ($self->command, @input, $self->imports);
	
	$self->print;
	
	my $dest = $self->{DEST};
	
	make_ancestor_dirs( $dest );
	
	run_command( @command );
}

1;
