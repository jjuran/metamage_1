#!/usr/bin/perl

use warnings FATAL => 'all';
use strict;

BEGIN
{
	eval
	{
		require POSIX;
		
		POSIX::import( ':errno_h' );
		
		1;
	}
	or *POSIX::ENOTEMPTY = sub { 66 };
}

my $DIR = "/tmp/rename.t";

print "1..19\n";

my $n = 0;

sub ok
{
	my ( $ok ) = @_;
	
	my $msg = "ok " . ++$n . "\n";
	
	$msg = "not $msg" if !$ok;
	
	print $msg;
	
	return $ok;
}

sub read_dir
{
	my ( $dir ) = @_;
	
	opendir my( $d ), $dir or die;
	
	return grep { $_ !~ m{^ [.]{1,2} $}x } readdir( $d );
}

sub test_rename
{
	ok( -e "foo" );
	
	rename "foo", "bar" or die;
	
	ok( !-e "foo" );
	ok(  -e "bar" );
	
	ok rename "bar", "Bar" or warn "# rename: $!\n";
	
	ok( -e "Bar" );
	
	ok !grep { $_ ne "Bar" } read_dir( "." );
}

sub test_replace
{
	ok rename "Bar", "baz" or warn "# rename: $!\n";
	
	ok( !-e "Bar" );
	ok(  -e "baz" );
}

mkdir $DIR or die unless -d $DIR;
chdir $DIR or die;

# Rename a directory

mkdir "foo" or die;

test_rename();

mkdir "baz" or die;
mkdir "baz/bar" or die;

ok !rename( "Bar", "baz" )  &&  $! == POSIX::ENOTEMPTY or warn "# $!\n";

rmdir "baz/bar" or die;

test_replace();

rmdir "baz";

# Rename a regular file

{ open my $f, ">", "foo" or die; }

test_rename();

{ open my $f, ">", "baz" or die; }

test_replace();

unlink "baz";

chdir "/" and rmdir $DIR or die;

__END__
