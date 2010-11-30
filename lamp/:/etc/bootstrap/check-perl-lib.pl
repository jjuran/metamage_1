#!/usr/bin/perl -w

#use strict;  # Perl library may not be present yet

-d '/usr/lib/perl' and exit;

if ( defined $ENV{HOME} )
{
	my $usr_lib_perl = "$ENV{HOME}/Library/Application Support/Lamp/usr/lib/perl";
	
	if ( -d $usr_lib_perl )
	{
		symlink( $usr_lib_perl, '/usr/lib/perl' ) or die "symlink: $!\n";
		
		exit;
	}
}

print "Run '/sbin/install-usr-lib-perl' to install the perl library.\n\n"

