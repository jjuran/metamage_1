#!/usr/bin/perl

use warnings FATAL => 'all';
use strict;

use Relix::Archiver;

local $| = 1;  # piping hot output

die "\$HOME must be defined\n" if !exists $ENV{HOME};

sub make_config
{
	# e.g. 'ppc-cfm-carb-dbg'
	
	my %arches   = qw( m68k 68k   powerpc ppc  );
	my %runtimes = qw( cfm  cfm   rsrc    a4   );
	my %backends = qw( blue blue  carbon  carb );
	
	my $arch    = $ENV{ HOSTTYPE    } or die "Missing HOSTTYPE\n";
	my $runtime = $ENV{ MAC_RUNTIME } or die "Missing MAC_RUNTIME\n";
	my $backend = $ENV{ MAC_BACKEND } or die "Missing MAC_BACKEND\n";
	
	return join '-', $arches{ $arch },
	                 $runtimes{ $runtime },
	                 $backends{ $backend },
	                 'dbg';
}

my $archiver = Relix::Archiver::->new( shift || make_config(), $ENV{ BUILD_DATE } );

my $config_short_name = $archiver->{ SHORT };
my $config_long_name  = $archiver->{ LONG  };

# This avoids a bug that squelches error output
print "Building for $config_long_name ($config_short_name)...\n";

my $relix_files_dir = $archiver->{ FILES };
my $user_builds_dir = $archiver->{ BUILD };

my $tmp_subdir = $archiver->{ TMP_ROOT };

my $build_tree      = $archiver->{ BUILD } . "/" . $archiver->{ LONG };
my $build_output    = "$build_tree/bin";

-d $build_tree or die "Missing build tree at $build_tree\n";

my $relix_dist = $archiver->{ ROOT };

print "\$FILES  = $relix_files_dir\n";
print "\$BUILDS = $user_builds_dir\n";
print "\$OUTPUT = $build_output\n";
#print "\$TMP    = $tmp_subdir\n";
print "\$DIST   = $relix_dist\n";

sub copy_script
{
	my ( $name, $path ) = @_;
	
	return $name => sub
	{
		$archiver->copy_file( $path, $_[0] );
		
		chmod 0700, $_[0];
	}
}

my $v_dir = "$relix_files_dir/../../v";

sub copy_vx
{
	my ( $name ) = @_;
	
	my $path = "$v_dir/bin/$name.vx";
	
	return copy_script( $name, $path );
}

sub copy_vy
{
	my ( $name ) = @_;
	
	$name .= '.vy';
	
	my $path = "$v_dir/lib/$name";
	
	return $name => sub { $archiver->copy_file( $path, $_[0] ) };
}

