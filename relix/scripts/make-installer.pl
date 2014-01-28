#!/usr/bin/perl

use warnings FATAL => 'all';
use strict;

use Fatal qw( close mkdir open chmod read rename );

local $| = 1;  # piping hot output

die "\$HOME must be defined\n" if !exists $ENV{HOME};

my %arches   = qw( m68k 68k   powerpc ppc  );
my %runtimes = qw( cfm  cfm   rsrc    a4   );
my %backends = qw( blue blue  carbon  carb );

my $arch    = $ENV{HOSTTYPE}    or die "Missing HOSTTYPE\n";
my $runtime = $ENV{MAC_RUNTIME} or die "Missing MAC_RUNTIME\n";
my $backend = $ENV{MAC_BACKEND} or die "Missing MAC_BACKEND\n";

my $build_config_name = join '-', $arches{$arch}, $runtimes{$runtime}, $backends{$backend}, 'dbg';

$build_config_name = shift || $build_config_name;  # e.g. 'ppc-cfm-carb-dbg'

my %supported_configs = qw
(
	68k-a4-blue-opt   68k
	ppc-cfm-blue-opt  os9
	ppc-cfm-carb-opt  osx
	68k-a4-blue-dbg   68k~
	ppc-cfm-blue-dbg  os9~
	ppc-cfm-carb-dbg  osx~
);

my $config_short_name = $supported_configs{$build_config_name} || 'xxx';

if ( $build_config_name =~ /^ \w{3} ~? $/x )
{
	$config_short_name = $build_config_name;
	
	my %reverse_configs = ( map { $supported_configs{$_}, $_ } keys %supported_configs );
	
	$build_config_name = $reverse_configs{$build_config_name} || die "No such config '$build_config_name'\n";
}

# This avoids a bug that squelches error output
print "Building for $build_config_name...\n";

my $build_area = $build_config_name;  # e.g. 'ppc-cfm-carb-dbg'

my $timestamp = timestamp();

my $lamp_source_dir = "$ENV{HOME}/src/tree/metamage/lamp";
my $user_builds_dir = "$ENV{HOME}/var/build";
my $user_lamp_dir   = "$ENV{HOME}/var/archive/MacRelix";

my $tmp_dir = tmpdir();

my $unique_dir_name = "$timestamp.$$";
my $tmp_subdir = "$tmp_dir/$unique_dir_name";

my $source_tree     = "$lamp_source_dir/:";
my $build_tree      = "$user_builds_dir/$build_area";
my $build_output    = "$build_tree/bin";
my $lamp_builds_dir = "$user_lamp_dir/Builds";

-d $build_tree or die "Missing build tree at $build_tree\n";

my $root_name = "relix-${config_short_name}_$timestamp";

my $lamp_dist = "$tmp_subdir/$root_name";

print "\$LAMP   = $lamp_source_dir\n";
print "\$BUILDS = $user_builds_dir\n";
print "\$OUTPUT = $build_output\n";
#print "\$TMP    = $tmp_subdir\n";
print "\$DIST   = $lamp_dist\n";

my $vers_1_data;
my $vers_2_data;

if ( defined $ENV{BUILD_DATE} )
{
	$vers_1_data = `vers "" "as of $ENV{BUILD_DATE}, by Josh Juran"`;
	$vers_2_data = `vers "" "MacRelix experimental snapshot"`;
}

my %fsmap =
(
	bin =>
	[
		# Standard
		\ qw( cat echo false ln mkdir mv rm rmdir sh sync true ),
		# Perl scripts
		qw( chmod date test ),
	],
	etc =>
	[
		{
			startup => sub { spew_to_rsrc( $_[0], <<'[END]', 'Exec' ) },
#!/bin/sh

exec /usr/app/installer/init
[END]
			platform => sub { spew_to_rsrc( $_[0], "$config_short_name\n", 'Data' ) unless $config_short_name eq 'xxx' },
			build_date => sub { spew_to_rsrc( $_[0], `date`, 'Data' ) },
			bootstrap => [qw( upgrade-lamp )],
		},
	],
	sbin => [qw( about upgrade )],
	usr =>
	{
		app =>
		{
			installer => [qw( init main )],
		},
		bin =>
		[
			# Standard
			\ qw( touch ),
			# Common
			\ qw( gzip htget perl ),
			# Script apps
			qw( confirm progress ),
			# App utilities
			\ qw( daemonize idle select ),
			# Mac-specific
			\ qw( macbin ),
		],
	},
);


