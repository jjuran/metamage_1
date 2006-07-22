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

my @programs = qw
(
	aevt tlsrvr A-line cpres
	cat cp echo false kill login mkdir mv pwd sh sleep true
	argv0 beep err2text gzip htget macbin md5sum open osascript perl tty
	superd inetd httpd
);
my %is_program = map { $_ => 1 } @programs;

my %fsmap =
(
	Developer =>
	{
		Tools => [qw( aevt activate quit system File Line tlsrvr A-line cpres filter-mwlink-warnings filter-mwlink-warnings.pl build-lamp.pl )],
	},
	bin => [qw( cat cp echo false kill login mkdir mv pwd sh sleep true ), qw( chmod date ls test )],
	etc => [qw( inetd.conf motd profile )],
	sbin => [],
	tmp => [],
	usr =>
	{
		bin => [qw( argv0 beep err2text gzip htget macbin md5sum open osascript perl tty ), qw( env grep head printenv strings tee time tr wc )],
		lib =>
		{
			#perl => sub { copy_tree( '/usr/lib/perl', shift ); },
		},
		sbin => [qw( install-usr-lib-perl superd inetd httpd )],
	},
);


sub timestamp
{
	my $stamp = `date +"%Y%m%d-%H%M"`;
	
	chop $stamp;
	
	return $stamp;
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
	
	if ( $is_program{ $file } )
	{
		install_program( $file, $path );
	}
	else
	{
		install_script( $file, $path );
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

sub make_macball
{
	my ( $tree_path ) = @_;
	
	my $macbin = "$tree_path.bin";
	
	system 'macbin', '--encode', $tree_path, $macbin;
	
	system 'gzip', $macbin;
}


sub Echo
{
	my ( $text ) = @_;
	
	print "$text\n";
	
	sleep 0;
	
	return;
}

# This avoids a bug that squelches error output
Echo "Building...";

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

Echo "Archiving...";

my $macball = make_macball( "$tmp_dir/$root_name" );

my $build_area_path = "$builds_dir_path/$build_area";

mkdir $build_area_path;

rename "$tmp_dir", "$build_area_path/";

print "Done.\n";

