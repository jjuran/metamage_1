#!/usr/bin/perl

use FindBin '$RealBin';

use lib "$RealBin/tools/perl";

use Compile::Driver;
use Compile::Driver::Configuration;
use Compile::Driver::Jobs;
use Compile::Driver::Module;
use Compile::Driver::Options;

use warnings;
use strict;

my @args = Compile::Driver::Options::set_options( @ARGV );

my $configuration = Compile::Driver::Configuration::->new( Compile::Driver::Options::specs );

foreach my $name ( @args )
{
	my $module = $configuration->get_module( $name, "[mandatory]" );
}

my $desc = { NAME => "[program args]", DATA => { use => [ @args ] } };

my $module = Compile::Driver::Module::->new( $configuration, $desc );

my @prereqs = @{ $module->recursive_prerequisites };

pop @prereqs;

my @jobs = map { Compile::Driver::jobs_for( $configuration->get_module( $_ ) ) } @prereqs;

foreach my $job ( @jobs )
{
	Compile::Driver::Jobs::do_job( $job );
}

