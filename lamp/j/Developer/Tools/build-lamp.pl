#!/usr/bin/perl

use warnings;
use strict;

use Fatal qw( close mkdir open chmod read );

local $| = 1;  # piping hot output

die "\$HOME must be defined\n" if !exists $ENV{HOME};

my %arches   = qw( m68k 68K  powerpc PPC );
my %runtimes = qw( cfm CFM  rsrc Res);
my %backends = qw( blue Blue  carbon Carbon );

my $arch    = $ENV{HOSTTYPE}    or die "Missing HOSTTYPE\n";
my $runtime = $ENV{MAC_RUNTIME} or die "Missing MAC_RUNTIME\n";
my $backend = $ENV{MAC_BACKEND} or die "Missing MAC_BACKEND\n";

my $build_config_name = join '-', $arches{$arch}, $runtimes{$runtime}, $backends{$backend}, 'Debug';

$build_config_name = shift || $build_config_name;  # e.g. 'PPC-CFM-Carbon-Debug'

my %supported_configs = qw
(
	68K-Res-Blue-Release    68k
	PPC-CFM-Blue-Release    std
	PPC-CFM-Carbon-Release  osx
	68K-Res-Blue-Debug      68k@
	PPC-CFM-Blue-Debug      std@
	PPC-CFM-Carbon-Debug    osx@
);

my $config_short_name = $supported_configs{$build_config_name} || 'xxx';

if ( $build_config_name =~ /^ \w{3} @? $/x )
{
	$config_short_name = $build_config_name;
	
	my %reverse_configs = ( map { $supported_configs{$_}, $_ } keys %supported_configs );
	
	$build_config_name = $reverse_configs{$build_config_name} || die "No such config '$build_config_name'\n";
}

# This avoids a bug that squelches error output
print "Building for $build_config_name...\n";

my $build_area = $build_config_name;  # e.g. 'PPC-CFM-Carbon-Debug'

my $should_copy_syms = $ENV{LAMP_SYMS};

my $timestamp = timestamp();

my $lamp_source_dir = "$ENV{HOME}/src/tree/synced/metamage/lamp";
my $user_builds_dir = "$ENV{HOME}/Developer/Builds";
my $user_lamp_dir   = "$ENV{HOME}/Developer/Lamp";

my $tmp_dir = tmpdir();

my $unique_dir_name = "$timestamp.$$";
my $tmp_subdir = "$tmp_dir/$unique_dir_name";

my $source_tree     = "$lamp_source_dir/j";
my $build_tree      = "$user_builds_dir/$build_area";
my $build_output    = "$build_tree/Output";
my $lamp_builds_dir = "$user_lamp_dir/Builds";

-d $build_tree or die "Missing build tree at $build_tree\n";

my $root_name = "lamp-${config_short_name}_$timestamp";

my $lamp_dist = "$tmp_subdir/$root_name";

print "\$LAMP   = $lamp_source_dir\n";
print "\$BUILDS = $user_builds_dir\n";
print "\$OUTPUT = $build_output\n";
#print "\$TMP    = $tmp_subdir\n";
print "\$DIST   = $lamp_dist\n";

my @programs = qw
(
	A-line
	SetFile
	abort aevt ar argv0
	beep buffer buserror
	cat cds chain cp cpres cr2lf
	daemonize divide
	echo env err2text
	false follower
	gzip
	htget httpd
	ic idle illegal inetd
	jgetty jsync jtest
	keymods kill killall
	ld lf2cr lf2crlf ln load-init local-edit-client login
	macbin md5sum mkdir mpwrez mread mv mwcc
	nohup
	open osascript
	pause perl privileged ps ptrace pwd
	readlink realpath rm rmdir rsrc-patch
	select setleds sh sleep stripcr striplf superd sync
	tcpcat tcpclient
	test-longjmp-past-vfork test-read-intr test-time test-write-locked
	th time tlsrvr touch true tty ttyd
	uncaught_exception
	vols
	which
);
my %is_program = map { $_ => 1 } @programs;

