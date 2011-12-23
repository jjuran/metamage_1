package Compile::Driver::Jobs;

use FindBin '$RealBin';

use Compile::Driver::Files;
use Compile::Driver::InputFile::CPPSource;

use warnings;
use strict;


*make_ancestor_dirs = \&Compile::Driver::Files::make_ancestor_dirs;

*read_cpp_source_file = \&Compile::Driver::InputFile::CPPSource::read_file;


# Assume we're called from the repo's top level
my $build_dir = "$RealBin/var/build";


sub derived_filename
{
	my ( $path ) = @_;
	
	my ( $subpath ) = $path =~ m{ // (.*) $}x;
	
	$subpath =~ tr{/}{:};
	
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
	my $out = shift @_;
	
	-f $out or return 0;
	
	my @in = @_;
	
	my $out_date = (stat _)[9];
	
	foreach my $in ( @in )
	{
		my @stat = stat $in;
		
		return 0  if !@stat || $stat[9] > $out_date;
	}
	
	return 1;
}

my %includes_cache;

sub get_includes
{
	my ( $path ) = @_;
	
	exists $includes_cache{ $path } and return $includes_cache{ $path };
	
	my $includes = read_cpp_source_file( $path );
	
	return $includes_cache{ $path } = $includes;
}

sub up_to_date_for_headers
{
	my ( $module, $out_date, @headers ) = @_;
	
	foreach my $h ( @headers )
	{
		my @stat = stat $h;
		
		return 0  if !@stat || $stat[9] >= $out_date;
		
		my $includes = get_includes( $h );
		
		$includes = $includes->{USER};
		
		my @headers = grep { defined } map { $module->find_include( $_ ) } @$includes;
		
		return 0  if !up_to_date_for_headers( $module, $out_date, @headers );
	}
	
	return 1;
}

sub up_to_date_including_headers
{
	my ( $module, $out, @in ) = @_;
	
	-f $out or return 0;
	
	my $out_date = (stat _)[9];
	
	return up_to_date_for_headers( $module, $out_date, @in );
}

sub print_job
{
	my ( $job ) = @_;
	
	my $type = $job->{TYPE};
	my $dest = $job->{DEST};
	my $item = $job->{PATH} || $job->{FROM}->name;
	
	$type .= " " x (5 - length( $type ));
	
	$item =~ s{ .* // }{}x;
	
	print "$type $item\n";
}

sub run_command
{
	my $wait = system( @_ );
	
	if ( $wait != 0 )
	{
		die "wait status from command: $wait\n";
	}
}

sub compile
{
	my ( $job ) = @_;
	
	my $module = $job->{FROM};
	
	my $path = $job->{PATH};
	my $dest = $job->{DEST};
	
	return ""  if up_to_date_including_headers( $module, $dest, $path );
	
	my $conf = $module->{CONF};
	
	print_job( $job );
	
	make_ancestor_dirs( $dest );
	
	my @o = -O2;
	
	my @f;
	
	if ( $conf->is_apple_gcc )
	{
		push @f, "-fpascal-strings";
	}
	
	my %d;
	
	$d{ TARGET_CONFIG_DEBUGGING } = $conf->debugging + 0;
	
	my @d = map { "-D$_=" . $d{ $_ } } keys %d;
	
	my @i = map { "-I$_" } @{ $module->all_search_dirs };
	
	run_command( qw( gcc -c -o ), $dest, @o, @f, @d, @i, $path );
}

sub link_lib
{
	my ( $job ) = @_;
	
	my $module = $job->{FROM};
	
	my $objs = $job->{OBJS};
	my $dest = $job->{DEST};
	
	$job->{PATH} = lib_filename( $module->name );
	
	return ""  if up_to_date( $dest, @$objs );
	
	print_job( $job );
	
	make_ancestor_dirs( $dest );
	
	unlink( $dest );
	
	run_command( qw( ar rcs ), $dest, @$objs );
}

sub link_exe
{
	my ( $job ) = @_;
	
	my $module = $job->{FROM};
	
	my $objs = $job->{OBJS};
	my $preq = $job->{PREQ};
	my $dest = $job->{DEST};
	
	my @prereqs = grep { $module->get( $_ )->is_static_lib } @$preq;
	
	my @libs = map { lib_pathname( $module->target, $_ ) } @prereqs;
	
	return ""  if up_to_date( $dest, @$objs, @libs );
	
	@libs = reverse @libs;
	
	print_job( $job );
	
	make_ancestor_dirs( $dest );
	
	run_command( qw( g++ -o ), $dest, @$objs, @libs );
}

1;

