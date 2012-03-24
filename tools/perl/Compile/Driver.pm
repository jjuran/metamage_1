package Compile::Driver;

use Compile::Driver::Configuration;
use Compile::Driver::Job;
use Compile::Driver::Module;
use Compile::Driver::Options;

use warnings;
use strict;


sub jobs_for
{
	my ( $module ) = @_;
	
	my @jobs;
	
	my $target = $module->target;
	
	my $product = $module->product_type;
	
	my $is_lib = $module->is_static_lib;
	my $is_exe = $module->is_executable;
	
	my $type = $is_lib ? "AR"
	         : $is_exe ? "LINK"
	         :           "";
	
	return if !$type;
	
	my @sources = $module->sources;
	
	my $name = $module->name;
	
	my @objs;
	
	foreach my $path ( @sources )
	{
		my $obj = Compile::Driver::Job::obj_pathname( $target, $name, $path );
		
		push @objs, $obj;
		
		my $compile = Compile::Driver::Job::->new
		(
			TYPE => "CC",
			FROM => $module,
			PATH => $path,
			DEST => $obj,
		);
		
		push @jobs, $compile;
	}
	
	my $dest = $is_lib ? Compile::Driver::Job::lib_pathname( $target, $name )
	                   : Compile::Driver::Job::bin_pathname( $target, $name, $module->program_name );
	
	my $link = Compile::Driver::Job::->new
	(
		TYPE => $type,
		FROM => $module,
		OBJS => \@objs,
		DEST => $dest,
	);
	
	$link->{PREQ} = $module->recursive_prerequisites  if $is_exe;
	
	push @jobs, $link;
	
	return @jobs;
}

sub main
{
	my @args = Compile::Driver::Options::set_options( @_ );
	
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
		$job->perform;
	}
	
}

1;

