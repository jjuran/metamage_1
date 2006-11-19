#!/usr/bin/perl

use warnings;
use strict;

use fatal qw( close mkdir open print chmod read );

$| = 1;  # piping hot output

die "\$HOME must be defined\n" if !exists $ENV{HOME};

my %arches   = qw( m68k 68K  powerpc PPC );
my %runtimes = qw( cfm CFM );
my %backends = qw( classic Toolbox  carbon Carbon );

my $arch    = $ENV{HOSTTYPE}    or die "Missing HOSTTYPE\n";
my $runtime = $ENV{MAC_RUNTIME} or die "Missing MAC_RUNTIME\n";
my $backend = $ENV{MAC_BACKEND} or die "Missing MAC_BACKEND\n";

my $build_config_name = join '-', $arches{$arch}, $runtimes{$runtime}, $backends{$backend}, 'Debug';

my %supported_configs = qw
(
	PPC-CFM-Toolbox-Debug  std
	PPC-CFM-Carbon-Debug   osx
);

my $config_short_name = $supported_configs{$build_config_name} || 'xxx';

sub Echo
{
	my ( $text ) = @_;
	
	print "$text\n";
	
	sleep 0;
	
	return;
}

# This avoids a bug that squelches error output
Echo "Building for $build_config_name...\n";

my $build_area = shift || $build_config_name;  # e.g. 'PPC-CFM-Carbon-Debug'

my $should_copy_syms = $ENV{LAMP_SYMS};

my $lamp_dir_path = "$ENV{HOME}/Developer/Lamp";

my $builds_dir_path      = "$lamp_dir_path/Builds";
#my $native_root_dir_path = "$lamp_dir_path/j";

#my $native_tmp_dir_path;

my $build_tree = "$ENV{HOME}/Developer/Lab/$build_area";
my $source_tree = "$ENV{HOME}/Developer/Projects/SourceForge/Lamp/j";

-d $build_tree or die "Missing build tree at $build_tree\n";

my $root_name = "lamp-${config_short_name}_";

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
	etc =>
	[
		qw( inetd.conf motd profile ),
		{
			#build_date => sub { system "date > '$_[0]'" },
			#build_date => sub { open my $fh, '>', $_[0]; print $fh `date`; close $fh; },
			bootstrap => [qw( check-perl-lib.pl install-usr-lib-perl )],
		},
	],
	sbin => [],
	tmp => [],
	usr =>
	{
		bin => [qw( argv0 beep err2text gzip htget macbin md5sum open osascript perl tty ), qw( env grep head printenv strings tee time tr wc )],
		lib =>
		{
			#perl => sub { copy_tree( '/usr/lib/perl', shift ); },
		},
		sbin => [qw( superd inetd httpd )],
	},
	var =>
	{
		www =>
		[
			qw( index.html help.html main.css cubes.gif ),
			{
				'cgi-bin' => [qw( motd )],
			},
		],
	}
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

sub copy_file
{
	my ( $src, $dest ) = @_;
	
	-f $src or die "Missing file $src for copy\n";
	
	my $wait_status = system( 'cp', $src, $dest );
	$wait_status == 0 or die "cp '$src' '$dest' failed: $!\n";
	
	return;
}

sub install_script
{
	my ( $name, $install_path ) = @_;
	
	my $path_from_root = $install_path;
	
	$path_from_root =~ s{^ .* /j/ }{}x;
	
	my $file = "$source_tree/$path_from_root/$name";
	
	-f $file or die "Missing static file /$path_from_root/$name\n";
	
	copy_file( $file, $install_path );
	
	open( my $fh, '<', $file );
	
	read( $fh, my $shebang, 2 );
	
	chmod 0700, "$install_path/$name" if $shebang eq '#!';
}

sub install_program
{
	my ( $project, $install_path ) = @_;
	
	my $output = build_output( $project );
	
	copy_file(  $output,       $install_path );
	copy_file( "$output.xSYM", $install_path )  if $should_copy_syms;
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
	
	return;
}

sub create_node
{
	my ( $path, $dir, $param ) = @_;
	
	#print "create_node( '$path', '$dir', '$param' )\n";
	
	my $ref = ref $param or return create_file( $path, $param );
	
	$path .= "/$dir"  unless $dir eq '.';
	
	if ( $ref eq "CODE" )
	{
		$param->( $path );
		return;
	}
	
	mkdir $path;
	
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

sub make_macball
{
	my ( $tree_path ) = @_;
	
	my $macbin = "$tree_path.bin";
	
	system 'macbin', '--encode', $tree_path, $macbin;
	
	system 'gzip', $macbin;
}


$root_name .= timestamp();

want_dir( "/tmp" );

my $dir_name = unique_dir_name();
my $tmp_dir = "/tmp/$dir_name";
mkdir $tmp_dir;

#$native_tmp_dir_path = $native_root_dir_path . $tmp_dir;

my $lamp = "$tmp_dir/$root_name";
mkdir $lamp;

install_program( 'Genie', "$lamp/" );

eval { create_node( $lamp, 'j' => \%fsmap ); };

if ( $@ )
{
	print $@;
	exit 1;
}

Echo "Archiving...";

my $macball = make_macball( "$tmp_dir/$root_name" );

my $build_area_path = "$builds_dir_path/$build_area";

mkdir $build_area_path;

rename "$tmp_dir", "$build_area_path/";

print "Done.\n";