my $next_id = 128;

sub next_id
{
	return $next_id++;
}

sub spew
{
	my ( $path, $contents ) = @_;
	
	open my $out, ">", $path;
	
	print $out $contents or die "$path: $!\n";
	
	close $out;
	
	return;
}

sub timestamp
{
	my $stamp;
	
	if ( defined $ENV{BUILD_DATE} )
	{
		$stamp = join '-', map { s{\D}{}g; $_ } split " ", $ENV{BUILD_DATE};
	}
	else
	{
		$stamp = `date +"%Y%m%d-%H%M"`;
		
		chop $stamp;
	}
	
	return $stamp;
}

sub want_dir
{
	my ( $dir ) = @_;
	
	mkdir $dir unless -d $dir;
	
	return;
}

sub want_dirs
{
	my ( $dir ) = @_;
	
	system( "mkdir", "-p", $dir ) unless -d $dir;
	
	return;
}

sub build_output
{
	my ( $project, $foreign_build_tree ) = @_;
	
	$foreign_build_tree ||= $build_tree;
	
	my $project_file = $project =~ m{/} ? $project : "$project/$project";
	
	my $result = "$foreign_build_tree/bin/$project_file";
	
	-f $result or die "Missing build output for $project\n";
	
	return $result;
}

sub verbose_system
{
	my $command = join( " ", @_ );
	
	$command =~ s{$build_output}{\$OUTPUT}o;
	
	$command =~ s{$lamp_source_dir}{\$LAMP}o;
	$command =~ s{$user_builds_dir}{\$BUILDS}o;
	
	$command =~ s{$lamp_dist}{\$DIST}og;
	#$command =~ s{$tmp_subdir}{\$TMP}og;
	
	if ( $command =~ m/^cp / )
	{
		my $space = " " x 60;
		
		$command =~ s{ (\$\S+)$}{$space $1}o;
		
		$command =~ s[^ (.{60}) \s*][$1]x;
	}
	
	print "$command\n";
	
	my $wait_status = system @_;
	
	$wait_status == 0 or die "### system() failed: $!\n";
}

sub copy_file
{
	my ( $src, $dest ) = @_;
	
	-f $src || readlink $src or die "### Missing file $src for copy\n";
	
	verbose_system( 'cp', $src, $dest );
	
	my ( $name ) = $src =~ m{/([^/]*)$};
	
	if ( defined $vers_1_data  &&  -d "$dest/$name/r" )
	{
		open my $out, ">", "$dest/$name/r/0001.vers" or die "$dest/$name/r/0001.vers: $!\n";
		
		print $out $vers_1_data;
		
		close $out;
	}
	
	if ( defined $vers_2_data  &&  -d "$dest/$name/r" )
	{
		open my $out, ">", "$dest/$name/r/0002.vers" or die "$dest/$name/r/0002.vers: $!\n";
		
		print $out $vers_2_data;
		
		close $out;
	}
	
	return;
}

sub spew_to_rsrc
{
	my ( $path, $contents, $type ) = @_;
	
	my $id = next_id();
	
	my $dest_path = "$lamp_dist/MacRelix/r/" . sprintf( "%.4x", $id ) . ".$type";
	
	spew( $dest_path, $contents );
	
	spew( "$dest_path/.~name", $path );
}

