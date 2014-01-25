#!/usr/bin/perl

use warnings FATAL => 'all';
use strict;

use Fatal qw( close mkdir open chmod read );

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
	Developer =>
	{
		Tests =>
		{
			jtest  => [ qw( ok.t oknot.t todo.t ) ],
			perl   => [ qw( print.t ) ],
			proc   => [ qw( exe.t   ) ],
			sh     => [ qw( braces.t errexit.t exit.t io.t vars.t ) ],
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
			                                capacity
			                                compare
			                                copy
			                                copyonwrite
			                                erase
			                                find
			                                insert
			                                replace
			                                substr
			                            ) ],
			"text-input" => [ \ map { "text-input-tests/$_" } qw( empty exceptions newlines ) ],
		},
		Tools =>
		[
			# Build tools
			\ qw( A-line ar cpres ld mpwrez mwcc postlink-68k-tool strip vers ),
			# Debugging
			\ qw( d68k demangle ),
			# Exceptions
			\ qw( abort buserror div0 illegal privileged ),
			# Mac-specific
			\ qw( SetFile aevt ic load-init rsrc-patch tlsrvr ),
			# AppleScript
			qw( system ),
			# aevt wrappers
			qw( File Line activate quit ),
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
	],
	etc =>
	[
		qw( inetd.conf motd profile startup traps welcome ),
		{
			#build_date => sub { verbose_system( "date > '$_[0]'" ) },
			platform => sub { verbose_system( "echo $config_short_name > $_[0]" ) unless $config_short_name eq 'xxx' },
			build_date => sub { verbose_system( "date > $_[0]" ) },
			#build_date => sub { open my $fh, '>', $_[0]; print $fh `date`; close $fh; },
			bootstrap => [qw( upgrade-lamp check-perl-lib.pl usr-lib-perl.mbin.gz.md5 )],
		},
	],
	home =>
	{
		jr => [],
	},
	sbin => [qw( about install-usr-lib-perl upgrade )],
	tmp => [],
	usr =>
	{
		bin =>
		[
			# Standard
			\ qw( clear env time touch ),
			# Common
			\ qw( git gzip htget killall md5sum nohup open osascript perl setleds tty which ),
			# djb's UCSPI
			\ qw( argv0 tcpcat tcpclient ),
			# v68k interpreter
			\ qw( xv68k ),
			# Modem-related scripts
			qw( cidlistener cidmon mcmd ),
			# Text translation
			\ qw( c cr2lf lf2cr lf2crlf mac2utf8 mread stripcr striplf utf82mac ),
			# Local editor
			\ qw( local-edit-client ),
			# Apps
			\ qw( buffer ),
			# Script apps
			qw( asklogin confirm progress prompt psmon ),
			# App utilities
			\ qw( daemonize follower idle overwrite select ),
			# Misc
			\ qw( chain copier divide jtest pause ptrace th ),
			# Mac-specific
			\ qw( beep cds err2text keymods macbin rominfo vols ),
			# Mac-specific scripts
			qw( drvr gestalt ),
			# Perl scripts
			qw( grep head printenv strings tee tr wc ),
		],
		lib =>
		{
			'git-core' =>
			{
				"git-receive-pack" => sub { symlink "../../bin/git", $_[0] },
				map { ("git-$_" => \ "git/$_") } qw( daemon upload-pack )
			},
			#perl => sub { copy_tree( '/usr/lib/perl', shift ); },
		},
		sbin => [ \ qw( superd inetd httpd ttyd ) ],
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

sub install_script
{
	my ( $name, $install_path ) = @_;
	
	my $path_from_root = $install_path;
	
	$path_from_root =~ s{^ .* /:/ }{}x;
	
	my $file = "$source_tree/$path_from_root/$name";
	
	-f $file || readlink $file or die "### Missing static file /$path_from_root/$name\n";
	
	copy_file( $file, $install_path );
	
	return if readlink $file;
	
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
	
	copy_file(  $output, $install_path );
}

sub create_node
{
	my ( $path, $dir, $param ) = @_;
	
	$path .= "/$dir"  unless $dir eq '.';
	
	my $ref = ref $param;
	
	if ( $ref eq "" )
	{
		return install_script( $param, $path );
	}
	
	if ( $ref eq "SCALAR" )
	{
		install_program( $$param, $path );
		
		return;
	}
	
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
	
	my $gz = "$tree_path.gz";
	
	rename( "$macbin.gz", $gz ) or die "$macbin.gz: $!\n";
	
	return $gz;
}


want_dir( $tmp_dir );

mkdir $tmp_subdir;

mkdir $lamp_dist;

# Genie is a different config than its programs on 68K
(my $genie_build_tree = $build_tree) =~ s/-a4-/-a5-/;

install_program( 'Genie/MacRelix', "$lamp_dist/", $genie_build_tree );

create_node( "$lamp_dist/:", "." => \%fsmap );

print "Archiving...\n";

my $macball = make_macball( "$tmp_subdir/$root_name" );

my $build_area_path = "$lamp_builds_dir/$build_area";

want_dirs( $build_area_path );

verbose_system( "cp", $macball, $build_area_path );

verbose_system( "rm", "-r", $tmp_subdir );

rmdir $tmp_dir unless $tmp_dir eq "/tmp";  # Clean up if empty; ignore errors

print "Done.\n";

