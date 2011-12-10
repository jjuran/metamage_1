package Compile::Driver::Files;

use warnings;
use strict;


sub list
{
	my ( $dir, $filter ) = @_;
	
	my @result;
	
	opendir my $d, $dir or die "$0: $dir: $!\n";
	
	while ( my $entry = readdir( $d ) )
	{
		next if substr( $entry, 0, 1 ) eq ".";  # no dotfiles or . / ..
		
		my $full_path = "$dir/$entry";
		
		if ( grep { $filter->() } $full_path )
		{
			push @result, $full_path;
		}
	}
	
	return @result;
}

sub enumerate
{
	my ( $dir, $filter ) = @_;
	
	my @result;
	
	opendir my $d, $dir or die "$0: $dir: $!\n";
	
	while ( my $entry = readdir( $d ) )
	{
		next if substr( $entry, 0, 1 ) eq ".";  # no dotfiles or . / ..
		
		my $full_path = "$dir/$entry";
		
		if ( -d $full_path )
		{
			push @result, enumerate( $full_path, $filter );
		}
		elsif ( grep { $filter->() } $full_path )
		{
			push @result, $full_path;
		}
	}
	
	return @result;
}

1;

