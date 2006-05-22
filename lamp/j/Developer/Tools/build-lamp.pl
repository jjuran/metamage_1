#!/usr/bin/perl

use warnings;
use strict;

use fatal 'mkdir';

$| = 1;  # piping hot output

die "\$HOME must be defined\n" if !exists $ENV{HOME};

my $build_area = shift || 'PPC-CFM-Carbon-Debug';

my $lamp_dir_path = "$ENV{HOME}/Developer/Lamp";

my $builds_dir_path      = "$lamp_dir_path/Builds";
my $native_root_dir_path = "$lamp_dir_path/j";

my $native_tmp_dir_path;

my $build_tree = "$ENV{HOME}/Developer/Lab/$build_area";
my $source_tree = "$ENV{HOME}/Developer/Projects/SourceForge/LAMP/j";

my $root_name = 'lamp_';

my @scripts = qw
(
	activate quit system
	ainstall File Line
	filter-mwlink-warnings
	filter-mwlink-warnings.pl
	build-lamp.pl
	chmod date ls tee tr
	
	profile
);
my %is_script = map { $_ => 1 } @scripts;

my %fsmap =
(
	Developer =>
	{
		Tools => [qw( aevt activate quit system File Line tlsrvr A-line cpres filter-mwlink-warnings filter-mwlink-warnings.pl build-lamp.pl )],
	},
	bin => [qw( true false login sh sleep kill pwd date echo cat chmod ls cp mv mkdir )],
	etc => [qw( profile )],
	sbin => [],
	tmp => [],
	usr =>
	{
		bin => [qw( beep env tee tr tty open osascript err2text htget aevt perl )],
		lib =>
		{
			perl => sub { copy_tree( '/usr/lib/perl', shift ); },
		},
		sbin => [qw( superd inetd httpd )],
	},
);


sub timestamp
{
	my $stamp = `date +"%Y%m%d-%H%M%S"`;
	
	chop $stamp;
	
	return $stamp;
}

sub run_native_command
{
	my ( $command, $cwd ) = @_;
	
	my $native_command = "system '$command'";
	
	if ( defined $cwd )
	{
		$native_command = "cd '$cwd'; " . $native_command;
	}
	
	#warn "$native_command\n";
	system $native_command;
}

sub copy_tree
{
	my ( $source, $dest ) = @_;
	
	$source = "..$source";
	$dest   = "..$dest";
	
	run_native_command( "ditto $source $dest", "/usr" );
}

sub want_dir
{
	my ( $dir ) = @_;
	
	return if -d $dir;
	
	mkdir $dir;
	
	return;
}

sub unique_dir_name
{
	my $stamp = timestamp();
	
	return "$stamp.$$";
}

sub build_output
{
	my ( $project ) = @_;
	
	my $result = "$build_tree/$project/Output/$project";
	
	-f $result or die "Missing build output for $project\n";
	
	return $result;
}

sub install_script
{
	my ( $name, $install_path ) = @_;
	
	my $path_from_root = $install_path;
	
	$path_from_root =~ s{^ .* /j/ }{}x;
	
	my $file = "$source_tree/$path_from_root/$name";
	
	-f $file or die "Missing static file /$path_from_root/$name\n";
	
	system 'cp', $file, $install_path;
}

sub install_program
{
	my ( $project, $install_path ) = @_;
	
	my $return = system 'cp', build_output( $project ), $install_path;
	
	die "Copy failed" if $return != 0;
}

sub create_file
{
	my ( $path, $file ) = @_;
	
	if ( $is_script{ $file } )
	{
		install_script( $file, $path );
	}
	else
	{
		install_program( $file, $path );
	}
}

sub create_node
{
	my ( $path, $dir, $param ) = @_;
	
	my $ref = ref $param or return;
	
	$path .= "/$dir";
	mkdir $path;
	
	if ( $ref eq "CODE" )
	{
		$param->( $path );
		return;
	}
	
	if ( $ref eq "ARRAY" )
	{
		foreach my $file ( @$param )
		{
			create_file( $path, $file );
		}
		
		return;
	}
	
	if ( $ref eq "HASH" )
	{
		while ( my ($key, $value) = each %$param )
		{
			create_node( $path, $key, $value );
		}
		
		return;
	}
	
	warn "Unrecognized $ref reference.\n";
	
	return;
}

sub make_tarball
{
	my ( $tree, $tmp_dir ) = @_;
	
	#my $tree    = "$native_tmp_dir_path/$root_name";
	my $tarball = "$tree.tar.gz";
	
	run_native_command( "tar czf $tarball $tree", $tmp_dir );
	
	return $tarball;
}

# This avoids a bug that squelches error output
print STDERR "Building...\n";
sleep 0;

$root_name .= timestamp();

want_dir( "/tmp" );

my $dir_name = unique_dir_name();
my $tmp_dir = "/tmp/$dir_name";
mkdir $tmp_dir;

$native_tmp_dir_path = $native_root_dir_path . $tmp_dir;

my $lamp = "$tmp_dir/$root_name";
mkdir $lamp;

install_program( 'Genie', "$lamp/" );

create_node( $lamp, 'j', \%fsmap );

my $tarball = make_tarball( $root_name, $tmp_dir );

my $build_area_path = "$builds_dir_path/$build_area";

mkdir $build_area_path;

rename "$tmp_dir", "$build_area_path/";
#rename "$tmp_dir/$root_name", "$build_area_path/$root_name";
#rename "$tmp_dir/$tarball",   "$build_area_path/$tarball";

print "Done.\n";

