#!/usr/bin/perl

use warnings FATAL => 'all';
use strict;

use FindBin '$RealBin';

my $metamage = "$RealBin/../..";


sub which
{
	my ( $cmd ) = @_;
	
	my @dirs = split ':', ($ENV{PATH} || "");
	
	foreach my $dir ( @dirs )
	{
		my $path = "$dir/$cmd";
		
		return $path  if -x $path;
	}
	
	return;
}

$ENV{NEXUS_ROOT} or die "$0: NEXUS_ROOT is unset\n";
$ENV{NEXUS_PEER} or die "$0: NEXUS_PEER is unset\n";

which "mxcpp" or die "$0: mxcpp not found\n";

which "ccache" and $ENV{CC} = "ccache-mwcc";

$ENV{SDK_DIR} ||= "$ENV{HOME}/opt/relix/SDK";

$ENV{NEXUS_HOST} ||= (substr `hostname -s`, 0, -1);

my $n_jobs = $ENV{NEXUS_JOBS} || 3;

$ENV{PATH}="$metamage/scripts/nexus:$metamage/bin:$ENV{PATH}";

my @argv = ("A-line", "--cpp", "-j$n_jobs", @ARGV);

system @argv;
