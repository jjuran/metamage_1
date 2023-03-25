package Relix::Archiver;

use warnings FATAL => 'all';
use strict;

use Fatal qw( close mkdir open chmod read );

use FindBin '$RealBin';


my %supported_configs = qw
(
	68k   68k-a4-blue-opt
	os9   ppc-cfm-blue-opt
	osx   ppc-cfm-carb-opt
	68k~  68k-a4-blue-dbg
	os9~  ppc-cfm-blue-dbg
	osx~  ppc-cfm-carb-dbg
);

sub config_names
{
	my ( $arg ) = @_;
	
	if ( $arg =~ /^ \w{3} ~? $/x )
	{
		my $long = $supported_configs{ $arg }  ||  die "No such config '$arg'\n";
		
		return $arg => $long;
	}
	
	my %reverse = ( map { $supported_configs{ $_ }, $_ } keys %supported_configs );
	
	my $short = $reverse{ $arg }  ||  die "No such config '$arg'\n";
	
	return $short => $arg;
}

sub timestamp
{
	my ( $date ) = @_;
	
	my $stamp;
	
	if ( defined $date )
	{
		# reformat "YYYY-mm-dd HH:MM" as "YYYYmmdd-HHMM"
		
		$stamp = join '-', map { s{\D}{}g; $_ } split " ", $date;
	}
	else
	{
		$stamp = `/bin/date +"%Y%m%d-%H%M"`;
		
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
	
	system( "/bin/mkdir", "-p", $dir ) unless -d $dir;
	
	return;
}

sub build_output
{
	my ( $project, $build_tree ) = @_;
	
	my $project_file = $project =~ m{/} ? $project : "$project/$project";
	
	my $result = "$build_tree/bin/$project_file";
	
	-f $result or die "Missing build output for $project\n";
	
	return $result;
}

sub tmpdir
{
	my $root = "";
	
	if ( my $ramdisk = `ram-disks -1` )
	{
		chomp $ramdisk;
		
		$root = "$ramdisk/mnt";
	}
	
	"$root/tmp";
}

sub spew
{
	my ( $path, $contents ) = @_;
	
	open my $out, ">", $path;
	
	print $out $contents or die "$path: $!\n";
	
	close $out;
	
	return;
}

sub make_macbin
{
	my ( $tree_path, $archive_path, $system ) = @_;
	
	if ( !defined $archive_path )
	{
		my $ext = -d $tree_path ? 'mBin' : 'mbin';
		
		$archive_path = "$tree_path.$ext";
	}
	
	$system->( 'macbin', 'encode', $tree_path, $archive_path );
	
	return $archive_path;
}

sub make_macball
{
	my ( $tree_path, $system ) = @_;
	
	my $mbin = make_macbin( $tree_path, undef, $system );
	
	$system->( 'gzip', $mbin );
	
	my $gz = "$tree_path.gz";
	
	rename( "$mbin.gz", $gz ) or die "$mbin.gz: $!\n";
	
	return $gz;
}

sub pascal_string
{
	my ( $string ) = @_;
	
	my $len = length $string;
	
	die "Can't make Pascal string longer than 255 bytes" if $len > 255;
	
	return (chr $len) . $string;
}

sub make_vers
{
	my ( $short, $long ) = @_;
	
	return ("\0" x 6) . (pascal_string $short) . (pascal_string $long);
}

sub new
{
	my $class = shift;
	
	my ( $config, $date ) = @_;
	
	my %self;
	
	my ( $config_short_name, $long ) = config_names( $config );
	
	$self{ SHORT } = $config_short_name;
	$self{ LONG  } = $long;
	
	my $timestamp = timestamp( $date );
	
	if ( defined $date )
	{
		$self{ vers_1 } = make_vers( $date, "as of $date, by Josh Juran"     );
		$self{ vers_2 } = make_vers( "",    "MacRelix experimental snapshot" );
	}
	
	my $dir = $RealBin;
	
	(my $files = $dir) =~ s{ scripts $}{files}x;
	
	$self{ FILES } = $files;
	
	$self{ BUILD    } = "$ENV{ HOME }/var/build";
	$self{ ARCHIVES } = "$ENV{ HOME }/var/archive/MacRelix/Builds";
	
	my $tmp_dir = tmpdir();
	
	want_dir( $tmp_dir );
	
	$self{ TMP      } =  $tmp_dir;
	$self{ TMP_ROOT } = "$tmp_dir/$timestamp.$$";
	
	$self{ ROOT } = "$self{ TMP_ROOT }/relix-${config_short_name}_$timestamp";
	
	#(my $metamage = $files) =~ s{ /relix/files $}{}x;
	
	my $next_id = 128;
	
	$self{ NEXTID } = sub { $next_id++ };
	
	return bless \%self, $class;
}

sub next_id
{
	my $self = shift;
	
	return $self->{ NEXTID }();
}

sub make_dirs
{
	my $self = shift;
	
	mkdir $self->{ TMP_ROOT };
	mkdir $self->{ ROOT     };
}

sub copy_file
{
	my $self = shift;
	
	my ( $src, $dest ) = @_;
	
	my $system = $self->{ SYSTEM };
	
	-f $src || readlink $src or die "### Missing file $src for copy\n";
	
	$system->( 'cp', $src, $dest );
	
	if ( -d $dest )
	{
		my ( $name ) = $src =~ m{/([^/]*)$};
		
		$dest .= "/$name";
	}
	
	if ( defined $self->{ vers_1 }  &&  -d "$dest/r" )
	{
		spew( "$dest/r/0001.vers", $self->{ vers_1 } );
	}
	
	if ( defined $self->{ vers_2 }  &&  -d "$dest/r" )
	{
		spew( "$dest/r/0002.vers", $self->{ vers_2 } );
	}
	
	return;
}

sub copy_file_to_rsrc
{
	my $self = shift;
	
	my ( $src, $path_from_root, $type ) = @_;
	
	my $dest = $self->{ RESFS };
	
	my $id = $self->next_id();
	
	my $system = $self->{ SYSTEM };
	
	-f $src or die "### Missing file $src for copy\n";
	
	if ( -z $src )
	{
		$src = "$src/r/0000.Tool";
	}
	
	my $dest_path = "$dest/r/" . sprintf( "%.4x", $id ) . ".$type";
	
	$system->( 'cp', $src, $dest_path );
	
	spew( "$dest_path/.~name", $path_from_root );
}

sub is_script
{
	my ( $file ) = @_;
	
	open( my $fh, '<', $file );
	
	read( $fh, my $data, 1024 );
	
	$data =~ m{\r} and warn "### Script $file contains CR characters\n";
	
	my $shebang = substr( $data, 0, 2 );
	
	return $shebang eq '#!';
}

sub install_script
{
	my $self = shift;
	
	my ( $name, $install_path, $path_from_root, $dst ) = @_;
	
	my $relix_files = $self->{ FILES };
	
	my $file = "$relix_files/$path_from_root/$name";
	
	-f $file || readlink $file or die "### Missing static file /$path_from_root/$name\n";
	
	my $is_link = readlink $file;
	
	my $is_exec = !$is_link  &&  is_script( $file );
	
	if ( $self->{ RESFS } )
	{
		my $type = $is_link ? 'Link'
		         : $is_exec ? 'Exec'
		         :            'Data';
		
		$self->copy_file_to_rsrc( $file, "$path_from_root/$dst", $type );
		
		return;
	}
	
	$self->copy_file( $file, $install_path );
	
	if ( $is_exec )
	{
		if ( -d $install_path )
		{
			$install_path .= "/$name";
		}
		
		chmod 0700, $install_path;
	}
}

sub install_subprogram
{
	my $self = shift;
	
	my ( $project, $install_path, $path_from_root ) = @_;
	
	my $build_tree = $self->{ BUILD } . "/" . $self->{ LONG };
	
	my $output = build_output( $project, $build_tree );
	
	if ( $self->{ RESFS } )
	{
		$path_from_root .= "/$project";
		
		$self->copy_file_to_rsrc( $output, $path_from_root, 'Exec' );
	}
	else
	{
		$self->copy_file( $output, $install_path );
	}
}

sub install_umbrella_program
{
	my $self = shift;
	
	my ( $project, $install_path, $foreign_build_tree ) = @_;
	
	$install_path ||= $self->{ ROOT };
	
	$foreign_build_tree ||= $self->{ BUILD } . "/" . $self->{ LONG };
	
	my $output = build_output( $project, $foreign_build_tree );
	
	$self->copy_file( $output, $install_path );
	
	( my $name = $project ) =~ s{^ .* / }{}x;
	
	return "$install_path/$name";
}

sub create_node
{
	my $self = shift;
	
	my ( $path, $subpath, $dir, $param ) = @_;
	
	my $ref = ref $param;
	
	$path = $self->{ ROOT } . "/$path"  if $path !~ m{^ / }x;
	
	$subpath .= "/$dir"  unless $dir eq "." || $ref eq "";
	
	my $path_from_root = substr( $subpath, 1 )  if $subpath;  # drop leading '/'
	
	my $full_path = $path . $subpath;
	
	if ( $ref eq "" )
	{
		$full_path .= "/$dir"  unless $dir eq ".";
		
		my $src = $param;
		my $dst = $dir ne "." ? $dir : $src;
		
		return $self->install_script( $src, $full_path, $path_from_root, $dst );
	}
	
	if ( $ref eq "SCALAR" )
	{
		$self->install_subprogram( $$param, $full_path, $path_from_root );
		
		return;
	}
	
	if ( $ref eq "CODE" )
	{
		$param->( $full_path, $path_from_root );
		
		return;
	}
	
	want_dir( $full_path )  unless $self->{ RESFS };
	
	if ( $ref eq "ARRAY" )
	{
		foreach my $file ( @$param )
		{
			$self->create_node( $path, $subpath, '.', $file );
		}
		
		return;
	}
	
	if ( $ref eq "HASH" )
	{
		while ( my ($key, $value) = each %$param )
		{
			$self->create_node( $path, $subpath, $key, $value );
		}
		
		return;
	}
	
	warn "Unrecognized $ref reference.\n";
	
	return;
}

sub archive_dir
{
	my $self = shift;
	
	my $archive_dir = $self->{ ARCHIVES } . "/" .$self->{ LONG };
	
	want_dirs( $archive_dir );
	
	return $archive_dir;
}

sub cleanup
{
	my $self = shift;
	
	my $system = $self->{ SYSTEM };
	
	$system->( "rm", "-r", $self->{ TMP_ROOT } );
	
	rmdir $self->{ TMP } unless $self->{ TMP } eq "/tmp";  # Clean up if empty; ignore errors
}

sub finish_macbin
{
	my $self = shift;
	
	my $system = $self->{ SYSTEM };
	
	my $macbin = make_macbin( $self->{ ROOT }, undef $system );
	
	$system->( "cp", $macbin, $self->archive_dir );
	
	$self->cleanup();
}

sub finish_macball
{
	my $self = shift;
	
	my $system = $self->{ SYSTEM };
	
	my $macball = make_macball( $self->{ ROOT }, $system );
	
	$system->( "cp", $macball, $self->archive_dir );
	
	$self->cleanup();
}

1;
