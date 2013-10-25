package Compile::Driver::InputFile::Catalog;

use warnings FATAL => 'all';
use strict;


sub read_file
{
	my ( $path ) = @_;
	
	my %result;
	
	open my $in, "<", $path or die "$0: $path: $!\n";
	
	while ( my $line = <$in> )
	{
		chop $line;
		
		my ($name, $platform, $path) = split "\t", $line;
		
		if ( $platform eq "0/0" )
		{
			die "Duplicate $name\n" if exists $result{ $name };
		}
		
		my ( $for, $not ) = split "/", $platform;
		
		my %entry =
		(
			NAME => $name,
			FOR => $for + 0,
			NOT => $not + 0,
			PATH => $path,
		);
		
		push @{ $result{ $name } }, \%entry;
	}
	
	return \%result;
}

1;