my %fsmap =
(
	Developer =>
	{
		Tests =>
		{
			jtest  => [ qw( ok.t oknot.t todo.t ) ],
			perl   => [ qw( print.t ) ],
			proc   => [ qw( exe.t   ) ],
			sh     => [ qw( braces.t errexit.t exit.t io.t vars.t ) ],
			perm   => [ qw( test-write-locked ) ],
			signal => [ qw( test-longjmp-past-vfork test-read-intr ) ],
			time   => [ qw( test-time ) ],
		},
		Tools =>
		[
			# Build tools
			qw( A-line ar cpres ld mpwrez mwcc ),
			# Source management
			qw( jsync ),
			# Exceptions
			qw( abort buserror illegal privileged ),
			# Mac-specific
			qw( SetFile aevt ic load-init rsrc-patch system tlsrvr ),
			# aevt wrappers
			qw( File Line activate quit ),
			# Misc scripts
			qw( filter-mwlink-warnings.pl build-lamp.pl report run-tests ),
		],
	},
	bin =>
	[
		# Standard
		qw( cat cp echo false kill ln login mkdir mv ps pwd readlink realpath rm rmdir sh sleep sync true ),
		# Custom
		qw( jgetty ),
		# Perl scripts
		qw( chmod date ls test ),
	],
	etc =>
	[
		qw( inetd.conf motd profile ),
		{
			#build_date => sub { verbose_system( "date > '$_[0]'" ) },
			platform => sub { verbose_system( "echo $config_short_name > $_[0]" ) unless $config_short_name eq 'xxx' },
			build_date => sub { verbose_system( "date > $_[0]" ) },
			#build_date => sub { open my $fh, '>', $_[0]; print $fh `date`; close $fh; },
			bootstrap => [qw( upgrade upgrade-lamp check-perl-lib.pl install-usr-lib-perl usr-lib-perl.mbin.gz.md5 )],
		},
	],
	sbin => [],
	tmp => [],
	usr =>
	{
		bin =>
		[
			# Standard
			qw( touch ),
			# Common
			qw( gzip htget killall md5sum nohup open osascript perl setleds tty which ),
			# djb's UCSPI
			qw( argv0 tcpcat tcpclient ),
			# Modem-related scripts
			qw( cidlistener cidmon mcmd ),
			# Newline translation
			qw( cr2lf lf2cr lf2crlf mread stripcr striplf ),
			# Local editor
			qw( local-edit-client ),
			# Apps
			qw( buffer confirm psmon ),
			# App utilities
			qw( daemonize follower idle select ),
			# Misc
			qw( chain divide jtest pause ptrace th ),
			# Mac-specific
			qw( beep cds drvr err2text gestalt keymods macbin vols ),
			# Perl scripts
			qw( env grep head printenv strings tee time tr wc ),
		],
		lib =>
		{
			#perl => sub { copy_tree( '/usr/lib/perl', shift ); },
		},
		sbin => [qw( superd inetd httpd ttyd )],
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
	
	mkdir $dir unless -d $dir;
	
	return;
}

sub build_output
{
	my ( $project, $foreign_build_tree ) = @_;
	
	$foreign_build_tree ||= $build_tree;
	
	my $result = "$foreign_build_tree/Output/$project/$project";
	
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
	
	-f $src or die "### Missing file $src for copy\n";
	
	verbose_system( 'cp', $src, $dest );
	
	return;
}

sub install_script
{
	my ( $name, $install_path ) = @_;
	
	my $path_from_root = $install_path;
	
	$path_from_root =~ s{^ .* /j/ }{}x;
	
	my $file = "$source_tree/$path_from_root/$name";
	
	-f $file or die "### Missing static file /$path_from_root/$name\n";
	
	copy_file( $file, $install_path );
	
	open( my $fh, '<', $file );
	
	read( $fh, my $data, 1024 );
	
	$data =~ m{\r} and warn "### Script /$path_from_root/$name contains CR characters\n";
	
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
	
	want_dir( $path );
	
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

sub make_macball
{
	my ( $tree_path ) = @_;
	
	my $macbin = "$tree_path.mBin";
	
	verbose_system 'macbin', '--encode', $tree_path, $macbin;
	
	verbose_system 'gzip', $macbin;
	
	my $gz = "$macbin.gz";
}


want_dir( tmpdir() );

mkdir $tmp_subdir;

mkdir $lamp_dist;

# Genie is a different config than its programs on 68K
(my $genie_build_tree = $build_tree) =~ s/-Res-/-Code-/;

install_program( 'Genie', "$lamp_dist/", $genie_build_tree );

create_node( $lamp_dist, 'j' => \%fsmap );

print "Archiving...\n";

my $macball = make_macball( "$tmp_subdir/$root_name" );

my $build_area_path = "$lamp_builds_dir/$build_area";

want_dir( $build_area_path );

verbose_system( "cp", $macball, $build_area_path );

verbose_system( "rm", "-r", $tmp_subdir );

rmdir $tmp_dir unless $tmp_dir eq "/tmp";  # Clean up if empty; ignore errors

print "Done.\n";

