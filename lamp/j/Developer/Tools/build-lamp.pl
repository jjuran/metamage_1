#!/usr/bin/perl

use warnings;
use strict;

use fatal qw( close mkdir open print chmod read );

local $| = 1;  # piping hot output

die "\$HOME must be defined\n" if !exists $ENV{HOME};

my %arches   = qw( m68k 68K  powerpc PPC );
my %runtimes = qw( cfm CFM  rsrc Res);
my %backends = qw( classic Toolbox  carbon Carbon );

my $arch    = $ENV{HOSTTYPE}    or die "Missing HOSTTYPE\n";
my $runtime = $ENV{MAC_RUNTIME} or die "Missing MAC_RUNTIME\n";
my $backend = $ENV{MAC_BACKEND} or die "Missing MAC_BACKEND\n";

my $build_config_name = join '-', $arches{$arch}, $runtimes{$runtime}, $backends{$backend}, 'Debug';

$build_config_name = shift || $build_config_name;  # e.g. 'PPC-CFM-Carbon-Debug'

my %supported_configs = qw
(
	68K-Res-Toolbox-Debug  68k
	PPC-CFM-Toolbox-Debug  std
	PPC-CFM-Carbon-Debug   osx
);

my $config_short_name = $supported_configs{$build_config_name} || 'xxx';

# This avoids a bug that squelches error output
print "Building for $build_config_name...\n";

my $build_area = $build_config_name;  # e.g. 'PPC-CFM-Carbon-Debug'

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
	A-line
	abort aevt argv0
	beep
	cat cds cp cpres cr2lf
	divide
	echo err2text
	false
	gzip
	htget httpd
	inetd
	jtest
	keymods kill killall
	lf2cr lf2crlf login
	macbin md5sum mkdir mv
	nohup
	open osascript
	perl pwd
	readlink rm rmdir
	sh sleep stripcr striplf superd
	tcpcat tcpclient th tlsrvr true tty
	uncaught_exception
);
my %is_program = map { $_ => 1 } @programs;

my %fsmap =
(
	Developer =>
	{
		Tools =>
		[
			qw( A-line abort activate aevt cpres keymods quit system File Line tlsrvr ),
			qw( filter-mwlink-warnings filter-mwlink-warnings.pl build-lamp.pl ),
		],
	},
	bin =>
	[
		qw( cat cp echo false kill login mkdir mv pwd readlink rm rmdir sh sleep true ),
		qw( chmod date ls test ),
	],
	etc =>
	[
		qw( inetd.conf motd profile ),
		{
			#build_date => sub { system "date > '$_[0]'" },
			#build_date => sub { open my $fh, '>', $_[0]; print $fh `date`; close $fh; },
			bootstrap => [qw( check-perl-lib.pl install-usr-lib-perl usr-lib-perl.mbin.gz.md5 )],
		},
	],
	sbin => [],
	tmp => [],
	usr =>
	{
		bin =>
		[
			qw( gzip htget killall md5sum nohup open osascript perl tty ),
			qw( argv0 tcpcat tcpclient ),
			qw( cidlistener cidmon mcmd ),
			qw( beep cds cr2lf divide err2text jtest keymods lf2cr lf2crlf macbin stripcr striplf th ),
			qw( env grep head printenv strings tee time tr wc ),
		],
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
	my ( $project, $foreign_build_tree ) = @_;
	
	$foreign_build_tree ||= $build_tree;
	
	my $result = "$foreign_build_tree/$project/Output/$project";
	
	-f $result or die "Missing build output for $project\n";
	
	return $result;
}

sub verbose_system
{
	print join( " ", @_ ), "\n";
	
	my $wait_status = system @_;
	
	$wait_status == 0 or die "system() failed: $!\n";
}

sub copy_file
{
	my ( $src, $dest ) = @_;
	
	-f $src or die "Missing file $src for copy\n";
	
	verbose_system( 'cp', $src, $dest );
	
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
	
	read( $fh, my $data, 1024 );
	
	$data =~ m{\r} and warn "Script /$path_from_root/$name contains CR characters\n";
	
	my $shebang = substr( $data, 0, 2 );
	
	chmod 0700, "$install_path/$name" if $shebang eq '#!';
}

sub install_program
{
	my ( $project, $install_path, $foreign_build_tree ) = @_;
	
	my $output = build_output( $project, $foreign_build_tree );
	
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
	
	my $macbin = "$tree_path.mbin";
	
	verbose_system 'macbin', '--encode', $tree_path, $macbin;
	
	verbose_system 'gzip', $macbin;
}


$root_name .= timestamp();

want_dir( "/tmp" );

my $dir_name = unique_dir_name();
my $tmp_dir = "/tmp/$dir_name";
mkdir $tmp_dir;

#$native_tmp_dir_path = $native_root_dir_path . $tmp_dir;

my $lamp = "$tmp_dir/$root_name";
mkdir $lamp;

# Genie is a different config than its programs on 68K
(my $genie_build_tree = $build_tree) =~ s/-Res-/-Code-/;

install_program( 'Genie', "$lamp/", $genie_build_tree );

create_node( $lamp, 'j' => \%fsmap );

print "Archiving...\n";

my $macball = make_macball( "$tmp_dir/$root_name" );

my $build_area_path = "$builds_dir_path/$build_area";

mkdir $build_area_path  if ! -d $build_area_path;

rename "$tmp_dir", "$build_area_path/$dir_name" or die "rename $tmp_dir $build_area_path/$dir_name failed: $!\n";

print "Done.\n";

