package Compile::Driver::InputFile::SourceList;

use warnings FATAL => 'all';
use strict;


sub read_file
{
	my ( $path ) = @_;
	
	my @sources;
	
	open my $in, "<", $path or die "$0: $path: $!\n";
	
	while ( my $line = <$in> )
	{
		chop $line;
		
		$line =~ s{^ \s+ }{}x;
		
		next if $line =~ m{^ \# }x;
		
		push @sources, $line;
	}
	
	return @sources;
}

1;

