package Compile::Driver::InputFile::Description;

use warnings FATAL => 'all';
use strict;


sub read_file
{
	my ( $path ) = @_;
	
	my %config;
	
	open my $in, "<", $path or die "$0: $path: $!\n";
	
	while ( my $line = <$in> )
	{
		chop $line;
		
		next if $line =~ m{^ \#? \s* $}x;
		
		my ( $key, $values ) = split /\s+/, $line, 2;
		
		my @values;
		
		if ( $values =~ m{^ " (.*) " $}x )
		{
			@values = $1;
		}
		else
		{
			@values = split /\s+/, $values;
		}
		
		push @{ $config{ $key } }, @values;
	}
	
	return \%config;
}

1;

