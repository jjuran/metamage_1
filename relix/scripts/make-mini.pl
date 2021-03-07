#!/usr/bin/perl

use warnings FATAL => 'all';
use strict;

use Fatal qw( close mkdir open chmod read rename );

use Relix::Archiver;

*splat = \&Relix::Archiver::splat;

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

my $build_tree   = $archiver->{ BUILD } . "/" . $archiver->{ LONG };
my $build_output = "$build_tree/bin";

-d $build_tree or die "Missing build tree at $build_tree\n";

my $relix_dist = $archiver->{ ROOT };

print "\$FILES  = $relix_files_dir\n";
print "\$BUILDS = $user_builds_dir\n";
print "\$OUTPUT = $build_output\n";
#print "\$TMP    = $tmp_subdir\n";
print "\$DIST   = $relix_dist\n";

sub splat_to_rsrc
{
	my ( $path, $contents, $type ) = @_;
	
	my $id = $archiver->next_id();
	
	my $dest_path = "$relix_dist/MacRelix/r/" . sprintf( "%.4x", $id ) . ".$type";
	
	splat( $dest_path, $contents );
	
	splat( "$dest_path/.~name", $path );
}

sub slurp
{
	my ( $path ) = @_;
	
	open my $in, "<", $path;
	
	my @lines = <$in>;
	
	return join "", @lines;
}

sub copy_to_rsrc
{
	my ( $path, $src, $type ) = @_;
	
	my $contents = slurp $src;
	
	$contents =~ s{^#!/usr/bin/env vx}{#!/usr/bin/minivx};
	
	splat_to_rsrc( $path, $contents, $type );
}

sub copy_script
{
	my ( $name, $path ) = @_;
	
	return $name => sub { copy_to_rsrc( $_[1], $path, 'Exec' ) }
}

my $v_dir = "$relix_files_dir/../../v";

sub copy_vx
{
	my ( $name ) = @_;
	
	my $path = "$v_dir/bin/$name.vx";
	
	return copy_script( $name, $path );
}

my %fsmap =
(
	bin =>
	[
		# Standard
		\ qw( cat cp echo false hostname kill ln login ls mkdir mv
		      ps pwd readlink realpath rm rmdir sh sleep sync true ),
		# Custom
		\ qw( teletype ),
		# Perl scripts
		qw( chmod date test ),
		# Varyx scripts
		qw( test-type ),
	],
	etc =>
	[
		qw( motd profile startup welcome ),
		{
			platform   => sub { splat_to_rsrc( $_[1], "$config_short_name\n", 'Data' ) unless $config_short_name eq 'xxx' },
			build_date => sub { splat_to_rsrc( $_[1], `date`, 'Data' ) },
		},
	],
	sbin => [{about => "about.vx"}],
	usr =>
	{
		bin =>
		[
			# Standard
			\ qw( clear env time touch ),
			# Common
			\ qw( perl ),
			# Varyx executor
			\ qw( minivx ),
			# App utilities
			\ qw( daemonize idle ),
			# Exceptions
			\ qw( abort addrerror buserror chk div0 illegal privileged trapv ),
			# Mac-specific
			\ qw( drvr sound-info system-info ),
			# Mac-specific scripts
			qw( gestalt ),
		],
	},
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

my $path = $archiver->install_umbrella_program( 'Genie/MacRelix', undef, $genie_build_tree );

$archiver->{ RESFS } = $path;

$archiver->create_node( ".", "", "." => \%fsmap );

my $program = "$relix_dist/MacRelix Mini";
my $archive = "$relix_dist/MacRelix-mini.mbin";

rename( $path, $program );

print "Archiving...\n";

Relix::Archiver::make_macbin( $program, $archive, \&verbose_system );

verbose_system( "cp", $archive, $archiver->archive_dir );

$archiver->cleanup;

print "Done.\n";

__END__
