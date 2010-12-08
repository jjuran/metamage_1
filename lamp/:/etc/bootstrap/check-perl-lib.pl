#!/usr/bin/perl -w

#use strict;  # Perl library may not be present yet

-d '/usr/lib/perl' and exit;

sub check
{
	my ( $path ) = @_;
	
	if ( -d $path )
	{
		symlink( $path, '/usr/lib/perl' ) or die "symlink: $!\n";
		
		exit;
	}
}

sub check_library
{
	my ( $path ) = @_;
	
	check( "$path/Application Support/MacRelix/usr/lib/perl" );
	check( "$path/Application Support/Lamp/usr/lib/perl"     );
}

check_library( "$ENV{HOME}/Library" )  if defined $ENV{HOME};

check_library( "/sys/mac/user/prefs/.." );

print "Run '/sbin/install-usr-lib-perl' to install the perl library.\n\n"