sub copy_file_to_rsrc
{
	my ( $src, $path_from_root, $type ) = @_;
	
	-f $src or die "### Missing file $src for copy\n";
	
	$src =~ m{ ( / [^/]+ ) $}x;
	
	$path_from_root .= $1;
	
	if ( -z $src )
	{
		$src = "$src/r/0000.Wish";
	}
	
	my $id = next_id();
	
	my $dest_path = "$lamp_dist/MacRelix/r/" . sprintf( "%.4x", $id ) . ".$type";
	
	verbose_system( 'cp', $src, $dest_path );
	
	spew( "$dest_path/.~name", $path_from_root );
}

sub install_script
{
	my ( $name, $path_from_root ) = @_;
	
	my $file = "$source_tree/$path_from_root/$name";
	
	-f $file or die "### Missing static file /$path_from_root/$name\n";
	
	open( my $fh, '<', $file );
	
	read( $fh, my $data, 1024 );
	
	$data =~ m{\r} and warn "### Script /$path_from_root/$name contains CR characters\n";
	
	my $shebang = substr( $data, 0, 2 );
	
	my $type = $shebang eq '#!' ? 'Exec' : 'Data';
	
	copy_file_to_rsrc( $file, $path_from_root, $type );
}

sub install_subprogram
{
	my ( $project, $install_path ) = @_;
	
	my $output = build_output( $project );
	
	copy_file_to_rsrc( $output, $install_path, 'Exec' );
}

sub install_umbrella_program
{
	my ( $project, $install_path, $foreign_build_tree ) = @_;
	
	my $output = build_output( $project, $foreign_build_tree );
	
	copy_file(  $output, $install_path );
}

sub create_node
{
	my ( $path, $dir, $param ) = @_;
	
	$path .= ("/" x !!length $path) . $dir  unless $dir eq '.';
	
	my $ref = ref $param;
	
	if ( $ref eq "" )
	{
		$path =~ s{^ .* /:/ }{}x;
		
		return install_script( $param, $path );
	}
	
	if ( $ref eq "SCALAR" )
	{
		$path =~ s{^ .* /:/ }{}x;
		
		install_subprogram( $$param, $path );
		
		return;
	}
	
	if ( $ref eq "CODE" )
	{
		$path =~ s{^ .* /:/ }{}x;
		
		$param->( $path );
		
		return;
	}
	
	if ( $ref eq "ARRAY" )
	{
		foreach my $file ( @$param )
		{
			create_node( $path, '.', $file );
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

sub tmpdir
{
	my $root = `vols --ram`;

	chomp $root;
	
	$root .= "/mnt" if length $root;
	
	"$root/tmp";
}

sub make_archive
{
	my ( $tree_path, $archive_path ) = @_;
	
	if ( !defined $archive_path )
	{
		my $ext = -d $tree_path ? 'mBin' : 'mbin';
		
		my $archive_path = "$tree_path.$ext";
	}
	
	verbose_system 'macbin', '--encode', $tree_path, $archive_path;
	
	return $archive_path;
}


want_dir( $tmp_dir );

mkdir $tmp_subdir;

mkdir $lamp_dist;

# Genie is a different config than its programs on 68K
(my $genie_build_tree = $build_tree) =~ s/-a4-/-a5-/;

install_umbrella_program( 'Genie/MacRelix', "$lamp_dist/", $genie_build_tree );

create_node( "$lamp_dist/:", "." => \%fsmap );

my $installer = "$lamp_dist/MacRelix Installer";
my $archive   = "$lamp_dist/MacRelix-installer.mbin";

rename( "$lamp_dist/MacRelix", $installer );

print "Archiving...\n";

make_archive( $installer, $archive );

my $build_area_path = "$lamp_builds_dir/$build_area";

want_dirs( $build_area_path );

verbose_system( "cp", $archive, $build_area_path );

verbose_system( "rm", "-r", $tmp_subdir );

rmdir $tmp_dir unless $tmp_dir eq "/tmp";  # Clean up if empty; ignore errors

print "Done.\n";