my %fsmap =
(
	Developer =>
	{
		Tests =>
		{
			jtest  => [ qw( ok.t oknot.t todo.t ) ],
			perl   => [ qw( print.t ) ],
			proc   => [ qw( exe.t   ) ],
			sh     => [ qw( braces.t errexit.t exit.t io.t tilde.t vars.t ) ],
			perm   => [ \ qw( test-write-locked ) ],
			signal => [ \ qw( test-read-intr ) ],
			time   => [ \ qw( test-time ) ],
			plus         => [ \ map { "plus-tests/$_" }
			                        "concat_strings",
			                        "mac_utf8",
			                        "utf8",
			                        map { "string_$_" }
			                            qw
			                            (
			                                alloc
			                                basics
			                                compare
			                                copy
			                                copyonwrite
			                                c_str
			                                erase
			                                find
			                                insert
			                                length
			                                move
			                                replace
			                                substr
			                            ) ],
			"text-input" => [ \ map { "text-input-tests/$_" } qw( empty exceptions newlines ) ],
		},
		Tools =>
		[
			# Build tools
			\ qw( A-line ar cpres ld mpwrez mwcc mxcpp
				postlink-68k-drvr postlink-68k-standalone
				postlink-68k-tool strip ),
			# Debugging
			\ qw( d68k demangle ),
			# Functionality testing
			\ qw( clock_getrestime fifo-read fifo-write ),
			# Exceptions
			\ qw( abort buserror div0 illegal privileged trapv ),
			# Mac-specific
			\ qw( SetFile aevt ic load-init mwnub rsrc-patch tlsrvr ),
			# AppleScript
			qw( system ),
			# aevt wrappers
			qw( File Line activate quit ),
			# Git repository repair
			qw( gitfix.pl ),
			# Misc scripts
			qw( filter-mwlink-warnings.pl ramdisk.pl report run-tests strip-all ),
		],
	},
	bin =>
	[
		# Standard
		\ qw( cat cp echo false hostname kill ln login ls mkdir mv ps pwd readlink realpath rm rmdir sh sleep sync true ),
		# Custom
		\ qw( teletype ),
		# Perl scripts
		qw( chmod date test ),
		# Varyx scripts
		qw( test-type ),
	],
	etc =>
	[
		qw( inetd.conf motd profile startup traps welcome ),
		{
			#build_date => sub { verbose_system( "date > '$_[0]'" ) },
			platform => sub { verbose_system( "echo $config_short_name > $_[0]" ) unless $config_short_name eq 'xxx' },
			build_date => sub { verbose_system( "date > $_[0]" ) },
			#build_date => sub { open my $fh, '>', $_[0]; print $fh `date`; close $fh; },
			bootstrap => [qw( check-perl-lib.pl usr-lib-perl.mbin.gz.md5 )],
		},
	],
	sbin => [{about => "about.vx", upgrade => "upgrade.vx"}, "install-usr-lib-perl"],
	tmp => [],
	usr =>
	{
		bin =>
		[
			# Standard
			\ qw( clear env mkfifo time touch ),
			# Common
			\ qw( git gzip htget killall md5sum nohup open osascript perl setleds sha1sum sha256sum tty which ),
			# Git utilities
			{
				sha1 => \ "git/test-sha1",
			},
			# Ed25519
			\ qw( ed-mkpub ed-sign ed-verify ),
			# Graphics
			\ qw( skif skif-info ),
			# unet
			\ qw( uexec utcp uunix ),
			# Freemount
			\ qw( fcat fdir fget fls fping fstat ),
			# djb's UCSPI
			\ qw( argv0 tcpcat tcpclient unixclient ),
			# Varyx calculator / executor
			\ qw( macvx minivx vc vx ),
			# v68k interpreter
			\ qw( xv68k ),
			# Modem-related scripts
			qw( cidlistener cidmon mcmd ),
			# Text translation
			\ qw( c cr2lf lf2cr lf2crlf mac2utf8 mread stripcr striplf utf82mac ),
			# Apps
			\ qw( buffer ),
			# App engines
			\ qw( icon-editor ),
			# Script apps
			qw( asklogin iconedit mon prompt ),
			{
				confirm  => "confirm.vx",
				progress => "progress.vx",
			},
			# App utilities
			\ qw( daemonize follower idle overwrite select stream ),
			# Misc
			\ qw( chain copier divide graft jtest pause ptrace th zpipe ),
			# Mac-specific
			\ qw( adbs beep cds crsr deicon drives drvr err2text gdev keymods
				location mac-id macbin mnam qdscreencap rom-checksum rominfo
				screenBits system-info thds units vcbs vols ),
			# Mac-specific scripts
			qw( gestalt ),
			# Perl scripts
			qw( grep head printenv strings tee tr wc ),
			# Varyx scripts
			{
				copy_vx( 'arcsign' ),
			},
		],
		lib =>
		{
			'git-core' =>
			{
				"git-receive-pack" => sub { symlink "../../bin/git", $_[0] },
				map { ("git-$_" => \ "git/$_") } qw( daemon upload-pack )
			},
			v =>
			{
				map { copy_vy $_ }
					qw( arcsign arcsign-seal arcsign-unseal date-time options ),
			},
			#perl => sub { copy_tree( '/usr/lib/perl', shift ); },
		},
		sbin => [ \ qw( superd inetd freemountd httpd listen ttyd ) ],
		share =>
		{
			'git-core' =>
			{
				templates =>
				{
					info => [qw( exclude )],
				},
			},
		},
	},
	var =>
	{
		freemount =>
		[
			'README',
			{
				www => sub { symlink "../www", $_[0] }
			},
		],
		root => [],
		www =>
		[
			qw( index.html help.html main.css cubes.gif ),
			{
				'cgi-bin' => [qw( motd )],
			},
		],
	}
);


sub verbose_system
{
	my $command = join( " ", @_ );
	
	$command =~ s{$build_output}{\$OUTPUT}o;
	
	$command =~ s{$relix_files_dir}{\$FILES}o;
	$command =~ s{$user_builds_dir}{\$BUILDS}o;
	
	$command =~ s{$relix_dist}{\$DIST}og;
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


$archiver->{ SYSTEM } = \&verbose_system;

$archiver->make_dirs();

# Genie is a different config than its programs on 68K
(my $genie_build_tree = $build_tree) =~ s/-a4-/-a5-/;

$archiver->install_umbrella_program( 'Genie/MacRelix', undef, $genie_build_tree );

$archiver->create_node( ":", "", "." => \%fsmap );

print "Archiving...\n";

$archiver->finish_macball();

print "Done.\n";

__END__
