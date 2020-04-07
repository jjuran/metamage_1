#!/usr/bin/env perl

#use warnings;
#use strict;


sub unpack_name
{
	my ( $name ) = @_;
	
	my ($parent, $prefix) = $name =~ m{^ ([0-9a-f]{2}) / ([0-9a-f]{24}) \xe2 \x80 \xa2 [0-9a-v]{6} $}x
		or die "$name: Not a munged Git object filename\n";
	
	return ($parent, $prefix);
}

sub reconstruct_hash
{
	my ( $path ) = @_;  # must relative to .git/objects
	
	my ($parent, $prefix) = unpack_name( $path );
	
	my $data = `zpipe -d < $path` or die "Decompression failed\n";
	
	my ( $type, $length ) = $data =~ m{^ ([a-z]+) [ ] ([0-9]+) \x00 }x;
	
	if ( length( $type ) + length( $length ) + 2 + $length != length( $data ) )
	{
		die "Length mismatch\n";
	}
	
	my $hash = `zpipe -d < $path | sha1` or die "Hashing failed\n";
	
	$hash = substr( $hash, 0, 40 );
	
	$hash =~ m{^ [0-9a-f]{40} $}x or die "Invalid hash string\n";
	
	die "Hash mismatch\n" if substr( $hash, 0,  2 ) ne $parent;
	die "Hash mismatch\n" if substr( $hash, 2, 24 ) ne $prefix;
	
	return $hash;
}

sub repair_file
{
	my ( $path ) = @_;
	
	my $hash = reconstruct_hash( $path );
	
	warn "$hash\n";
	
	$hash =~ s{^ (..) }{$1/}x;
	
	rename( $path, $hash ) or die "$path: $!\n";
}

sub repair_pack
{
	my ( $name ) = @_;
	
	my ( $prefix, $ext ) = $name =~ m{^ ( pack-[0-9a-f]{14} ) .* \. (idx|pack) }x or return;
	
	my $new_name = "$prefix.$ext";
	
	-f $new_name and warn "$name: $new_name exists\n";
	
	warn "$new_name\n";
	
	rename( $name, $new_name ) or die "$name: $!\n";
}

if ( @ARGV )
{
	repair_file( $_ ) for @ARGV;
	
	exit;
}

if ( -d ".git" )
{
	chdir ".git/objects" or die ".git/objects: $!\n";
	
	opendir( my $d, "." ) or die "$!\n";
	
	my @initials = grep { ! m{^ [.]{1,2} $}x } readdir $d;
	
	foreach my $initial ( grep { length( $_ ) == 2 } @initials )
	{
		opendir( my $init, $initial ) or die "$initial: $!\n";
		
		my @files = grep { ! m{^ [.]{1,2} $}x } readdir $init;
		
		foreach my $file ( grep { length( $_ ) != 38 } @files )
		{
			repair_file( "$initial/$file" );
		}
	}
	
	exit if ! -d "pack";
	
	chdir "pack" or die "pack: $!\n";
	
	opendir( $d, "." ) or die "$!\n";
	
	my @packs = grep { ! m{^ [.]{1,2} $}x } readdir $d;
	
	foreach my $pack ( @packs )
	{
		repair_pack( $pack );
	}
}
